// Copyright Ziger


#include "AbilitySystem/Abilities/AuraHitReact.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAuraHitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	//在计算伤害后激活能力
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

 FActiveGameplayEffectHandle UAuraHitReact::GrantedTag()//造了轮子,蓝图中ApplyGameEffectOwner 
{
	UAbilitySystemComponent* Asc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	FGameplayEffectContextHandle ContextHandle = Asc->MakeEffectContext();
	FGameplayEffectSpecHandle EffectSpec = Asc->MakeOutgoingSpec(HitReactEffectClass,1,ContextHandle);
	return Asc->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
}
