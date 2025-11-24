// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility//Base Class of Ability
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FGameplayTag StartupInputTag;
	
	virtual FString GetDescription(int Level);
	virtual FString GetDescriptionNextLevel(int Level);
	static FString GetDescriptionOnLock(int Level);

	float GetManaCost(int InLevel = 1) const;

	float GetCooldown(int InLevel = 1) const;
};
