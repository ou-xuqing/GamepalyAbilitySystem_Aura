// Copyright Ziger


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionByTag(const FGameplayTag& Tag, bool bLogIsNotFound)
{
	for (auto &InputAction : AbilityInputActions)
	{
		if (InputAction.InputTag.MatchesTagExact(Tag))return InputAction.InputAction;
	}
	if (bLogIsNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find InputAction by Tag: [%s], on InputConfig: [%s]"),*Tag.ToString(),*GetNameSafe(this));
	}
	return nullptr;
}
