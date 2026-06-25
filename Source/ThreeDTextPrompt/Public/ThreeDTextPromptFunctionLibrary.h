// 版权所有 Epic Games, Inc. 保留所有权利。

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ThreeDTextPromptTypes.h"
#include "ThreeDTextPromptFunctionLibrary.generated.h"

class AThreeDTextPromptActor;

/** 用于生成 3D 文字提示的 C++ 静态入口。 */
UCLASS()
class THREEDTEXTPROMPT_API UThreeDTextPromptFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** 使用项目默认值和调用方参数合并后的结果生成提示。 */
	UFUNCTION(BlueprintCallable, Category = "3D Text Prompt", meta = (WorldContext = "WorldContextObject"))
	static AThreeDTextPromptActor* Show3DTextPrompt(UObject* WorldContextObject, const FText& Text, const FVector Location, const FRotator Rotation, const FThreeDTextPromptOptions& Options);

	/** 仅使用项目默认值生成提示。 */
	UFUNCTION(BlueprintCallable, Category = "3D Text Prompt", meta = (WorldContext = "WorldContextObject"))
	static AThreeDTextPromptActor* Show3DTextPromptWithDefaults(UObject* WorldContextObject, const FText& Text, const FVector Location, const FRotator Rotation);

private:
	/** 将默认资源合并到调用方参数中，不覆盖调用方显式指定的值。 */
	static FThreeDTextPromptOptions MergeOptionsWithDefaults(const FThreeDTextPromptOptions& Options);
};

