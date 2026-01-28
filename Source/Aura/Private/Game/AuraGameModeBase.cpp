// Copyright Ziger


#include "Game/AuraGameModeBase.h"

#include "Game/AuraGameInstance.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
//通过name和id来找到对应的存档
void AAuraGameModeBase::SaveLoadScreen(UMVVM_LoadSlot* LoadSlot)
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->GetLoadSlotName(),LoadSlot->LoadSlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlot->GetLoadSlotName(),LoadSlot->LoadSlotIndex);
	}

	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadSlotSaveGameClass);
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->LoadSlotState = Taken;
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();
	LoadScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;
	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame,LoadSlot->GetLoadSlotName(),LoadSlot->LoadSlotIndex);
}

ULoadScreenSaveGame* AAuraGameModeBase::LoadSlotData(const FString& LoadSlotName, const int32 LoadSlotIndex)
{
	USaveGame* SaveGameObject = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(LoadSlotName,LoadSlotIndex))
	{
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(LoadSlotName,LoadSlotIndex);
	}
	if (SaveGameObject)
	{
		ULoadScreenSaveGame* SaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
		return SaveGame;
	}
	return nullptr;
}

void AAuraGameModeBase::DeleteSlotData(const UMVVM_LoadSlot* LoadSlot)
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->GetLoadSlotName(),LoadSlot->LoadSlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlot->GetLoadSlotName(),LoadSlot->LoadSlotIndex);	
	}
}

void AAuraGameModeBase::TravelMap(UMVVM_LoadSlot* LoadSlot)
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(LoadSlot,Maps.FindChecked(LoadSlot->GetMapName()));
}

ULoadScreenSaveGame* AAuraGameModeBase::GetCurGameSaveData()
{
	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		return LoadSlotData(GameInstance->LoadSlotName,GameInstance->LoadSlotIndex);
	}
	return nullptr;
}

void AAuraGameModeBase::SaveCurGameProgress(ULoadScreenSaveGame* SaveObject)
{
	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	GameInstance->PlayerStartTag = SaveObject->PlayerStartTag;
	
	UGameplayStatics::SaveGameToSlot(SaveObject,GameInstance->LoadSlotName,GameInstance->LoadSlotIndex);
}

//通过playerstartTag来选择场景中的playerstart然后生成角色.
AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* GameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),APlayerStart::StaticClass(),PlayerStarts);
	if (PlayerStarts.Num()>0)
	{
		AActor* CurChoosePlayerStart = PlayerStarts[0];
		for (AActor* Actor : PlayerStarts)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				if (PlayerStart->PlayerStartTag == GameInstance->PlayerStartTag)
				{
					CurChoosePlayerStart = PlayerStart;
					break;
				}
			}
		}
		return CurChoosePlayerStart;
	}
	return nullptr;
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	Maps.Add(DefaultMapName,DefaultMap);
}
