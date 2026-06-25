// 版权所有 Epic Games, Inc. 保留所有权利。

#include "ThreeDTextPromptSettings.h"

#include "Engine/Font.h"
#include "ThreeDTextPromptActor.h"

/** 使用原生提示 Actor 类初始化设置对象。 */
UThreeDTextPromptSettings::UThreeDTextPromptSettings()
{
	PromptActorClass = AThreeDTextPromptActor::StaticClass();
	DefaultOptions.Font = LoadObject<UFont>(nullptr, TEXT("/ThreeDTextPrompt/Fonts/Unibit_Font.Unibit_Font"));
	if (!DefaultOptions.Font)
	{
		DefaultOptions.Font = LoadObject<UFont>(nullptr, TEXT("/ThreeDTextPrompt/Fonts/Unibit.Unibit"));
	}
}

/** 将该设置面板放到项目设置的 Game 分类下。 */
FName UThreeDTextPromptSettings::GetCategoryName() const
{
	return TEXT("Game");
}
