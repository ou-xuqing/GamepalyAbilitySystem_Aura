// Copyright Ziger


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Aura/Public/AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Public/AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()//为ASC委托绑定函数，其中函数又调用了overlayWidget委托，而overlayWidget委托在蓝图中绑定函数
  {
  	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
  		GetAuraAS()->GetHealthAttribute()
  		).AddLambda([this](const FOnAttributeChangeData& Data)
		  {
			  OnHealthChanged.Broadcast(Data.NewValue);
		  });//绑定更新委托，更新委托调用的是蓝图委托方法，在蓝图progressBar中绑定

	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetMaxHealthAttribute()
		).AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
  		GetAuraAS()->GetManaAttribute()
  		).AddLambda([this](const FOnAttributeChangeData& Data)
		  {
			  OnManaChanged.Broadcast(Data.NewValue);
		  });
  
  	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
  		GetAuraAS()->GetMaxManaAttribute()
  		).AddLambda([this](const FOnAttributeChangeData& Data)
  		{
  			OnMaxManaChanged.Broadcast(Data.NewValue);
  		});

	GetAuraASC()->AbilityEquippedDelegate.AddUObject(this,&UOverlayWidgetController::OnEquippedAbilityChanged);
	
	if (UAuraAbilitySystemComponent* AuraASC = GetAuraASC())
	{
		if (AuraASC->bStartupAbilitiesGiven)//当AuraASC已经设置好能力，但是OWC还未绑定该委托时，直接调用函数
		{
			BroadcastAbilityInfo();
		}else
		{
			AuraASC->AbilitiesGivenDelegate.AddUObject(this,&UOverlayWidgetController::BroadcastAbilityInfo);
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
	if (AAuraPlayerState* AuraPS = GetAuraPS())
	{
		AuraPS->OnXPChangedDelegate.AddUObject(this,&UOverlayWidgetController::OnXPChanged);
		AuraPS->OnLevelChangedDelegate.AddLambda([this](int32 NewLevel)
		{
			OnLevelUPDelegate.Broadcast(NewLevel);
		});
	}

  }

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* InAuraASC)
{
	if (!GetAuraASC()->bStartupAbilitiesGiven) return;//如果开始能力还未设置，返回

	FForeachAbility ForeachAbilityDelegate;
	ForeachAbilityDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FAuraAbilityInfo Info= AbilityInfo->FindInfoFromAbilityTag(UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec),true);
		Info.InputTag = UAuraAbilitySystemComponent::GetInputTagFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);//OverlapWidget接受广播的值
	});
	GetAuraASC()->ForEachAbility(ForeachAbilityDelegate);//在ASC中执行该委托，为了不在WidgetController中过多使用其他类的内容。（例如，将当前激活能力列表锁定）
}

void UOverlayWidgetController::OnXPChanged(int32 XP)
{
	if (AAuraPlayerState* AuraPS = GetAuraPS())
	{
		ULevelUpInfo* LevelUpInfo = AuraPS->LevelUpInfo;
			
		const int32 Level = LevelUpInfo->FindLevelForXP(XP);//通过经验值获取等级
		const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();

		if (Level<=MaxLevel && Level>0)
		{
			const int32 CurMaxXP = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
			const int32 CurMinXP = LevelUpInfo->LevelUpInformation[Level-1].LevelUpRequirement;
			const int32 DeltaXP = CurMaxXP - CurMinXP;
			const int32 XPForThisLevel = XP - CurMinXP;
			
			const float Precent =static_cast<float> (XPForThisLevel) / static_cast<float> (DeltaXP);
			OnXPBarChangedDelegate.Broadcast(Precent);
		}
	}
}

void UOverlayWidgetController::OnEquippedAbilityChanged(const FGameplayTag& AbilityTag, const FGameplayTag& Status,const FGameplayTag& Slot, const FGameplayTag& PreSlot)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	FAuraAbilityInfo PreInfo;
	PreInfo.AbilityStatus = GameplayTags.Abilities_Status_UnLocked;
	PreInfo.AbilityTag = GameplayTags.Abilities_None;
	PreInfo.InputTag = PreSlot;
	AbilityInfoDelegate.Broadcast(PreInfo);

	FAuraAbilityInfo Info;
	Info = AbilityInfo->FindInfoFromAbilityTag(AbilityTag);
	Info.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(Info);
}
