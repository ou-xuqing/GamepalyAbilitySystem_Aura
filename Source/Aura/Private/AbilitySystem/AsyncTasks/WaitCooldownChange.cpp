// Copyright Ziger


#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"
#include "AbilitySystemComponent.h"

UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,
                                                                const FGameplayTag& InCooldownTag)
{
	UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
	WaitCooldownChange->Asc = AbilitySystemComponent;
	WaitCooldownChange->CooldownTag = InCooldownTag;

	if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
	{
		WaitCooldownChange->EndTask();
		return nullptr;
	}
	//绑定Tag委托事件，当Tag发生变化时调用绑定函数。因为在静态函数中，this没有意义，所以传入对象（WaitCooldownChange）
	AbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTag,EGameplayTagEventType::NewOrRemoved).AddUObject(
		WaitCooldownChange,&UWaitCooldownChange::CooldownTagChanged);
	//当CooldownEffect应用时调用，此委托会在客户端和服务器都执行，而且只针对于周期的Effect
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange,&UWaitCooldownChange::OnActiveEffectAdded);
	return WaitCooldownChange;
}

void UWaitCooldownChange::EndTask()
{
	if (!IsValid(Asc)) return;
	Asc->RegisterGameplayTagEvent(CooldownTag,EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	SetReadyToDestroy();//Task完全做完时调用
	MarkAsGarbage();
}

//Tag和Int是RegisterGameplayTagEvent委托所声明的参数
void UWaitCooldownChange::CooldownTagChanged(FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount == 0)//当Tag被移除时End进行广播。
	{
		CooldownEnd.Broadcast(0.f);
	}
}

void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetAsc,const FGameplayEffectSpec& AppliedSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer AssertTags;
	AppliedSpec.GetAllAssetTags(AssertTags);
	FGameplayTagContainer GrantedTags;
	AppliedSpec.GetAllGrantedTags(GrantedTags);

	if (AssertTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))//检查该Effect是否有CooldownTag
	{
		//查找有CooldownTag的Effect
		FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		TArray<float> TimesRemaining  = Asc->GetActiveEffectsTimeRemaining(Query);//获取所有有cdTag的Effect的剩余时间，本项目其实只能找到一个，但是为了健壮性，需要对数组进行处理。
		float TimeRemaining = TimesRemaining[0];
		for (int i = 0;i<TimesRemaining.Num();i++)//获取冷却时间最长的那个
		{
			if (TimesRemaining[i] > TimeRemaining)
			{
				TimeRemaining = TimesRemaining[i];
			}
		}
		CooldownStart.Broadcast(TimeRemaining);
	}
}
