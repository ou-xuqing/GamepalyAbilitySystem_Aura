// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraInputComponent.generated.h"

class UAuraInputConfig;
/**
 * 只是绑定InputAction到具体方法
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	template<class UserClass, typename PressFuncType, typename ReleaseFuncType, typename HeldFuncType>
	void BindAbilityAction(UAuraInputConfig* InputConfig, UserClass *Object, PressFuncType PressFunc, ReleaseFuncType ReleaseFunc, HeldFuncType HeldFunc);//InputAction在AuraInputConfig当中
};

template <class UserClass, typename PressFuncType, typename ReleaseFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityAction(UAuraInputConfig* InputConfig, UserClass* Object, PressFuncType PressFunc,
	ReleaseFuncType ReleaseFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);
	for (FAuraInputAction &Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Started,Object, PressFunc, Action.InputTag);
			}
			if (ReleaseFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Completed,Object, ReleaseFunc, Action.InputTag);
			}
			if (HeldFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Triggered,Object, HeldFunc, Action.InputTag);
			}
		}
	}

}
