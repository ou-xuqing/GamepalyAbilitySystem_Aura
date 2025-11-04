// Copyright Ziger


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* Target)
{
	//GA内置函数，他可以找到自己的ASC
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass,1.f);

	for (TTuple<FGameplayTag, FScalableFloat> Pair : DamageTypes)
	{
		const float CurDamage =  Pair.Value.GetValueAtLevel(GetAbilityLevel());
		//将伤害与Tag进行绑定，这是GA要做的，最后在Execal中取出对应的伤害进行计算。
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle,Pair.Key, CurDamage);
	}
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(),UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target));
}
