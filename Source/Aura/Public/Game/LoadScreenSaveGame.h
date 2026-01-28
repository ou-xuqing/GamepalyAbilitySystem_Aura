// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "LoadScreenSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	/*
	 * 存档标记
	 */
	UPROPERTY()
	FString SlotName = FString();

	UPROPERTY()
	int32 SlotIndex = 0;

	/*
	 * 存储内容
	 */
	UPROPERTY()
	FString PlayerName = FString();

	UPROPERTY()
	TEnumAsByte<ELoadSlotStates> LoadSlotState = Vacant;

	UPROPERTY()
	FString MapName = FString();

	UPROPERTY()
	FName PlayerStartTag;
};
