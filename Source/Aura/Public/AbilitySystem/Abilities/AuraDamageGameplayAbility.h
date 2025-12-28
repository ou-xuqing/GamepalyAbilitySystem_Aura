// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* Target);

	//默认在击飞方向增加斜向上45.f
	UFUNCTION(BlueprintCallable)
	virtual FDamageAbilityEffectParams MakeDamageAbilityEffectParams(AActor* Target = nullptr,
		FVector InRadialDamageOrigin = FVector::ZeroVector,
		bool bOverrideKnockbackDirection = false,FVector OverrideKnockbackDirection = FVector::ZeroVector,
		bool bOverrideDeathImpulse = false,FVector OverrideDeathImpulse = FVector::ZeroVector,
		bool bOverridePitch = false,float OverridePitch = 45.f) const;

protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Damage")
	FGameplayTag DamageType;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Damage")
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly,Category="Damage")
	float DeBuffChance = 20.f;
	
	UPROPERTY(EditDefaultsOnly,Category="Damage")
	float DeBuffDamage = 5.f;

	UPROPERTY(EditDefaultsOnly,Category="Damage")
	float DeBuffDuration = 5.f;

	UPROPERTY(EditDefaultsOnly,Category="Damage")
	float DeBuffFrequency = 1.f;

	UPROPERTY(EditDefaultsOnly,Category="Damage")
	float DeathImpulse = 500.f;

	UPROPERTY(EditDefaultsOnly,Category="Damage")
	float KnockbackMagnitude = 500.f;
	
	UPROPERTY(EditDefaultsOnly,Category="Damage")
	float KnockbackChance = 20.f;

	UPROPERTY(EditDefaultsOnly,Category="Damage")
	bool bIsRadialDamage = false;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Damage")
	float RadialDamageInnerRadius = 0.f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Damage")
	float RadialDamageOuterRadius = 0.f;
	
	
	float GetDamage(int32 InLevel);
};
