// Copyright Ziger


#include "AbilitySystem/Data/AbilityInfo.h"

#include "Aura/AuraLogChannels.h"

FAuraAbilityInfo UAbilityInfo::GetAbilityInfo(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const FAuraAbilityInfo& AbilityInfo : AbilityInformation)
	{
		if (AbilityInfo.AbilityTag == AbilityTag)
		{
			return AbilityInfo;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogAura,Error,TEXT("在%s中，无法通过AbilityTag%s找到相应数据"),*GetNameSafe(this),*AbilityTag.ToString());
	}
	
	return FAuraAbilityInfo();
}
