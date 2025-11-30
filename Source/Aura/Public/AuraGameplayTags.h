// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * 自定义Tag，使用单例设计模式
 */

struct FAuraGameplayTags
{
public:
	static const FAuraGameplayTags& Get(){return GameplayTags;}
	static void InitializeNativeGameplayTags();
	
	FGameplayTag Attribute_Primary_Strength;
	FGameplayTag Attribute_Primary_Intelligence;
	FGameplayTag Attribute_Primary_Resilience;
	FGameplayTag Attribute_Primary_Vigor;
	
	FGameplayTag Attribute_Secondary_Armor;
	FGameplayTag Attribute_Secondary_ArmorPenetration;
	FGameplayTag Attribute_Secondary_BlockChance;
	FGameplayTag Attribute_Secondary_CriticalHitChance;
	FGameplayTag Attribute_Secondary_CriticalHitDamage;
	FGameplayTag Attribute_Secondary_CriticalHitResistance;
	FGameplayTag Attribute_Secondary_HealthRegeneration;
	FGameplayTag Attribute_Secondary_ManaRegeneration;
	FGameplayTag Attribute_Secondary_MaxHealth;
	FGameplayTag Attribute_Secondary_MaxMana;

	FGameplayTag Attribute_Resilience_Fire;
	FGameplayTag Attribute_Resilience_Lightning;
	FGameplayTag Attribute_Resilience_Arcane;
	FGameplayTag Attribute_Resilience_Physical;

	FGameplayTag Attribute_Meta_InComingXP;
	
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;
	FGameplayTag InputTag_Passive_1;
	FGameplayTag InputTag_Passive_2;

	
	FGameplayTag Damage;
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Physical;

	FGameplayTag DeBuff_Burn;
	FGameplayTag DeBuff_Stun;
	FGameplayTag DeBuff_Physical;
	FGameplayTag DeBuff_Arcane;
	
	FGameplayTag DeBuff_Damage;
	FGameplayTag DeBuff_Chance;
	FGameplayTag DeBuff_Duration;
	FGameplayTag DeBuff_Frequency;
	
	FGameplayTag Effect_HitReact;

	FGameplayTag Abilities_None;
	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_Summon;

	FGameplayTag Abilities_HitReact;
	FGameplayTag Abilities_Knockback;
	
	FGameplayTag Abilities_Status_Locked;
	FGameplayTag Abilities_Status_UnLocked;
	FGameplayTag Abilities_Status_Eligible;
	FGameplayTag Abilities_Status_Equipped;

	FGameplayTag Abilities_Type_Offensive;
	FGameplayTag Abilities_Type_Passive;
	FGameplayTag Abilities_Type_None;
	
	FGameplayTag Abilities_Fire_FireBolt;
	FGameplayTag CoolDown_Fire_FireBolt;
	FGameplayTag Abilities_Lightning_Electrocute;
	
	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_Tail;
	
	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;
	
	TMap<FGameplayTag,FGameplayTag> DamageTypesToResilience;

	TMap<FGameplayTag,FGameplayTag> DamageTypeToDeBuff;
private:
	static FAuraGameplayTags GameplayTags;
};
