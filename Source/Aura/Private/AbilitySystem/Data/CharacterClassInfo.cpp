// Copyright Ziger


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetDefaultCharacterClassInfo(ECharacterClass CharacterClass)
{
	return DefaultCharacterClassInformation.FindChecked(CharacterClass);
}
