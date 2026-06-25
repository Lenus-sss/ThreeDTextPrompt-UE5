// 版权所有 Epic Games, Inc. 保留所有权利。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThreeDTextPromptTypes.h"
#include "ThreeDTextPromptActor.generated.h"

class UText3DCharacterBase;
class UText3DComponent;

/** 单个已生成 Text3D 字符的运行时状态。 */
struct THREEDTEXTPROMPT_API FThreeDTextPromptCharacterState
{
	/** 在 Text3D 生成结果写入前保存默认状态。 */
	FThreeDTextPromptCharacterState();

	/** Text3D 内部持有的字符对象。 */
	TWeakObjectPtr<UText3DCharacterBase> Character;

	/** Text3D 布局阶段生成的稳定本地位置。 */
	FVector BaseLocation = FVector::ZeroVector;

	/** 该字符相对提示开始时间的出现偏移。 */
	float RevealTime = 0.0f;

	/** 标记该字符的弹出音效是否已经播放。 */
	bool bSoundPlayed = false;
};

/** 渲染带厚度 3D 文字，并用弹出动画逐字显示的 Actor。 */
UCLASS(BlueprintType)
class THREEDTEXTPROMPT_API AThreeDTextPromptActor : public AActor
{
	GENERATED_BODY()

public:
	/** 创建 Text3D 组件，并只在提示激活时启用 Tick。 */
	AThreeDTextPromptActor();

	/** 使用传入文本和参数开始渲染并播放动画。 */
	UFUNCTION(BlueprintCallable, Category = "3D Text Prompt")
	void StartPrompt(const FText& InText, const FThreeDTextPromptOptions& InOptions);

	/** 返回底层 Text3D 组件，便于原生代码进行高级定制。 */
	UText3DComponent* GetTextComponent() const;

protected:
	/** 在 StartPrompt 配置文本和运行时数据前禁用处理。 */
	virtual void BeginPlay() override;

	/** 更新逐字显示时序、弹出动画、朝向和生命周期。 */
	virtual void Tick(float DeltaSeconds) override;

	/** 在销毁前移除该 Actor 持有的原生委托。 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** 应用 Text3D 的几何、字体、材质和间距参数。 */
	void ApplyTextOptions(const FText& InText);

	/** 在 Text3D 生成字形后构建逐字符运行时数据。 */
	void HandleTextGenerated();

	/** 推进单个已生成字符的动画状态。 */
	void UpdateCharacter(FThreeDTextPromptCharacterState& CharacterState, float PromptAge) const;

	/** 在提示位置播放配置的弹出音效。 */
	void PlayCharacterSound() const;

	/** 在需要时将 Actor 转向第一个本地玩家摄像机。 */
	void UpdateCameraFacing();

	/** 完整提示显示足够时间后结束 Actor 生命周期。 */
	void UpdateLifetime(float PromptAge);

	/** 持有生成的 3D 文字及其字符数据的组件。 */
	UPROPERTY(VisibleAnywhere, Category = "3D Text Prompt")
	TObjectPtr<UText3DComponent> TextComponent;

	/** 当前提示实例从调用方捕获的参数。 */
	UPROPERTY(Transient)
	FThreeDTextPromptOptions Options;

	/** 按视觉顺序保存每个已生成字符的运行时数据。 */
	TArray<FThreeDTextPromptCharacterState> Characters;

	/** 生成字符准备好开始动画时的世界时间。 */
	float PromptStartTime = 0.0f;

	/** Actor 应自毁时对应的提示年龄。 */
	float DestroyAfterAge = 0.0f;

	/** Text3D 已生成字符且动画可运行时为 true。 */
	bool bPromptReady = false;

	/** 用于从 Text3D 生成事件中注销的委托句柄。 */
	FDelegateHandle TextGeneratedDelegateHandle;
};

