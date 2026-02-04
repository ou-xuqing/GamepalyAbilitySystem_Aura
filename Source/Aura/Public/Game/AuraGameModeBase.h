// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class ULootTiers;
class USaveGame;
class ULoadScreenSaveGame;
class UMVVM_LoadSlot;
class UAbilityInfo;
class UCharacterClassInfo;
/**
 * 
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:

	void SaveLoadScreen(UMVVM_LoadSlot* LoadSlot);

	ULoadScreenSaveGame* LoadSlotData(const FString& LoadSlotName, const int32 LoadSlotIndex);

	void DeleteSlotData(const UMVVM_LoadSlot* LoadSlot);

	void TravelMap(UMVVM_LoadSlot* LoadSlot);

	ULoadScreenSaveGame* GetCurGameSaveData();
	void SaveCurGameProgress(ULoadScreenSaveGame* SaveObject);
	void SaveWorldState(UWorld* World, FString DestinationWorldAssetName = FString(""));
	void LoadWorldState(UWorld* World);
	
	UPROPERTY(EditDefaultsOnly,Category="Enemy CharacterClass Info")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly,Category="Aura Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	UPROPERTY(EditDefaultsOnly,Category="Enemy LootTires")
	TObjectPtr<ULootTiers> LootTiers;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> SaveGameClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAbilityInfo> AbilityInfoClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCharacterClassInfo> CharacterClassInfoClass;
	
	UPROPERTY(EditDefaultsOnly,Category="Load Screen")
	TSubclassOf<USaveGame> LoadSlotSaveGameClass;

	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<FString,TSoftObjectPtr<UWorld>> Maps;

	UPROPERTY(EditDefaultsOnly)
	FName DefaultPlayerStartTag;
	
	FString GetMapNameFromMapAssertName(FString InMapAssertName);
	
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	void PlayerDeath(ACharacter* DeathPlayer);
protected:
	virtual void BeginPlay() override;
};
