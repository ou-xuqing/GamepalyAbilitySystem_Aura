// Copyright Ziger


#include "Aura/Public/AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()//因为该处GameplayTag与AuraCharacter有关，所以在AuraCharacter的InitAbilityActorInfo中调用
{
	//GE应用时要复制到客户端（该委托只会在服务器中被调用）
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UAuraAbilitySystemComponent::ClientEffectApplied);//固定写法，将EffectApplied方法绑定到OnGameplayEffectAppliedDelegateToSelf委托
	//FGameplayTag Secondary_Armor =  FAuraGameplayTags::Get().Attribute_Secondary_Armor;
	//GEngine->AddOnScreenDebugMessage(-1,4.f,FColor::Blue,FString::Printf(TEXT("Tag: %s"),*Secondary_Armor.ToString()));
}

void UAuraAbilitySystemComponent::LoadAbilitiesFromDisk(ULoadScreenSaveGame* SaveData)
{
	for (FSavedAbility& Ability : SaveData->SavedAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability.AbilityClass,Ability.AbilityLevel);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(Ability.AbilitySlot);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(Ability.AbilityStatus);
		GiveAbility(AbilitySpec);
		if (Ability.AbilityType.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Type_Passive) && Ability.AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
		{
			TryActivateAbility(AbilitySpec.Handle);
		}
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)//只在服务器中调用
{
	for (const auto Ability : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability,1);
		if (const UAuraGameplayAbility *AuraGameplayAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraGameplayAbility->StartupInputTag);//让能力和蓝图GA中的tag进行绑定
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true; //只进行一次广播。注意：服务器中的bool和客户端的bool是独立的（?）
	AbilitiesGivenDelegate.Broadcast();//广播给SpellWidget
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const auto Ability : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability,1);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
		GiveAbilityAndActivateOnce(AbilitySpec);//被动技能的类是GameplayAbility，所以不需要cast到AuraGameplayAbility
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagPress(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())return;
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec &AbilitySpec : GetActivatableAbilities())//ActivatableAbilities是玩家所装配的技能，也就是ASC调用GiveAbility给予的。存放AbilitySpec
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				//激活WaitInputPress这个AbilityTask的Press出口
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed,AbilitySpec.Handle,AbilitySpec.GetAbilityInstances().Last()->GetCurrentActivationInfoRef().GetActivationPredictionKey());
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)//通过输入的tag来确定是哪个能力
{
	if (!InputTag.IsValid())return;
	FScopedAbilityListLock AbilityListLock(*this);
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
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec &AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			AbilitySpecInputReleased(AbilitySpec);
			//激活WaitInputReleased这个AbilityTask的Released出口
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased,AbilitySpec.Handle,AbilitySpec.GetAbilityInstances().Last()->GetCurrentActivationInfoRef().GetActivationPredictionKey());
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
		for (const FGameplayTag& AbilityTag : AbilitySpec.Ability.Get()->GetAssetTags())
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
		if (DyTag.MatchesTag(FGameplayTag::RequestGameplayTag("InputTag")))//InputTag可以在技能设计时填入也可以在游玩时自行修改
		{
			return DyTag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& DyTag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (DyTag.MatchesTag(FGameplayTag::RequestGameplayTag("Abilities.Status")))
		{
			return DyTag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())//只要能力被give，就会放在可激活能力里面
	{
		for (FGameplayTag Tag : Spec.Ability->GetAssetTags())
		{
			if (Tag.MatchesTagExact(AbilityTag))
			{
				return GetStatusTagFromSpec(Spec);
			}
		}
	}
	return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())//只要能力被give，就会放在可激活能力里面
	{
		for (FGameplayTag Tag : Spec.Ability->GetAssetTags())
		{
			if (Tag.MatchesTagExact(AbilityTag))
			{
				return &Spec;
			}
		}
	}
	return nullptr;
}

FGameplayTag UAuraAbilitySystemComponent::GetSlotFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromTag(AbilityTag))
	{
		return GetInputTagFromSpec(*AbilitySpec);
	}
	return FGameplayTag();
}

bool UAuraAbilitySystemComponent::GetDescriptionFromAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription,FString& OutDescriptionNextLevel)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromTag(AbilityTag))
	{
		if (!GetStatusTagFromSpec(*AbilitySpec).MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Locked))
		{
			UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability);
			OutDescription = AuraAbility->GetDescription(AbilitySpec->Level);
			OutDescriptionNextLevel = AuraAbility->GetDescriptionNextLevel(AbilitySpec->Level + 1);
			return true;
		}
	}
	if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_None))
	{
		OutDescription = FString();
	}else
	{
		const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
		OutDescription = UAuraGameplayAbility::GetDescriptionOnLock(AbilityInfo->FindInfoFromAbilityTag(AbilityTag).LevelRequirement);
	}
	OutDescriptionNextLevel = FString();
	return false;
}

bool UAuraAbilitySystemComponent::HasAbilityInSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(Slot))
		{
			return true;
		}
	}
	return false;
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(Slot))
		{
			return &Spec;
		}
	}
	return nullptr;
}

bool UAuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& AbilitySpec) const
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	const FAuraAbilityInfo& Info = AbilityInfo->FindInfoFromAbilityTag(GetAbilityTagFromSpec(AbilitySpec),true);
	if (Info.AbilityType.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Type_Passive))
	{
		return true;
	}
	return false;
}

bool UAuraAbilitySystemComponent::IsAbilityHasSlot(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.GetDynamicSpecSourceTags().HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
	{
		return true;
	}
	return false;
}

void UAuraAbilitySystemComponent::AssignSlotToAbility(const FGameplayTag& Slot, FGameplayAbilitySpec& AbilitySpec)
{
	ClearSlot(&AbilitySpec);
	AbilitySpec.GetDynamicSpecSourceTags().AddTag(Slot);
	const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();
	FGameplayTag Status = GetStatusTagFromSpec(AbilitySpec);
	if (Status == AuraGameplayTags.Abilities_Status_UnLocked)
	{
		AbilitySpec.GetDynamicSpecSourceTags().RemoveTag(AuraGameplayTags.Abilities_Status_UnLocked);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraGameplayTags.Abilities_Status_Equipped);
	}
}

void UAuraAbilitySystemComponent::UpdateAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())//检查Character是否实现playerInterface
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor())>0)
		{
			ServerUpdateAttribute(AttributeTag);//客户端点击时也在服务器执行
		}
	}
}

void UAuraAbilitySystemComponent::UpdateAbilityStatus(int32 Level)//在AuraCharacter的AddToLevel中调用，解锁新技能
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());

	for (FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if (!Info.AbilityTag.IsValid()) continue;//Tag未设置
		if (Level < Info.LevelRequirement) continue;//等级不够
		if (GetSpecFromTag(Info.AbilityTag) == nullptr)//发现了1没被添加的能力
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability,1);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);//等级达到后就从lock变成eligible
			GiveAbility(AbilitySpec);
			MarkAbilitySpecDirty(AbilitySpec);//将能力立刻复制到客户端
			ClientAbilityStatusChanged(Info.AbilityTag,FAuraGameplayTags::Get().Abilities_Status_Eligible,AbilitySpec.Level);//客户端和服务器一起广播StatusChanged委托
		}
	}
}
//Slot为当前点击的技能栏的InputTag,在客户端中有bug（主动技能没有这个bug），点击装备按钮，点击装备栏后取出的spec中Status是错误的，在服务器中执行没问题，升级技能时取出的Status也没问题。
void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& Slot,const FGameplayTag& AbilityTag)
{
	
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromTag(AbilityTag))
	{
		const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();
		const FGameplayTag& Status = GetStatusTagFromSpec(*AbilitySpec);
		const FGameplayTag& PreSlot = GetInputTagFromSpec(*AbilitySpec); //InputTag来标记Slot
		//如果技能是Unlocked状态，他可能没被安装过也可能被安装过，所以需要清除一下Slot Status == AuraGameplayTags.Abilities_Status_Equipped || Status == AuraGameplayTags.Abilities_Status_UnLocked
		if (Status == AuraGameplayTags.Abilities_Status_Equipped || Status == AuraGameplayTags.Abilities_Status_UnLocked)
		{
			if (HasAbilityInSlot(Slot))//查看被点击技能栏是否已经存有技能
			{
				FGameplayAbilitySpec* Spec = GetSpecFromSlot(Slot);
				if (Spec)
				{
					if (AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*Spec)))//存有的技能是否和当前要装备的技能一致
					{
						ClientEquipAbility(AbilityTag,AuraGameplayTags.Abilities_Status_Equipped,Slot,PreSlot);//更新是为了取消特效
						return;
					}
                    				
					if (IsPassiveAbility(*Spec))//如果存有的技能是被动技能，那么要结束这个技能
					{
						MulticastActivatePassiveEffect(GetAbilityTagFromSpec(*Spec),false);
						DeActivePassiveAbilityDelegate.Broadcast(GetAbilityTagFromSpec(*Spec));
					}
					ClearSlot(Spec);
				}
			}
			if (!IsAbilityHasSlot(*AbilitySpec))//如果要装备的技能之前没有装备到其他技能栏上
			{
				if (IsPassiveAbility(*AbilitySpec))//被动技能装备时激活
				{
					TryActivateAbility(AbilitySpec->Handle);
					MulticastActivatePassiveEffect(AbilityTag,true);
				}
				AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(GetStatusTagFromSpec(*AbilitySpec));
				AbilitySpec->GetDynamicSpecSourceTags().AddTag(AuraGameplayTags.Abilities_Status_Equipped);
			}
			AssignSlotToAbility(Slot,*AbilitySpec);//给新装备的技能分配技能栏
			MarkAbilitySpecDirty(*AbilitySpec);
		}
		
		ClientEquipAbility(AbilityTag,AuraGameplayTags.Abilities_Status_Equipped,Slot,PreSlot);
	}

	/*
	//清理要安装的Globe中已经安装的技能的Slot
	ClearAbilityOfSlot(Slot);
	//清理准备安装到Globe的技能的Slot
	ClearSlot(AbilitySpec);
	//将该技能与Globe的Slot绑定
	AbilitySpec->GetDynamicSpecSourceTags().AddTag(Slot);
	if (Status == AuraGameplayTags.Abilities_Status_UnLocked)
	{
		AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(AuraGameplayTags.Abilities_Status_UnLocked);
		AbilitySpec->GetDynamicSpecSourceTags().AddTag(AuraGameplayTags.Abilities_Status_Equipped);
	}*/
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag,const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreSlot)
{
	AbilityEquippedDelegate.Broadcast(AbilityTag,Status,Slot,PreSlot);
}

//如果有技能解锁，此时player已经解锁新技能。在UpdateCharacterStatus中增加新技能
void UAuraAbilitySystemComponent::ServerSpendSpellPoints_Implementation(const FGameplayTag& AbilityTag)
{
	if ( FGameplayAbilitySpec* AbilitySpec = GetSpecFromTag(AbilityTag))
	{
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(),-1);
		}
		
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		FGameplayTag AbilityStatus = GetStatusTagFromSpec(*AbilitySpec);
		if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
		{
			AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(GameplayTags.Abilities_Status_Eligible);
			AbilitySpec->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_UnLocked);
			AbilityStatus = GameplayTags.Abilities_Status_UnLocked;
		}else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_UnLocked))
		{
			AbilitySpec->Level += 1;
		}
		
		MarkAbilitySpecDirty(*AbilitySpec);
		ClientAbilityStatusChanged(AbilityTag,AbilityStatus,AbilitySpec->Level);
	}
}

void UAuraAbilitySystemComponent::ServerUpdateAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;
	//使用蓝图事件来修改属性（在蓝图中通过ASC ApplyEffect）。通过Tag来识别事件，在XP中也用过，使用一个持续激活的被动能力来监听事件
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(),AttributeTag,Payload);

	IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(),-1);
}



void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
	const FGameplayTag Slot = GetInputTagFromSpec(*Spec);
	Spec->GetDynamicSpecSourceTags().RemoveTag(Slot);
}

void UAuraAbilitySystemComponent::ClearAbilityOfSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(Slot,&AbilitySpec))
		{
			ClearSlot(&AbilitySpec);
		}	
	}
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(const FGameplayTag& Slot, FGameplayAbilitySpec* Spec)
{
	for (FGameplayTag Tag:Spec->GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTagExact(Slot))
		{
			return true;
		}
	}
	return false;
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)//本来是在AddCharacter中执行，但是由于AddCharacter只会在服务器中执行，所以客户端的UI无法接收广播。而能力会使用该函数复制到客户端，所以在此时广播。
	{
		bStartupAbilitiesGiven = true; //只进行一次广播
		AbilitiesGivenDelegate.Broadcast();//广播给SpellWidget
	}
	
}

void UAuraAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& PassiveSpellTag,bool IsActive)
{
	//因为委托的Broadcast只会在本地执行，而被动特效需要所有客户端都看到，所以采用多播来广播到每个客户端。
	ActivatePassiveNiagaraDelegate.Broadcast(PassiveSpellTag,IsActive);
}

//给特定客户端广播
void UAuraAbilitySystemComponent::ClientAbilityStatusChanged_Implementation(const FGameplayTag& AbilityTag , const FGameplayTag& StatusTag,int32 NewLevel)
{
	AbilityStatusChangedDelegate.Broadcast(AbilityTag,StatusTag,NewLevel);//SpellMenuController中绑定此委托
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                     const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle GameplayEffectHandle)//ASC传递GameplayTags的固定写法
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}
