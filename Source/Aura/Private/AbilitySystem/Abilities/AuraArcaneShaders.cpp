// Copyright Ziger


#include "AbilitySystem/Abilities/AuraArcaneShaders.h"

#include "Components/DecalComponent.h"

FString UAuraArcaneShaders::GetDescription(int Level)
{
	const float ManaCost = GetManaCost(Level);
	const float CooldownTime = GetCooldown(Level);
	const int32 CurDamage = GetDamage(Level);
	
	return FString::Printf(TEXT("<Tile>ArcaneShaders</> \n"
		"<Small>Level: </><Level>%d</> \n"
		"<Small>ManaCost: </><ManaCost>%.1f</> \n"
		"<Small>Cooldown: </><Cooldown>%.1f</> \n"
		"Cause </><Damage> %d </> <Default>Damage</> \n"
		),
		Level,
		ManaCost,
		CooldownTime,
		CurDamage
		);
}

FString UAuraArcaneShaders::GetDescriptionNextLevel(int Level)
{
	const float ManaCost = GetManaCost(Level);
	const float CooldownTime = GetCooldown(Level);
	const int32 CurDamage = GetDamage(Level);
	
	return FString::Printf(TEXT("<Tile>Next Level</> \n"
		"<Small>Level: </><Level>%d</> \n"
		"<Small>ManaCost: </><ManaCost>%.1f</> \n"
		"<Small>Cooldown: </><Cooldown>%.1f</> \n"
		"Cause </><Damage> %d </> <Default>Damage</> \n"
		),
		Level,
		ManaCost,
		CooldownTime,
		CurDamage
		);
}

void UAuraArcaneShaders::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	if (!IsValid(MagicCircle))
	{
		MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);
		if (DecalMaterial)
		{
			MagicCircle->GetDecalComponent()->SetMaterial(0,DecalMaterial);
		}
	}
}

void UAuraArcaneShaders::HideMagicCircle()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->Destroy();
	}
}


