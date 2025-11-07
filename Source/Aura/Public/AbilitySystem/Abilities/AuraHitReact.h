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

	//GA蓝图调用这个GE是为了给目标授予Tag，通过这个Tag将目标的移动速度变为0（受到攻击禁止移动）
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="HitReacting")
	TSubclassOf<UGameplayEffect> HitReactEffectClass;
};
