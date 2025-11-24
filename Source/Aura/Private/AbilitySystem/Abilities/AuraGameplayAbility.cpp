// Copyright Ziger


#include "Aura/Public/AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystem/AuraAttributeSet.h"

FString UAuraGameplayAbility::GetDescription(int Level)
{
	return FString::Printf(TEXT("<Default>%sm</><Level>%d</>"),L"AbilityName",Level);
}

FString UAuraGameplayAbility::GetDescriptionNextLevel(int Level)
{
	return FString::Printf(TEXT("<Level>%d</> \n <Damage>Cause Much More Damage</>"),Level);
}

FString UAuraGameplayAbility::GetDescriptionOnLock(int Level)
{
	return FString::Printf(TEXT("<Level>Spell Locked Until Level %d </>"),Level);
}

float UAuraGameplayAbility::GetManaCost(int InLevel) const
{
	float ManaCost = 0;
	if (UGameplayEffect* CostEffect= GetCostGameplayEffect())
	{
		for (FGameplayModifierInfo Mod : CostEffect->Modifiers)
		{
			if (Mod.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				//可以返回ScalableFloat类型的数据,有无等级不影响
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel,ManaCost); 
				break;
			}
		}
	}
	return ManaCost;
}

float UAuraGameplayAbility::GetCooldown(int InLevel) const
{
	float CooldownTime = 0.f;
	if (UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel,CooldownTime);
	}
	return CooldownTime;
}
