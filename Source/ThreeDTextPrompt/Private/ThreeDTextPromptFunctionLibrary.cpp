// 版权所有 Epic Games, Inc. 保留所有权利。

#include "ThreeDTextPromptFunctionLibrary.h"

#include "Engine/World.h"
#include "ThreeDTextPromptActor.h"
#include "ThreeDTextPromptSettings.h"

/** 在解析出的世界中生成并启动 3D 文字提示。 */
AThreeDTextPromptActor* UThreeDTextPromptFunctionLibrary::Show3DTextPrompt(UObject* WorldContextObject, const FText& Text, const FVector Location, const FRotator Rotation, const FThreeDTextPromptOptions& Options)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	const UThreeDTextPromptSettings* Settings = GetDefault<UThreeDTextPromptSettings>();
	TSubclassOf<AThreeDTextPromptActor> ActorClass = AThreeDTextPromptActor::StaticClass();
	if (Settings && Settings->PromptActorClass)
	{
		ActorClass = Settings->PromptActorClass;
	}
	const FThreeDTextPromptOptions MergedOptions = MergeOptionsWithDefaults(Options);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AThreeDTextPromptActor* PromptActor = World->SpawnActor<AThreeDTextPromptActor>(ActorClass, Location, Rotation, SpawnParameters);
	if (PromptActor)
	{
		PromptActor->StartPrompt(Text, MergedOptions);
	}

	return PromptActor;
}

/** 仅使用项目设置生成并启动 3D 文字提示。 */
AThreeDTextPromptActor* UThreeDTextPromptFunctionLibrary::Show3DTextPromptWithDefaults(UObject* WorldContextObject, const FText& Text, const FVector Location, const FRotator Rotation)
{
	const UThreeDTextPromptSettings* Settings = GetDefault<UThreeDTextPromptSettings>();
	const FThreeDTextPromptOptions Options = Settings ? Settings->DefaultOptions : FThreeDTextPromptOptions();
	return Show3DTextPrompt(WorldContextObject, Text, Location, Rotation, Options);
}

/** 将默认资源和调用方时序参数合并为一组提示参数。 */
FThreeDTextPromptOptions UThreeDTextPromptFunctionLibrary::MergeOptionsWithDefaults(const FThreeDTextPromptOptions& Options)
{
	const UThreeDTextPromptSettings* Settings = GetDefault<UThreeDTextPromptSettings>();
	if (!Settings)
	{
		return Options;
	}

	// 从调用方参数开始合并，确保数值字段始终尊重 C++ 显式传入的结构体。
	FThreeDTextPromptOptions Result = Options;
	const FThreeDTextPromptOptions& Defaults = Settings->DefaultOptions;

	if (!Result.Font)
	{
		Result.Font = Defaults.Font;
	}
	if (!Result.FrontMaterial)
	{
		Result.FrontMaterial = Defaults.FrontMaterial;
	}
	if (!Result.BevelMaterial)
	{
		Result.BevelMaterial = Defaults.BevelMaterial;
	}
	if (!Result.ExtrudeMaterial)
	{
		Result.ExtrudeMaterial = Defaults.ExtrudeMaterial;
	}
	if (!Result.BackMaterial)
	{
		Result.BackMaterial = Defaults.BackMaterial;
	}
	if (!Result.PopSound)
	{
		Result.PopSound = Defaults.PopSound;
	}

	return Result;
}
