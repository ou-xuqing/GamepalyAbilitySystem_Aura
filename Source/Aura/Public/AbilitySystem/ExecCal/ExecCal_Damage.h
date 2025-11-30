// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCal_Damage.generated.h"




/**
 * 
 */
UCLASS()
class AURA_API UExecCal_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	public:
	UExecCal_Damage();
	void IsDeBuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	              const FGameplayEffectSpec& EffectSpec,
	              FAggregatorEvaluateParameters EvaluateParameters) const;

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
};
