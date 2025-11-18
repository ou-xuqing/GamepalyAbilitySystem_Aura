// Copyright Ziger


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Public/AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()//在蓝图中调用，因为开始游戏时AttributeMenu不会创建到游戏中
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);
	AAuraPlayerState* AuraPS = Cast<AAuraPlayerState>(PlayerState);
	for (auto& pair: AS->TagsToAttribute)
	{
		BroadcastAttributeInfo(pair.Key,pair.Value());
	}
	OnAttributePointsChangedDelegate.Broadcast(AuraPS->GetAttributePoints());
	OnSpellPointsChangedDelegate.Broadcast(AuraPS->GetSpellPoints());
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	//AuraWidgetController中可是有四个参数，所以有关GAS的属性几乎都能获得
	for (auto &pair: AS->TagsToAttribute)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(pair.Value()).AddLambda(
		[this,pair](const FOnAttributeChangeData &Data)
		{
			BroadcastAttributeInfo(pair.Key,pair.Value());
		}
		);
	}
	if (AAuraPlayerState* AuraPS = Cast<AAuraPlayerState>(PlayerState))
	{
		AuraPS->OnAttributePointsChangedDelegate.AddLambda([this](int32 NewPoints)
		{
			OnAttributePointsChangedDelegate.Broadcast(NewPoints);
		});

		AuraPS->OnSpellPointsChangedDelegate.AddLambda([this](int32 NewPoints)
		{
			OnSpellPointsChangedDelegate.Broadcast(NewPoints);
		});
	}
}

void UAttributeMenuWidgetController::UpdateAttribute(const FGameplayTag& AttributeTag)
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	AuraASC->UpdateAttribute(AttributeTag);
}

//通过tag来找到Info中相应的信息，然后通过AS获得属性的值，然后广播给相应的部件（通过tag来分辨，tag在UE中分配）
void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& Tag, const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoByTag(Tag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
