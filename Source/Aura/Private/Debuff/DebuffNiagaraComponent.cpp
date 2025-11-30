// Copyright Ziger


#include "Debuff/DebuffNiagaraComponent.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Interaction/CombatInterface.h"

UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
	bAutoActivate = false;

}

void UDebuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
	if (UAbilitySystemComponent* OwnerAsc = Cast<UAbilitySystemComponent>(GetOwner()))
	{
		OwnerAsc->RegisterGameplayTagEvent(DebuffTag,EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&UDebuffNiagaraComponent::DebuffTagChanged
			);
	}else if (CombatInterface)
	{
		//WeakLambda不会增加引用计数器
		CombatInterface->GetOnASCRegisterDelegate().AddWeakLambda(this,[this](UAbilitySystemComponent* NewASC)
		{
			NewASC->RegisterGameplayTagEvent(DebuffTag,EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&UDebuffNiagaraComponent::DebuffTagChanged
			);
		});
	}
	if (CombatInterface)
	{
		//C++中注册动态多播时，函数要用UFUNCTION宏，并且要用AddDynamic
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UDebuffNiagaraComponent::OnActorDeath);	
	}
	
}

void UDebuffNiagaraComponent::DebuffTagChanged(FGameplayTag CallbackTag,int32 NewCount)
{
	bool bOwner = IsValid(GetOwner());
	bool bIsLive = GetOwner()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(GetOwner());
	if (NewCount > 0 && bOwner && bIsLive)//活着才应用Debuff
	{
		Activate();
	}else
	{
		Deactivate();
	}
}

void UDebuffNiagaraComponent::OnActorDeath(AActor* DeathActor)
{
	Deactivate();
}

