// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LootTiers.generated.h"

USTRUCT(BlueprintType)
struct FLootItem
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="LootTiers")
	TSubclassOf<AActor> ItemClass;

	UPROPERTY(EditDefaultsOnly,Category="LootTiers")
	float SpawnChance = 10.f;

	UPROPERTY(EditDefaultsOnly,Category="LootTiers")
	float MaxSpawnNums = 1.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="LootTiers")
	bool bLevelOverride = true;
};

/**
 * 
 */
UCLASS()
class AURA_API ULootTiers : public UDataAsset
{
	GENERATED_BODY()
public:
	//获取掉落物
	UFUNCTION(BlueprintCallable)
	TArray<FLootItem> GetLootItems();
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="LootTiers")
	TArray<FLootItem> LootItems;
};
