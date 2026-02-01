// Copyright Ziger


#include "PassiveNiagara/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
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
		ActivateIfEquipped(AuraASC);
	}
	else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnASCRegisterDelegate().AddLambda([this](UAbilitySystemComponent* ASC)
		{
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(ASC))
			{
				AuraASC->ActivatePassiveNiagaraDelegate.AddUObject(this,&UPassiveNiagaraComponent::OnPassiveActivated);
				ActivateIfEquipped(AuraASC);
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

void UPassiveNiagaraComponent::ActivateIfEquipped(UAuraAbilitySystemComponent* AuraASC)
{
	if (AuraASC->bStartupAbilitiesGiven)
	{
		if (AuraASC->GetStatusFromAbilityTag(PassiveSpellTag).MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
		{
			Activate();
		}
	}
}
