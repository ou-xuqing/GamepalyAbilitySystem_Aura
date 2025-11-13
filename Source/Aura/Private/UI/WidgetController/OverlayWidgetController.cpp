// Copyright Ziger


#include "UI/WidgetController/OverlayWidgetController.h"

#include "GameplayTagsManager.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/Public/AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Public/AbilitySystem/AuraAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet);
	
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()//为ASC委托绑定函数，其中函数又调用了overlayWidget委托，而overlayWidget委托在蓝图中绑定函数
  {
  	const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet);
  	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
  		AuraAttributeSet->GetHealthAttribute()
  		).AddLambda([this](const FOnAttributeChangeData& Data)
		  {
			  OnHealthChanged.Broadcast(Data.NewValue);
		  });//绑定更新委托，更新委托调用的是蓝图委托方法，在蓝图progressBar中绑定

	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetMaxHealthAttribute()
		).AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
  		AuraAttributeSet->GetManaAttribute()
  		).AddLambda([this](const FOnAttributeChangeData& Data)
		  {
			  OnManaChanged.Broadcast(Data.NewValue);
		  });
  
  	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
  		AuraAttributeSet->GetMaxManaAttribute()
  		).AddLambda([this](const FOnAttributeChangeData& Data)
  		{
  			OnMaxManaChanged.Broadcast(Data.NewValue);
  		});

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		if (AuraASC->bStartupAbilitiesGiven)//当AuraASC已经设置好能力，但是OWC还未绑定该委托时，直接调用函数
		{
			OnInitializeStartupAbilities(AuraASC);
		}else
		{
			AuraASC->AbilitiesGivenDelegate.AddUObject(this,&UOverlayWidgetController::OnInitializeStartupAbilities);
		}
		
		AuraASC->EffectAssetTags.AddLambda(//为EffectAssetTags绑定函数
			[this](const FGameplayTagContainer& TagContainer)
			{
				for (const FGameplayTag& Tag : TagContainer)
				{
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));//GameplayTag类型匹配，树的祖宗节点匹配
					if (Tag.MatchesTag(MessageTag))
					{
						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable,Tag);
						MessageWidgetDelegate.Broadcast(*Row);//该委托在蓝图overlayWidget中绑定
					}
				}
			}
		);
	}

  }

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraASC)
{
	if (!AuraASC->bStartupAbilitiesGiven) return;//如果开始能力还未设置，返回

	FForeachAbility ForeachAbilityDelegate;
	ForeachAbilityDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FAuraAbilityInfo Info= AbilityInfo->GetAbilityInfo(UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec),true);
		Info.InputTag = UAuraAbilitySystemComponent::GetInputTagFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);//OverlapWidget接受广播的值
	});
	AuraASC->ForEachAbility(ForeachAbilityDelegate);//在ASC中执行该委托，为了不在WidgetController中过多使用其他类的内容。（例如，将当前激活能力列表锁定）
}
