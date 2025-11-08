// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();

	UPROPERTY(EditDefaultsOnly,Category="Summoning")
	int NumMinions = 5;

	UPROPERTY(EditDefaultsOnly,Category="Summoning")
	TArray<TSubclassOf<APawn>> MinionClass;
	
	UPROPERTY(EditDefaultsOnly,Category="Summoning")
	float MinSpawnDistance = 50.f;

	UPROPERTY(EditDefaultsOnly,Category="Summoning")
	float MaxSpawnDistance = 200.f;

	UPROPERTY(EditDefaultsOnly,Category="Summoning")
	float SpawnSpread = 90.f;

	UFUNCTION(BlueprintPure,Category="Summoning")
	TSubclassOf<APawn> GetRandomMinionClass();
};
