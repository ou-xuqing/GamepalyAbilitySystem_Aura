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
};
