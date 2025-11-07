// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

class UAuraGameplayAbility;
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer&)

/**
 *ASC和AttributeSet定义在character和playerstate当中，其中玩家操控的character也包含了playerstate
 *通过ASC调用InitAbilityActorInfo来设置InOwnerActor和InAvatarActor
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void AbilityActorInfoSet();
	//多播委托，广播Tag
	FEffectAssetTags EffectAssetTags;

	void AddCharacterAbilities(TArray<TSubclassOf<UGameplayAbility>> &StartupAbilities);//ASC is where to add Ability
	
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
protected:
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle GameplayEffectHandle);
};
