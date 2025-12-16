// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "PassiveNiagaraComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UPassiveNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
public:
	UPassiveNiagaraComponent();

	UPROPERTY(EditAnywhere)
	FGameplayTag PassiveSpellTag;
protected:
	virtual void BeginPlay() override;

	void OnPassiveActivated(const FGameplayTag& PassiveTag,bool IsActive);
};
