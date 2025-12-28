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

FDamageAbilityEffectParams UAuraDamageGameplayAbility::MakeDamageAbilityEffectParams(AActor* Target,FVector InRadialDamageOrigin,
	bool bOverrideKnockbackDirection,FVector OverrideKnockbackDirection,
	bool bOverrideDeathImpulse,FVector OverrideDeathImpulse,
	bool bOverridePitch,float OverridePitch) const 
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
	if (IsValid(Target))
	{
		FRotator Rotation = (Target->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		FVector ToTarget = Rotation.Vector();
		Params.DeathImpulse = ToTarget * Params.DeathImpulseMagnitude;
		Rotation.Pitch = OverridePitch;
		Params.Knockback = ToTarget * Params.KnockbackMagnitude;
	}

	if (bOverrideKnockbackDirection)
	{
		OverrideKnockbackDirection.Normalize();
		Params.Knockback = OverrideKnockbackDirection * Params.KnockbackMagnitude;
		if (bOverridePitch)
		{
			FRotator KnockbackRotation= OverrideKnockbackDirection.Rotation();
			KnockbackRotation.Pitch = OverridePitch;
			Params.Knockback = KnockbackRotation.Vector() * Params.KnockbackMagnitude;
		}
	}

	if (bOverrideDeathImpulse)
	{
		OverrideDeathImpulse.Normalize();
		Params.DeathImpulse = OverrideDeathImpulse * Params.DeathImpulse;
		if (bOverridePitch)
		{
			FRotator DeathImpulseRotation = OverrideDeathImpulse.Rotation();
			DeathImpulseRotation.Pitch = OverridePitch;
			Params.DeathImpulse = DeathImpulseRotation.Vector() * Params.DeathImpulse;
		}
	}

	
	if (bIsRadialDamage == true)
	{
		Params.bIsRadialDamage = bIsRadialDamage;
		Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
		Params.RadialDamageOuterRadius = RadialDamageOuterRadius;
		Params.RadialDamageOrigin = InRadialDamageOrigin;
	}
	return Params;
}

float UAuraDamageGameplayAbility::GetDamage(int32 InLevel)
{
	const float CauseDamage = Damage.GetValueAtLevel(InLevel);
	return CauseDamage;
}
