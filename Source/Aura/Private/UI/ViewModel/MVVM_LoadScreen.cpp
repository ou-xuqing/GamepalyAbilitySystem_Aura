// Copyright Ziger


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UMVVM_LoadScreen::InitialLoadSlotViewModel()
{
	LoadSlotViewModel_0 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlotViewModel_0->SetLoadSlotName(FString("LoadSlot_0"));
	LoadSlotViewModel_0->LoadSlotIndex = 0;
	LoadSlotViewModels.Add(LoadSlotViewModel_0);
	LoadSlotViewModel_1 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlotViewModel_1->SetLoadSlotName(FString("LoadSlot_1"));
	LoadSlotViewModel_1->LoadSlotIndex = 1;
	LoadSlotViewModels.Add(LoadSlotViewModel_1);
	LoadSlotViewModel_2 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlotViewModel_2->SetLoadSlotName(FString("LoadSlot_2"));
	LoadSlotViewModel_2->LoadSlotIndex = 2;
	LoadSlotViewModels.Add(LoadSlotViewModel_2);
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModel(int32 Index) const
{
	return LoadSlotViewModels[Index];
}

void UMVVM_LoadScreen::LoadData()
{
	AAuraGameModeBase* GameMode =Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	for (UMVVM_LoadSlot* LoadSlot : LoadSlotViewModels)
	{
		if (ULoadScreenSaveGame* SaveGame = GameMode->LoadSlotData(LoadSlot->GetLoadSlotName(), LoadSlot->LoadSlotIndex))
		{
			LoadSlot->SetPlayerName(SaveGame->PlayerName);
			LoadSlot->LoadSlotState = SaveGame->LoadSlotState;
			LoadSlot->SetMapName(SaveGame->MapName);
			LoadSlot->PlayerStartTag = SaveGame->PlayerStartTag;
		}else
		{
			LoadSlot->LoadSlotState = Vacant;
		}
		LoadSlot->InitializeSlot();
	}
}

void UMVVM_LoadScreen::SelectButtonPressed(int32 Index)
{
	SelectedSlotDelegate.Broadcast();
	SelectedSlotIndex = Index;
	for (const UMVVM_LoadSlot* LoadSlot:LoadSlotViewModels)
	{
		if (LoadSlot->LoadSlotIndex == Index)
		{
			LoadSlot->SelectButtonPressedDelegate.Broadcast(false);
		}else
		{
			LoadSlot->SelectButtonPressedDelegate.Broadcast(true);
		}
	}
}

void UMVVM_LoadScreen::DeleteSlot()
{
	if (SelectedSlotIndex == -1) return;
	AAuraGameModeBase* GameMode =Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	GameMode->DeleteSlotData(LoadSlotViewModels[SelectedSlotIndex]);
	LoadSlotViewModels[SelectedSlotIndex]->LoadSlotState = Vacant;
	LoadSlotViewModels[SelectedSlotIndex]->InitializeSlot();
	LoadSlotViewModels[SelectedSlotIndex]->DeleteSlotDelegate.Broadcast();
	SelectedSlotIndex = -1;
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	if (SelectedSlotIndex == -1) return;
	AAuraGameModeBase* GameMode =Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GameMode->GetGameInstance());
	
	GameInstance->PlayerStartTag = LoadSlotViewModels[SelectedSlotIndex]->PlayerStartTag;
	GameInstance->LoadSlotName = LoadSlotViewModels[SelectedSlotIndex]->GetLoadSlotName();
	GameInstance->LoadSlotIndex = LoadSlotViewModels[SelectedSlotIndex]->LoadSlotIndex;
	GameMode->TravelMap(LoadSlotViewModels[SelectedSlotIndex]);
}

void UMVVM_LoadScreen::SetLoadScreenName(FString InName)
{
	UE_MVVM_SET_PROPERTY_VALUE(LoadScreenName,InName);
}
