// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraHitReact.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraHitReact : public UAuraGameplayAbility
{
	GENERATED_BODY()
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable)
	FActiveGameplayEffectHandle GrantedTag();
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="HitReacting")
	TSubclassOf<UGameplayEffect> HitReactEffectClass;
};
