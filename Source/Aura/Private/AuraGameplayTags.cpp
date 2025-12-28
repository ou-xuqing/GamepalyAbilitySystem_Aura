// Copyright Ziger


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::GameplayTags;
void FAuraGameplayTags::InitializeNativeGameplayTags()//调用UGameplayTagsManager中的静态函数来声明
{
	//Primary Tags
	GameplayTags.Attribute_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"),
		FString("Increase physical damage"));
	GameplayTags.Attribute_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"),
		FString("Increase magical damage"));
	GameplayTags.Attribute_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Resilience"),
		FString("Increase armor and armor penetration"));
	GameplayTags.Attribute_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Vigor"),
		FString("Increase Health"));
	
	//Secondary Tags
	GameplayTags.Attribute_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"),
		FString("Reduce damage taken, improve block chance"));
	GameplayTags.Attribute_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ArmorPenetration"),
		FString("Ignore percentage of enemy armor, increase critical hit chance"));
	GameplayTags.Attribute_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.BlockChance"),
		FString("Chance to cut incoming damage in half"));
	GameplayTags.Attribute_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitChance"),
		FString("Chance to double damage plus critical hit bonus"));
	GameplayTags.Attribute_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitDamage"),
		FString("Bonus damage added when a critical hit is scored"));
	GameplayTags.Attribute_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitResistance"),
		FString("Reduce critical hit chance of attacking enemy"));
	GameplayTags.Attribute_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.HealthRegeneration"),
		FString("Amount of health regenerated every 1 second"));
	GameplayTags.Attribute_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ManaRegeneration"),
		FString("Amount of mana regenerated every 1 second"));
	GameplayTags.Attribute_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxHealth"),
		FString("Maximum amount of health obtainable"));
	GameplayTags.Attribute_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxMana"),
		FString("Maximum amount of mana obtainable"));

	//Meta Tags
	GameplayTags.Attribute_Meta_InComingXP = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Meta.InComingXP"),
	FString("InComingXP Meta Attribute"));
	
	//Input Tags
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.LMB"),
		FString("InputTag of left mouse button"));
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.RMB"),
		FString("InputTag of right mouse button"));
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.1"),
		FString("InputTag of 1 key"));
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.2"),
		FString("InputTag of 2 key"));
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.3"),
		FString("InputTag of 3 key"));
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.4"),
		FString("InputTag of 4 key"));
	GameplayTags.InputTag_Passive_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("InputTag.Passive.1"),
	FString("InputTag of Passive1"));
	GameplayTags.InputTag_Passive_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("InputTag.Passive.2"),
	FString("InputTag of Passive2"));

	//Damage Tags
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage"),
	FString("Damage"));
	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage.Fire"),
	FString("Fire Damage Type"));
	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage.Lightning"),
	FString("Lightning Damage Type"));
	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage.Arcane"),
	FString("Arcane Damage Type"));
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage.Physical"),
	FString("Physical Damage Type"));

	//Damage Resilience
	GameplayTags.Attribute_Resilience_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Resilience.Fire"),
	FString("Fire Resilience"));
	GameplayTags.Attribute_Resilience_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Resilience.Lightning"),
	FString("Lightning Resilience"));
	GameplayTags.Attribute_Resilience_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Resilience.Arcane"),
	FString("Arcane Resilience"));
	GameplayTags.Attribute_Resilience_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Resilience.Physical"),
	FString("Physical Resilience"));

	GameplayTags.DamageTypesToResilience.Add(GameplayTags.Damage_Fire,GameplayTags.Attribute_Resilience_Fire);
	GameplayTags.DamageTypesToResilience.Add(GameplayTags.Damage_Lightning,GameplayTags.Attribute_Resilience_Lightning);
	GameplayTags.DamageTypesToResilience.Add(GameplayTags.Damage_Arcane,GameplayTags.Attribute_Resilience_Arcane);
	GameplayTags.DamageTypesToResilience.Add(GameplayTags.Damage_Physical,GameplayTags.Attribute_Resilience_Physical);

	//DeBuff
	GameplayTags.DeBuff_Burn = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("DeBuff.Fire"),
	FString("Fire DeBuff"));
	GameplayTags.DeBuff_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("DeBuff.Stun"),
	FString("Lightning DeBuff"));
	GameplayTags.DeBuff_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("DeBuff.Physical"),
	FString("Physical DeBuff"));
	GameplayTags.DeBuff_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("DeBuff.Arcane"),
	FString("Arcane DeBuff"));

	GameplayTags.DamageTypeToDeBuff.Add(GameplayTags.Damage_Fire,GameplayTags.DeBuff_Burn);
	GameplayTags.DamageTypeToDeBuff.Add(GameplayTags.Damage_Lightning,GameplayTags.DeBuff_Stun);
	GameplayTags.DamageTypeToDeBuff.Add(GameplayTags.Damage_Physical,GameplayTags.DeBuff_Physical);
	GameplayTags.DamageTypeToDeBuff.Add(GameplayTags.Damage_Arcane,GameplayTags.DeBuff_Arcane);

	GameplayTags.DeBuff_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("DeBuff.Damage"),
	FString("DeBuff Cause Damage"));
	GameplayTags.DeBuff_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("DeBuff.Duration"),
	FString("DeBuff Duration"));
	GameplayTags.DeBuff_Chance = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("DeBuff.Chance"),
	FString("DeBuff Chance"));
	GameplayTags.DeBuff_Frequency = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("DeBuff.Frequency"),
	FString("DeBuff Frequency"));
	
	//Effect Tags
	GameplayTags.Effect_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Effect.HitReact"),
	FString("Tag Granted When Hit Reacting"));

	//Ability Tags
	GameplayTags.Abilities_None = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.None"),
	FString("None Ability Tag"));
	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Attack"),
	FString("Attack Ability Tag"));
	GameplayTags.Abilities_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Summon"),
	FString("Summon Ability Tag"));
	GameplayTags.Abilities_Fire_FireBolt = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Fire.FireBolt"),
	FString("FireBolt Ability Tag"));
	GameplayTags.Abilities_Lightning_Electrocute = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Lightning.Electrocute"),
	FString("Electrocute Ability Tag"));
	GameplayTags.Abilities_Arcane_ArcaneShader = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Arcane.ArcaneShader"),
	FString("ArcaneShader Ability Tag"));
	GameplayTags.Abilities_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.HitReact"),
	FString("HitReact Ability Tag"));
	GameplayTags.Abilities_Knockback = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Knockback"),
	FString("Knockback Ability Tag"));
	
	//PassiveAbility
	GameplayTags.Abilities_Passive_LifeSiphon = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Passive.LifeSiphon"),
	FString("Passive LifeSiphon Ability Tag"));
	GameplayTags.Abilities_Passive_ManaSiphon = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Passive.ManaSiphon"),
	FString("Passive ManaSiphon Ability Tag"));
	GameplayTags.Abilities_Passive_HaloOfProtection = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Passive.HaloOfProtection"),
	FString("Passive HaloOfProtection Ability Tag"));
	
	//AbilityStatus
	GameplayTags.Abilities_Status_Locked = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Status.Locked"),
	FString("Locked Ability Status"));
	GameplayTags.Abilities_Status_Eligible = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Status.Eligible"),
	FString("Eligible Ability Status"));
	GameplayTags.Abilities_Status_UnLocked = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Status.UnLocked"),
	FString("UnLocked Ability Status"));
	GameplayTags.Abilities_Status_Equipped = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Status.Equipped"),
	FString("Equipped Ability Status"));

	//AbilityType
	GameplayTags.Abilities_Type_Offensive = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Type.Offensive"),
	FString("Offensive Ability Type"));
	GameplayTags.Abilities_Type_Passive = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Type.Passive"),
	FString("Passive Ability Type"));
	GameplayTags.Abilities_Type_None = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Type.None"),
	FString("None Ability Type"));
	
	//CoolDown Tags
	GameplayTags.CoolDown_Fire_FireBolt = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("CoolDown.Fire.FireBolt"),
	FString("FireBolt CoolDown Tag"));
	
	//CombatSocket Tags
	GameplayTags.CombatSocket_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("CombatSocket.Weapon"),
	FString("Weapon"));
	GameplayTags.CombatSocket_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("CombatSocket.LeftHand"),
	FString("Left Hand"));
	GameplayTags.CombatSocket_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("CombatSocket.RightHand"),
	FString("Right Hand"));
	GameplayTags.CombatSocket_Tail = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("CombatSocket.Tail"),
	FString("Tail"));

	
	//Montage Tags
	GameplayTags.Montage_Attack_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Montage.Attack.1"),
	FString("Attack 1"));
	GameplayTags.Montage_Attack_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Montage.Attack.2"),
	FString("Attack 2"));
	GameplayTags.Montage_Attack_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Montage.Attack.3"),
	FString("Attack 3"));
	GameplayTags.Montage_Attack_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Montage.Attack.4"),
	FString("Attack 4"));


	//PlayerBlock
	GameplayTags.Player_Block_CursorTrace = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Player.Block.CursorTrace"),
	FString("Block CursorTrace"));
	GameplayTags.Player_Block_InputHeld = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Player.Block.InputHeld"),
	FString("Block InputHeld"));
	GameplayTags.Player_Block_InputPress = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Player.Block.InputPress"),
	FString("Block InputPress"));
	GameplayTags.Player_Block_InputRelease = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Player.Block.InputRelease"),
	FString("Block InputRelease"));
}
