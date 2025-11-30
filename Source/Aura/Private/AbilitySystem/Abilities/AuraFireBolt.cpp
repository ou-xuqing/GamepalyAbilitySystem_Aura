// Copyright Ziger


#include "AbilitySystem/Abilities/AuraFireBolt.h"

//在ASC中被调用
FString UAuraFireBolt::GetDescription(int Level)
{
	//三个Get在父类中实现
	const float ManaCost = GetManaCost(Level);
	const float CooldownTime = GetCooldown(Level);
	const int32 CurDamage = GetDamage(Level);
	
	return FString::Printf(TEXT("<Tile>Fire Bolt</> \n"
		"<Small>Level: </><Level>%d</> \n"
		"<Small>ManaCost: </><ManaCost>%.1f</> \n"
		"<Small>Cooldown: </><Cooldown>%.1f</> \n"
		"<Default>Launch %d FireBolt,"
		"Cause </><Damage> %d </> <Default>Damage</> \n"
		),
		Level,
		ManaCost,
		CooldownTime,
		FMath::Min(Level,NumProjectileSpell),
		CurDamage
		);
}

FString UAuraFireBolt::GetDescriptionNextLevel(int Level)
{
	const float ManaCost = GetManaCost(Level);
	const float CooldownTime = GetCooldown(Level);
	const int32 CurDamage = GetDamage(Level);
	
	return FString::Printf(TEXT("<Tile>Next Level</> \n"
		"<Small>Level: </><Level>%d</> \n"
		"<Small>ManaCost: </><ManaCost>%.1f</> \n"
		"<Small>Cooldown: </><Cooldown>%.1f</> \n"
		"<Default>Launch %d FireBolt,"
		"Cause </><Damage> %d </> <Default>Damage</> \n"
		),
		Level,
		ManaCost,
		CooldownTime,
		FMath::Min(Level,NumProjectileSpell),
		CurDamage
		);
}