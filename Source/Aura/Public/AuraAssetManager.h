// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AuraAssetManager.generated.h"

/**
 * 修改配置文件，让UGameplayTagsManager中Get方法返回此类的实例。为了初始化NativeTag
 */
UCLASS()
class AURA_API UAuraAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static UAuraAssetManager& Get();
	virtual void StartInitialLoading() override;
};
