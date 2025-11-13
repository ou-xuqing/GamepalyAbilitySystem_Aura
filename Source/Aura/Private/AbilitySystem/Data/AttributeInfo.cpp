// Copyright Ziger


#include "Aura/Public/AbilitySystem/Data/AttributeInfo.h"

#include "Aura/AuraLogChannels.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoByTag(const FGameplayTag& Tag, bool bLogIsNotFound) const
{
	for (auto const & Info : AttributeInformation)
	{
		if (Info.AttributeTag.MatchesTagExact(Tag))
		{
			return Info;
		}
	}
	if (bLogIsNotFound)
	{
		UE_LOG(LogAura,Error,TEXT("Can't find Info for this AttributeTag [%s] on AttributeInfo [%s]"),*Tag.ToString(),*GetNameSafe(this));
	}
	return FAuraAttributeInfo();
}
