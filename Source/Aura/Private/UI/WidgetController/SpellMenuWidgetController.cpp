// Copyright Ziger


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	OnSpellPointsChangedDelegate.Broadcast(GetAuraPS()->GetSpellPoints());//开始游戏时先广播初始值
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	
	//AbilityStatusChangedDelegate在人物升级时会调用
	GetAuraASC()->AbilityStatusChangedDelegate.AddLambda([this](const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag,int32 NewLevel)
	{
		if (SelectedAbility.AbilityTag.MatchesTagExact(AbilityTag))//当选择该SpellGlobe时，Status发生改变，需要广播新的状态
		{
			SelectedAbility.AbilityStatus = StatusTag;
			FString OutDescription;
			FString OutDescriptionNextLevel;
			bool bEquipped = false; bool bSpendPoints = false;
			
			GetAuraASC()->GetDescriptionFromAbilityTag(AbilityTag,OutDescription,OutDescriptionNextLevel);
			SetSpendPointsAndEquipped(StatusTag,CurrentSpellPoints,bSpendPoints,bEquipped);
			OnSpellGlobeSelectDelegate.Broadcast(bSpendPoints,bEquipped,OutDescription,OutDescriptionNextLevel);
		}
		FAuraAbilityInfo Info = AbilityInfo->GetAbilityInfo(AbilityTag);
		Info.AbilityStatus = StatusTag;
		AbilityInfoDelegate.Broadcast(Info);
	});

	GetAuraASC()->AbilityEquippedDelegate.AddUObject(this,&USpellMenuWidgetController::OnEquipSpellGlobePressed);
	
	GetAuraPS()->OnPSSpellPointsChangedDelegate.AddLambda([this](int32 NewPoints)
	{
		OnSpellPointsChangedDelegate.Broadcast(NewPoints);//当PS中SpellPoints改变时就广播。
		FString OutDescription;
		FString OutDescriptionNextLevel;
		//当选择该SpellGlobe时，Points发生改变，需要广播新的状态
		CurrentSpellPoints = NewPoints;
		bool bEquipped = false; bool bSpendPoints = false;
		GetAuraASC()->GetDescriptionFromAbilityTag(SelectedAbility.AbilityTag,OutDescription,OutDescriptionNextLevel);
		SetSpendPointsAndEquipped(SelectedAbility.AbilityStatus,CurrentSpellPoints,bSpendPoints,bEquipped);
		OnSpellGlobeSelectDelegate.Broadcast(bSpendPoints,bEquipped,OutDescription,OutDescriptionNextLevel);
	});
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitForEquipped)//关闭EquippedSpell动画，注意此时是SelectedAbility是上次选择的Ability
	{
		FGameplayTag AbilityType = AbilityInfo->GetAbilityInfo(SelectedAbility.AbilityTag).AbilityType;
		StopWaitForEquippedDelegate.Broadcast(AbilityType);
		bWaitForEquipped = false;
	}
	
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	const int32 SpellPoints = GetAuraPS()->GetSpellPoints();
	FGameplayTag AbilityStatus;
	FString OutDescription;
	FString OutDescriptionNextLevel;
	
	const bool bTagValid = AbilityTag.IsValid();
	const bool bTagNone = AbilityTag.MatchesTagExact(GameplayTags.Abilities_None);
	FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromTag(AbilityTag);
	const bool bSpecValid = AbilitySpec != nullptr;
	if (!bTagValid || bTagNone || !bSpecValid)
	{
		AbilityStatus = GameplayTags.Abilities_Status_Locked;
	}else
	{
		AbilityStatus = GetAuraASC()->GetStatusTagFromSpec(*AbilitySpec);
	}
	
	SelectedAbility.AbilityStatus = AbilityStatus;
	SelectedAbility.AbilityTag = AbilityTag;
	bool bEquipped = false; bool bSpendPoints = false;

	//获得技能的描述
	GetAuraASC()->GetDescriptionFromAbilityTag(AbilityTag,OutDescription,OutDescriptionNextLevel);
	//计算按键是否可以使用
	SetSpendPointsAndEquipped(AbilityStatus,SpellPoints,bSpendPoints,bEquipped);
	//向蓝图发送广播
	OnSpellGlobeSelectDelegate.Broadcast(bSpendPoints,bEquipped,OutDescription,OutDescriptionNextLevel);
}

void USpellMenuWidgetController::SetSpendPointsAndEquipped(const FGameplayTag& StatusTag, int32 SpellPoints,bool& bSpendPoints, bool& bEquipped)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	if (StatusTag == GameplayTags.Abilities_Status_Equipped || StatusTag == GameplayTags.Abilities_Status_UnLocked)
	{
		bEquipped = true;
		bSpendPoints = true;
	}else if (StatusTag == GameplayTags.Abilities_Status_Eligible)
	{
		bSpendPoints = true;
		bEquipped = false;
	}else if (StatusTag == GameplayTags.Abilities_Status_Locked)
	{
		bSpendPoints = false;
		bEquipped = false;
	}
	if (SpellPoints == 0) bSpendPoints = false;
}

void USpellMenuWidgetController::SpendPointsPressed()
{
	GetAuraASC()->ServerSpendSpellPoints_Implementation(SelectedAbility.AbilityTag);
}

void USpellMenuWidgetController::GlobeDeSelect()
{
	if (bWaitForEquipped)//关闭EquippedSpell动画
	{
		FGameplayTag AbilityType = AbilityInfo->GetAbilityInfo(SelectedAbility.AbilityTag).AbilityType;
		StopWaitForEquippedDelegate.Broadcast(AbilityType);
		bWaitForEquipped = false;
	}
	SelectedAbility.AbilityTag = FAuraGameplayTags::Get().Abilities_None;
	SelectedAbility.AbilityStatus = FAuraGameplayTags::Get().Abilities_Status_Locked;
	OnSpellGlobeSelectDelegate.Broadcast(false,false,FString(),FString());
}

void USpellMenuWidgetController::EquippedPressed()//按下装备按键
{
	bWaitForEquipped = true;
	FGameplayTag AbilityType = AbilityInfo->GetAbilityInfo(SelectedAbility.AbilityTag).AbilityType;
	WaitForEquippedDelegate.Broadcast(AbilityType);

	//EquipSpellGlobe根据InputTag来确定AbilityInfo是否发给自身,所以要事先记录技能的InputTag用来后续清除已经装备的技能
	if (SelectedAbility.AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
	{
		SelectedSlot = GetAuraASC()->GetInputTagFromAbilityTag(SelectedAbility.AbilityTag);
	}
}

void USpellMenuWidgetController::EquippedSpellGlobePressed(const FGameplayTag& Slot, const FGameplayTag& AbilityType)
{
	//没有按下Equipped
	if (!bWaitForEquipped) return;
	//所选技能类型和装备栏类型不同
	if (!AbilityType.MatchesTagExact(AbilityInfo->GetAbilityInfo(SelectedAbility.AbilityTag).AbilityType)) return;
	//对技能操作需要在ASC中进行
	GetAuraASC()->ServerEquipAbility(Slot,SelectedAbility.AbilityTag);
}
//接收ASC中AbilityEquippedDelegate发出的广播
void USpellMenuWidgetController::OnEquipSpellGlobePressed(const FGameplayTag& AbilityTag, const FGameplayTag& Status,const FGameplayTag& Slot, const FGameplayTag& PreSlot)
{
	bWaitForEquipped = false;
	//EquipSpellGlobe根据InputTag来确定AbilityInfo是否发给自身，EquipSpellGlobe监听AbilityInfoDelegate的广播
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	FAuraAbilityInfo PreInfo;
	//SpellGlobe是通过AbilityTag来确定AbilityInfo是否发给自身，所以如果SpellGlobe的AbilityTag默认是None则会改变。
	PreInfo.AbilityTag = GameplayTags.Abilities_None;
	PreInfo.InputTag = PreSlot;
	PreInfo.AbilityStatus = GameplayTags.Abilities_Status_UnLocked;
	AbilityInfoDelegate.Broadcast(PreInfo);

	FAuraAbilityInfo Info = AbilityInfo->GetAbilityInfo(AbilityTag);
	Info.AbilityStatus = Status;
	Info.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(Info);

	StopWaitForEquippedDelegate.Broadcast(AbilityInfo->GetAbilityInfo(AbilityTag).AbilityType);//关闭EquipSpell动画
}


/*
//第二章 87节，没有按照视频方法写，可能是bug
void USpellMenuWidgetController::OnSpellGlobeClickedSetButton(const FAuraAbilityInfo& Info)
{
	
	bool bSpendPoints= false;
	bool bEquipped = false;
	if (Info.AbilityStatus == FAuraGameplayTags::Get().Abilities_Status_Equipped || Info.AbilityStatus == FAuraGameplayTags::Get().Abilities_Status_UnLocked)
	{
		bSpendPoints = true;
		bEquipped = true;
	}else if (Info.AbilityStatus == FAuraGameplayTags::Get().Abilities_Status_Eligible)
	{
		bSpendPoints = true;
		bEquipped = false;
	}else if (Info.AbilityStatus == FAuraGameplayTags::Get().Abilities_Status_Locked)
	{
		bSpendPoints = false;
		bEquipped = false;
	}else //还未填写技能信息
	{
		bSpendPoints = false;
		bEquipped = false;
	}
	if (GetAuraPS()->GetSpellPoints() <= 0 )
	{
		bSpendPoints = false;
	}

	OnSpellGlobeSelectDelegate.Broadcast(bSpendPoints,bEquipped);
}
*/
