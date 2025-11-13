// Copyright Ziger


#include "Aura/Public/AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Aura/AuraLogChannels.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()//因为该处GameplayTag与AuraCharacter有关，所以在AuraCharacter的InitAbilityActorInfo中调用
{
	//GE应用时要复制到客户端（该委托只会在服务器中被调用）
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UAuraAbilitySystemComponent::ClientEffectApplied);//固定写法，将EffectApplied方法绑定到OnGameplayEffectAppliedDelegateToSelf委托
	//FGameplayTag Secondary_Armor =  FAuraGameplayTags::Get().Attribute_Secondary_Armor;
	//GEngine->AddOnScreenDebugMessage(-1,4.f,FColor::Blue,FString::Printf(TEXT("Tag: %s"),*Secondary_Armor.ToString()));
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)//只在服务器中调用
{
	for (const auto Ability : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability,1);
		if (const UAuraGameplayAbility *AuraGameplayAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraGameplayAbility->StartupInputTag);//让能力和蓝图GA中的tag进行绑定
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true; //只进行一次广播。注意：服务器中的bool和客户端的bool是独立的（?）
	AbilitiesGivenDelegate.Broadcast(this);//广播给SpellWidget
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)//通过输入的tag来确定是哪个能力
{
	if (!InputTag.IsValid())return;
	for (FGameplayAbilitySpec &AbilitySpec : GetActivatableAbilities())//ActivatableAbilities是玩家所装配的技能，也就是ASC调用GiveAbility给予的。存放AbilitySpec
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())return;
	for (FGameplayAbilitySpec &AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForeachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))//委托如果绑定了函数就执行
		{
			UE_LOG(LogAura,Error,TEXT("在%hs中没有执行委托"),__FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (const FGameplayTag& AbilityTag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (AbilityTag.MatchesTag(FGameplayTag::RequestGameplayTag("Abilities")))
			{
				return AbilityTag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& DyTag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (DyTag.MatchesTag(FGameplayTag::RequestGameplayTag("InputTag")))
		{
			return DyTag;
		}
	}
	return FGameplayTag();
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)//本来是在AddCharacter中执行，但是由于AddCharacter只会在服务器中执行，所以客户端的UI无法接收广播。而能力会使用该函数复制到客户端，所以在此时广播。
	{
		bStartupAbilitiesGiven = true; //只进行一次广播
		AbilitiesGivenDelegate.Broadcast(this);//广播给SpellWidget
	}
	
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                     const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle GameplayEffectHandle)//ASC传递GameplayTags的固定写法
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}
