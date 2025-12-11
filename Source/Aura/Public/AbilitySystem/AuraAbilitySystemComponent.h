// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

class UAuraAbilitySystemComponent;
class UAuraGameplayAbility;
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer&)
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven)
DECLARE_DELEGATE_OneParam(FForeachAbility,const FGameplayAbilitySpec&)
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged,const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/, int32/*NewLevel*/)
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped,const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/,const FGameplayTag& /*Slot(InputTag)*/, const FGameplayTag& /*PreSlot*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FDeActivePassiveAbility,const FGameplayTag& /*AbilityTag*/)
/**
 *ASC和AttributeSet定义在character和playerstate当中，其中玩家操控的character也包含了playerstate
 *通过ASC调用InitAbilityActorInfo来设置InOwnerActor和InAvatarActor
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void AbilityActorInfoSet();
	//多播委托，广播Tag
	FEffectAssetTags EffectAssetTags;
	//告诉OverlayWidgetController，Ability已经设置完毕
	FAbilitiesGiven AbilitiesGivenDelegate;
	FAbilityStatusChanged AbilityStatusChangedDelegate;
	FAbilityEquipped AbilityEquippedDelegate;
	FDeActivePassiveAbility DeActivePassiveAbilityDelegate;
	bool bStartupAbilitiesGiven = false;
	
	void AddCharacterAbilities(TArray<TSubclassOf<UGameplayAbility>> &StartupAbilities);//ASC is where to add Ability
	void AddCharacterPassiveAbilities(TArray<TSubclassOf<UGameplayAbility>> &StartupPassiveAbilities);

	void AbilityInputTagPress(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	//遍历已经添加的能力（能力在add之后变成activable能力），执行委托绑定的函数。该委托是直接调用进行执行，不是收到了其他地方的广播
	void ForEachAbility(const FForeachAbility& Delegate);

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	FGameplayAbilitySpec* GetSpecFromTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag);
	bool GetDescriptionFromAbilityTag(const FGameplayTag& AbilityTag,FString& OutDescription,FString& OutDescriptionNextLevel);
	
	void UpdateAttribute(const FGameplayTag& AttributeTag);

	void UpdateAbilityStatus(int32 Level);

	UFUNCTION(Server,Reliable)
	void ServerSpendSpellPoints(const FGameplayTag& AbilityTag);

	UFUNCTION(Server,Reliable)
	void ServerEquipAbility(const FGameplayTag& Slot,const FGameplayTag& AbilityTag);

	UFUNCTION(Client,Reliable)
	void ClientEquipAbility(const FGameplayTag& AbilityTag,const FGameplayTag& Status,const FGameplayTag& Slot,const FGameplayTag& PreSlot);

	void ClearSlot(FGameplayAbilitySpec* Spec);
	void ClearAbilityOfSlot(const FGameplayTag& Slot);
	bool AbilityHasSlot(const FGameplayTag& Slot,FGameplayAbilitySpec* Spec);
protected:

	UFUNCTION(Server,Reliable)
	void ServerUpdateAttribute(const FGameplayTag& AttributeTag);
	
	virtual void OnRep_ActivateAbilities() override;
	
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle GameplayEffectHandle);
	UFUNCTION(Client, Reliable)
	void ClientAbilityStatusChanged(const FGameplayTag& AbilityTag , const FGameplayTag& StatusTag,int32 NewLevel);
};
