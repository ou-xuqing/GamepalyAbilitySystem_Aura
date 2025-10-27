// Copyright Ziger


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::GameplayTags;
void FAuraGameplayTags::InitializeNativeGameplayTags()
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

	//Damage Tags
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage"),
	FString("Damage"));

	//Effect Tags
	GameplayTags.Effect_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Effect.HitReact"),
	FString("Tag Granted When Hit Reacting"));
}
