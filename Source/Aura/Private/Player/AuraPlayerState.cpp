// Copyright Ziger


#include "Player/AuraPlayerState.h"

#include "Aura/Public/AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Public/AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	SetNetUpdateFrequency(100.f);
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraPlayerState,Level);
	DOREPLIFETIME(AAuraPlayerState,XPMember);
	DOREPLIFETIME(AAuraPlayerState,AttributePointsMember);
	DOREPLIFETIME(AAuraPlayerState,SpellPointsMember);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::AddToLevel(int32 InLevel)
{
	Level += InLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void AAuraPlayerState::SetLevel(int32 InLevel)
{
	Level = InLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void AAuraPlayerState::AddToXP(int32 InXP)
{
	XPMember += InXP;
	OnXPChangedDelegate.Broadcast(XPMember);
}

void AAuraPlayerState::SetXP(int32 InXP)
{
	XPMember = InXP;
	OnXPChangedDelegate.Broadcast(XPMember);
}

void AAuraPlayerState::AddToAttributePoints(int32 InPoints)
{
	AttributePointsMember += InPoints;
	OnAttributePointsChangedDelegate.Broadcast(AttributePointsMember);
}

void AAuraPlayerState::SetAttributePoints(int32 InPoints)
{
	AttributePointsMember = InPoints;
	OnAttributePointsChangedDelegate.Broadcast(AttributePointsMember);
}

void AAuraPlayerState::AddToSpellPoints(int32 InPoints)
{
	SpellPointsMember += InPoints;
	OnSpellPointsChangedDelegate.Broadcast(SpellPointsMember);
}

void AAuraPlayerState::SetSpellPoints(int32 InPoints)
{
	SpellPointsMember = InPoints;
	OnSpellPointsChangedDelegate.Broadcast(SpellPointsMember);
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChangedDelegate.Broadcast(Level);
}

void AAuraPlayerState::OnRep_XP(int32 OldXP)//XP复制到客户端时进行广播
{
	OnXPChangedDelegate.Broadcast(XPMember);
}

void AAuraPlayerState::OnRep_AttributePoints(int32 OldXP)
{
	OnAttributePointsChangedDelegate.Broadcast(AttributePointsMember);
}

void AAuraPlayerState::OnRep_SpellPoints(int32 OldSpellPoints)
{
	OnSpellPointsChangedDelegate.Broadcast(SpellPointsMember);
}
