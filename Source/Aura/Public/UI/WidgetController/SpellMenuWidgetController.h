// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellGlobeSelectSignatrue,bool,bSpendPointButtonEnable,bool,bEquippedButtonEnablel,FString,OutDescription,FString,OutDescriptionNextLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquippedSignature,const FGameplayTag&,AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquippedSkillSignature,const FGameplayTag&,AbilityTag);

struct FSelectedAbility
{
	FGameplayTag AbilityTag = FGameplayTag();
	
	FGameplayTag AbilityStatus = FGameplayTag();
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;
	
	virtual void BindCallbacksToDependencies() override;
	
	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);

	void SetSpendPointsAndEquipped(const FGameplayTag& StatusTag,int32 SpellPoints,bool& bSpendPoints,bool& bEquipped);

	UFUNCTION(BlueprintCallable)
	void SpendPointsPressed();
	
	UFUNCTION(BlueprintCallable)
	void GlobeDeSelect();

	UFUNCTION(BlueprintCallable)
	void EquippedPressed();

	UFUNCTION(BlueprintCallable)
	void EquippedSpellGlobePressed(const FGameplayTag& Slot,const FGameplayTag& AbilityType);

	void OnEquipSpellGlobePressed(const FGameplayTag& AbilityTag,const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreSlot);
	
	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectSignatrue OnSpellGlobeSelectDelegate;

	UPROPERTY(BlueprintAssignable)
	FWaitForEquippedSignature WaitForEquippedDelegate;

	UPROPERTY(BlueprintAssignable)
	FWaitForEquippedSignature StopWaitForEquippedDelegate;
	
	UPROPERTY(BlueprintAssignable,category="GAS|XP")
	FPlayerStateSignatrue OnSpellPointsChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FEquippedSkillSignature OnEquippedSkillDelegate;

private:
	//存放当前选择的Ability信息
	FSelectedAbility SelectedAbility{FAuraGameplayTags::Get().Abilities_None,FAuraGameplayTags::Get().Abilities_Status_Locked};
	int32 CurrentSpellPoints = 0;
	bool bWaitForEquipped = false;
	FGameplayTag SelectedSlot;
};
