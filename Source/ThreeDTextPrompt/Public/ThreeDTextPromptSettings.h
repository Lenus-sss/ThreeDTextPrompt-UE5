// 版权所有 Epic Games, Inc. 保留所有权利。

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ThreeDTextPromptTypes.h"
#include "ThreeDTextPromptSettings.generated.h"

class AThreeDTextPromptActor;

/** C++ 生成 3D 文字提示时使用的项目级默认设置。 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "3D Text Prompt"))
class THREEDTEXTPROMPT_API UThreeDTextPromptSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** 使用原生 Actor 类和可编辑默认参数创建设置对象。 */
	UThreeDTextPromptSettings();

	/** 返回该设置对象在项目设置中的分类。 */
	virtual FName GetCategoryName() const override;

	/** 函数库未指定自定义类时生成的提示 Actor 类。 */
	UPROPERTY(Config, EditAnywhere, Category = "Spawn")
	TSubclassOf<AThreeDTextPromptActor> PromptActorClass;

	/** 每次生成提示前用于合并的默认参数，调用方参数会覆盖它。 */
	UPROPERTY(Config, EditAnywhere, Category = "Defaults")
	FThreeDTextPromptOptions DefaultOptions;
};

