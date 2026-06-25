// 版权所有 Epic Games, Inc. 保留所有权利。

#pragma once

#include "CoreMinimal.h"
#include "ThreeDTextPromptTypes.generated.h"

class UFont;
class UMaterialInterface;
class USoundBase;

/** 用于生成并驱动单个 3D 文字提示的运行时参数。 */
USTRUCT(BlueprintType)
struct THREEDTEXTPROMPT_API FThreeDTextPromptOptions
{
	GENERATED_BODY()

	/** 使用引擎默认值，让调用方只覆盖需要调整的字段。 */
	FThreeDTextPromptOptions();

	/** 可选字体资源；显示中文时请使用支持中文的字体。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
	TObjectPtr<UFont> Font = nullptr;

	/** 可选正面材质，用于字形的可见正面。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	TObjectPtr<UMaterialInterface> FrontMaterial = nullptr;

	/** 可选倒角材质，用于字形边缘的圆角或斜角部分。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	TObjectPtr<UMaterialInterface> BevelMaterial = nullptr;

	/** 可选挤出材质，用于字形厚度侧面。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	TObjectPtr<UMaterialInterface> ExtrudeMaterial = nullptr;

	/** 可选背面材质，用于字形背面。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	TObjectPtr<UMaterialInterface> BackMaterial = nullptr;

	/** 可选音效，每个字符出现时播放一次。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TObjectPtr<USoundBase> PopSound = nullptr;

	/** Text3D 字号，单位为 Unreal 单位。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text", meta = (ClampMin = "0.1"))
	float FontSize = 48.0f;

	/** 字形厚度，单位为 Unreal 单位。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry", meta = (ClampMin = "0.0"))
	float Extrude = 8.0f;

	/** 倒角宽度，单位为 Unreal 单位。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry", meta = (ClampMin = "0.0"))
	float Bevel = 0.5f;

	/** 生成字符之间的额外间距。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
	float Kerning = 0.0f;

	/** 相邻两个字符依次出现的时间间隔。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (ClampMin = "0.0"))
	float CharacterDelay = 0.08f;

	/** 单个字符弹出动画的持续时间。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (ClampMin = "0.01"))
	float PopDuration = 0.28f;

	/** 弹出动画中的本地 Z 轴跳起高度。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (ClampMin = "0.0"))
	float JumpHeight = 24.0f;

	/** 字符刚显示时使用的初始缩放。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (ClampMin = "0.0"))
	float StartScale = 0.1f;

	/** 弹出时的临时超调缩放，用于增强弹性。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (ClampMin = "0.0"))
	float OvershootScale = 1.2f;

	/** 最后一个字符出现后，完整文字继续保留的时间。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lifetime", meta = (ClampMin = "0.0"))
	float HoldTimeAfterReveal = 2.5f;

	/** 每次弹出音效的音量倍率。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (ClampMin = "0.0"))
	float SoundVolume = 1.0f;

	/** 每次弹出音效的音高倍率。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (ClampMin = "0.01"))
	float SoundPitch = 1.0f;

	/** 每帧将提示文字转向第一个本地玩家摄像机。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orientation")
	bool bFacePlayerCamera = false;
};

