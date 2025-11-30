// Copyright Ziger


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* Target)
{
	FDamageAbilityEffectParams Params = MakeDamageAbilityEffectParams(Target);
	FRotator Rotation = (Target->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
	Rotation.Pitch = 45.f;
	const FVector ToTarget = Rotation.Vector();
	Params.Knockback = ToTarget * Params.KnockbackMagnitude;
	UAuraAbilitySystemLibrary::ApplyDamageEffect(Params);
}

FDamageAbilityEffectParams UAuraDamageGameplayAbility::MakeDamageAbilityEffectParams(AActor* Target) const 
{
	FDamageAbilityEffectParams Params;
	Params.SourceAsc = GetAbilitySystemComponentFromActorInfo();
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	Params.Damage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	Params.DamageEffectClass = DamageEffectClass;
	Params.DeBuffDamage = DeBuffDamage;
	Params.DeBuffChance = DeBuffChance;
	Params.DeBuffDuration = DeBuffDuration;
	Params.DeBuffFrequency = DeBuffFrequency;
	Params.DeathImpulseMagnitude = DeathImpulse;
	Params.KnockbackMagnitude = KnockbackMagnitude;
	Params.KnockbackChance = KnockbackChance;
	return Params;
}

float UAuraDamageGameplayAbility::GetDamage(int32 InLevel)
{
	const float CauseDamage = Damage.GetValueAtLevel(InLevel);
	return CauseDamage;
}
