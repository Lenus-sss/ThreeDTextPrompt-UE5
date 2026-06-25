// 版权所有 Epic Games, Inc. 保留所有权利。

#include "ThreeDTextPromptActor.h"

#include "Camera/PlayerCameraManager.h"
#include "Characters/Text3DCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Text3DComponent.h"
#include "Text3DTypes.h"

/** 在 Text3D 提供字符前使用零初始化的运行时数据。 */
FThreeDTextPromptCharacterState::FThreeDTextPromptCharacterState()
{
}

/** 创建 Text3D 根组件，并在动画开始前关闭 Tick。 */
AThreeDTextPromptActor::AThreeDTextPromptActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	TextComponent = CreateDefaultSubobject<UText3DComponent>(TEXT("Text3D"));
	SetRootComponent(TextComponent);
}

/** 确保生成后的空闲 Actor 不会在 StartPrompt 准备数据前 Tick。 */
void AThreeDTextPromptActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
}

/** 移除该 Actor 持有的 Text3D 生成回调。 */
void AThreeDTextPromptActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TextComponent && TextGeneratedDelegateHandle.IsValid())
	{
		TextComponent->OnTextGenerated().Remove(TextGeneratedDelegateHandle);
		TextGeneratedDelegateHandle.Reset();
	}

	Super::EndPlay(EndPlayReason);
}

/** 应用参数，并等待 Text3D 回报已生成的字符数据。 */
void AThreeDTextPromptActor::StartPrompt(const FText& InText, const FThreeDTextPromptOptions& InOptions)
{
	Options = InOptions;
	Characters.Reset();
	bPromptReady = false;
	DestroyAfterAge = 0.0f;
	SetActorTickEnabled(false);

	if (!TextComponent)
	{
		Destroy();
		return;
	}

	if (TextGeneratedDelegateHandle.IsValid())
	{
		TextComponent->OnTextGenerated().Remove(TextGeneratedDelegateHandle);
		TextGeneratedDelegateHandle.Reset();
	}

	TextGeneratedDelegateHandle = TextComponent->OnTextGenerated().AddUObject(this, &AThreeDTextPromptActor::HandleTextGenerated);
	ApplyTextOptions(InText);
}

/** 返回该提示 Actor 使用的 Text3D 组件。 */
UText3DComponent* AThreeDTextPromptActor::GetTextComponent() const
{
	return TextComponent;
}

/** 更新所有已生成字符，并在停留时间结束后销毁 Actor。 */
void AThreeDTextPromptActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bPromptReady || !GetWorld())
	{
		return;
	}

	// PromptAge 从字形准备完成时开始计算，避免生成耗时影响动画节奏。
	const float PromptAge = GetWorld()->GetTimeSeconds() - PromptStartTime;

	if (Options.bFacePlayerCamera)
	{
		UpdateCameraFacing();
	}

	for (FThreeDTextPromptCharacterState& CharacterState : Characters)
	{
		UpdateCharacter(CharacterState, PromptAge);
	}

	UpdateLifetime(PromptAge);
}

/** 在生成文本前将调用方参数写入 Text3D。 */
void AThreeDTextPromptActor::ApplyTextOptions(const FText& InText)
{
	// 先配置稳定的几何参数，最后设置文本，确保生成字形使用最终设置。
	if (Options.Font)
	{
		TextComponent->SetFont(Options.Font);
	}

	TextComponent->SetFontSize(Options.FontSize);
	TextComponent->SetExtrude(Options.Extrude);
	TextComponent->SetBevel(Options.Bevel);
	TextComponent->SetKerning(Options.Kerning);
	TextComponent->SetHorizontalAlignment(EText3DHorizontalTextAlignment::Center);
	TextComponent->SetVerticalAlignment(EText3DVerticalTextAlignment::Center);

	if (Options.FrontMaterial)
	{
		TextComponent->SetFrontMaterial(Options.FrontMaterial);
	}
	if (Options.BevelMaterial)
	{
		TextComponent->SetBevelMaterial(Options.BevelMaterial);
	}
	if (Options.ExtrudeMaterial)
	{
		TextComponent->SetExtrudeMaterial(Options.ExtrudeMaterial);
	}
	if (Options.BackMaterial)
	{
		TextComponent->SetBackMaterial(Options.BackMaterial);
	}

	TextComponent->SetText(InText);
}

/** 捕获已生成字符、隐藏它们，并启动逐字显示时间线。 */
void AThreeDTextPromptActor::HandleTextGenerated()
{
	if (!TextComponent || !GetWorld())
	{
		Destroy();
		return;
	}

	Characters.Reset();

	// Text3D 拥有每个字符；该 Actor 只保存弱引用和基础布局变换。
	const uint16 CharacterCount = TextComponent->GetCharacterCount();
	Characters.Reserve(CharacterCount);

	for (uint16 Index = 0; Index < CharacterCount; ++Index)
	{
		UText3DCharacterBase* Character = TextComponent->GetCharacter(Index);
		if (!Character)
		{
			continue;
		}

		FThreeDTextPromptCharacterState State;
		State.Character = Character;
		State.BaseLocation = Character->GetRelativeLocation();
		State.RevealTime = static_cast<float>(Characters.Num()) * FMath::Max(0.0f, Options.CharacterDelay);
		State.bSoundPlayed = false;

		Character->SetVisibility(false);
		Character->SetRelativeLocation(State.BaseLocation);
		Character->SetRelativeScale(FVector::OneVector * FMath::Max(0.0f, Options.StartScale));

		Characters.Add(State);
	}

	PromptStartTime = GetWorld()->GetTimeSeconds();
	DestroyAfterAge = (Characters.Num() > 0 ? Characters.Last().RevealTime : 0.0f) + FMath::Max(0.0f, Options.HoldTimeAfterReveal);
	bPromptReady = true;
	SetActorTickEnabled(true);
}

/** 计算紧凑的弹跳曲线，并应用可见性、缩放和垂直偏移。 */
void AThreeDTextPromptActor::UpdateCharacter(FThreeDTextPromptCharacterState& CharacterState, float PromptAge) const
{
	UText3DCharacterBase* Character = CharacterState.Character.Get();
	if (!Character)
	{
		return;
	}

	if (PromptAge < CharacterState.RevealTime)
	{
		Character->SetVisibility(false);
		return;
	}

	if (!CharacterState.bSoundPlayed)
	{
		Character->SetVisibility(true);
		PlayCharacterSound();
		CharacterState.bSoundPlayed = true;
	}

	// 第一段放大到超过最终尺寸，第二段回落到 1.0，同时正弦位移归零。
	const float LocalAge = PromptAge - CharacterState.RevealTime;
	const float Duration = FMath::Max(0.01f, Options.PopDuration);
	const float Alpha = FMath::Clamp(LocalAge / Duration, 0.0f, 1.0f);
	const float Split = 0.6f;
	const float SafeStartScale = FMath::Max(0.0f, Options.StartScale);
	const float SafeOvershootScale = FMath::Max(0.0f, Options.OvershootScale);

	float Scale = 1.0f;
	if (Alpha < Split)
	{
		const float GrowAlpha = FMath::InterpEaseOut(0.0f, 1.0f, Alpha / Split, 3.0f);
		Scale = FMath::Lerp(SafeStartScale, SafeOvershootScale, GrowAlpha);
	}
	else
	{
		const float SettleAlpha = FMath::InterpEaseInOut(0.0f, 1.0f, (Alpha - Split) / (1.0f - Split), 2.0f);
		Scale = FMath::Lerp(SafeOvershootScale, 1.0f, SettleAlpha);
	}

	const float JumpOffset = FMath::Max(0.0f, Options.JumpHeight) * FMath::Sin(Alpha * UE_PI);
	Character->SetVisibility(true);
	Character->SetRelativeLocation(CharacterState.BaseLocation + FVector(0.0f, 0.0f, JumpOffset));
	Character->SetRelativeScale(FVector::OneVector * Scale);
}

/** 在 Actor 位置播放逐字符音效。 */
void AThreeDTextPromptActor::PlayCharacterSound() const
{
	if (Options.PopSound && GetWorld())
	{
		UGameplayStatics::PlaySoundAtLocation(this, Options.PopSound, GetActorLocation(), Options.SoundVolume, Options.SoundPitch);
	}
}

/** 使用第一个玩家摄像机作为朝向目标，但不改变位置。 */
void AThreeDTextPromptActor::UpdateCameraFacing()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!CameraManager)
	{
		return;
	}

	const FVector FromCamera = GetActorLocation() - CameraManager->GetCameraLocation();
	if (!FromCamera.IsNearlyZero())
	{
		SetActorRotation(FromCamera.Rotation());
	}
}

/** 所有字符出现且停留时间结束后销毁提示。 */
void AThreeDTextPromptActor::UpdateLifetime(float PromptAge)
{
	if (PromptAge >= DestroyAfterAge)
	{
		Destroy();
	}
}
