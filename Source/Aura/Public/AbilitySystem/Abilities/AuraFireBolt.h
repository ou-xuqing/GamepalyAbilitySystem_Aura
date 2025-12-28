// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "AuraFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()
public:
	//在ASC中被调用，ASC可以通过Tag找到Spec，通过Spec找到Ability
	virtual FString GetDescription(int Level) override;
	virtual FString GetDescriptionNextLevel(int Level) override;
	
	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles(const FVector& ProjectileTargetLocation,const FGameplayTag& SocketTag,bool bIsOverridePitch = false,float OverridePitch = 0.f,AActor* Target = nullptr);

	UPROPERTY(EditDefaultsOnly,Category="Spell")
	float SpawnSpread = 90.f;
	
	UPROPERTY(EditDefaultsOnly,Category="Spell")
	float HomingMagnitudeMin = 1600.f;
	
	UPROPERTY(EditDefaultsOnly,Category="Spell")
	float HomingMagnitudeMax = 3200.f;

};
