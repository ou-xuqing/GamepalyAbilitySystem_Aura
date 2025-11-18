// Copyright Ziger


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 XP) const
{
	int32 Level = 1;
	bool bSearching = true;
	while (bSearching)
	{
		//LevelUpInformation[0]没有意义，该数组从1开始
		if (LevelUpInformation.Num()-1 <= Level)
		{
			return Level;
		}
		if (XP >= LevelUpInformation[Level].LevelUpRequirement)
		{
			Level++;
		}else
		{
			bSearching = false;
		}
	}
	return Level;
}
