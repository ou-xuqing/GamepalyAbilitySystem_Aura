// Copyright Ziger


#include "PassiveNiagara/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		AuraASC->ActivatePassiveNiagaraDelegate.AddUObject(this,&UPassiveNiagaraComponent::OnPassiveActivated);
	}else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnASCRegisterDelegate().AddLambda([this](UAbilitySystemComponent* ASC)
		{
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(ASC))
			{
				AuraASC->ActivatePassiveNiagaraDelegate.AddUObject(this,&UPassiveNiagaraComponent::OnPassiveActivated);
			}
			
		});
	}
}

void UPassiveNiagaraComponent::OnPassiveActivated(const FGameplayTag& PassiveTag, bool IsActive)
{
	if (PassiveTag.MatchesTagExact(PassiveSpellTag))
	{
		if (IsActive)
		{
			Activate();
		}else
		{
			Deactivate();
		}
	}
}
