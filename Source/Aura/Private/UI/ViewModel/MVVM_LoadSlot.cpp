// Copyright Ziger


#include "UI/ViewModel/MVVM_LoadSlot.h"

#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"

void UMVVM_LoadSlot::InitializeSlot()
{
	//TODO::根据存档来初始化slot
	int32 WidgetSwitcherIndex = LoadSlotState.GetIntValue();
	SetWidgetSwitcherIndex.Broadcast(WidgetSwitcherIndex);
}

void UMVVM_LoadSlot::NewGameButtonPressed()
{
	SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadSlot::NewSlotButtonPressed(FString Name)
{
	AAuraGameModeBase* GameMode =Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	SetMapName(GameMode->DefaultMapName);
	SetPlayerName(Name);
	SetPlayerLevel(1);
	LoadSlotState = Taken;
	PlayerStartTag = GameMode->DefaultPlayerStartTag;
	MapAssertName = GameMode->DefaultMap.ToSoftObjectPath().GetAssetName();
	InitializeSlot();
	
	GameMode->SaveLoadScreen(this);

	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GameMode->GetGameInstance());
	GameInstance->LoadSlotIndex = LoadSlotIndex;
	GameInstance->LoadSlotName = GetLoadSlotName();
	GameInstance->PlayerStartTag = GameMode->DefaultPlayerStartTag;
}


void UMVVM_LoadSlot::SetLoadSlotName(const FString& InName)
{
	UE_MVVM_SET_PROPERTY_VALUE(LoadSlotName,InName);
}

void UMVVM_LoadSlot::SetPlayerName(const FString& InName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName,InName);
}

void UMVVM_LoadSlot::SetMapName(const FString& InMapName)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName,InMapName);
}

void UMVVM_LoadSlot::SetPlayerLevel(int32 InLevel)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerLevel,InLevel);
}
