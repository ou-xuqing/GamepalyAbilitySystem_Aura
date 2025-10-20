// Copyright Ziger


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "Aura/Public/AbilitySystem/AuraAttributeSet.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);

	for (auto& pair: AS->TagsToAttribute)
	{
		BroadcastAttributeInfo(pair.Key,pair.Value());
	}
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	//AuraWidgetController中可是有四个参数，所以有关GAS的属性几乎都能获得
	for (auto &pair: AS->TagsToAttribute)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(pair.Value()).AddLambda(
		[this,pair,AS](const FOnAttributeChangeData &Data)
		{
			BroadcastAttributeInfo(pair.Key,pair.Value());
		}
		);
	}
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& Tag, const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoByTag(Tag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
