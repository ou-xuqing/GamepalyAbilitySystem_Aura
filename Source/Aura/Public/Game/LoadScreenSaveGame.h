// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/SaveGame.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "LoadScreenSaveGame.generated.h"

USTRUCT()
struct FSavedActor
{
	GENERATED_BODY()
	UPROPERTY()
	FName ActorName = FName();

	UPROPERTY()
	FTransform Transform = FTransform();

	//序列化Actor中被标记为SaveGame的变量
	UPROPERTY()
	TArray<uint8> Bytes;
};

inline bool operator==(const FSavedActor& Left,const FSavedActor& Right)
{
	return Left.ActorName == Right.ActorName;
}

USTRUCT()
struct FSavedMap
{
	GENERATED_BODY()
	UPROPERTY()
	FString MapName = FString();

	UPROPERTY()
	TArray<FSavedActor> SavedActors;
};

USTRUCT()
struct FSavedAbility
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<UGameplayAbility> AbilityClass;
	
	UPROPERTY()
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY()
	FGameplayTag AbilityType = FGameplayTag();

	UPROPERTY()
	FGameplayTag AbilitySlot = FGameplayTag();

	UPROPERTY()
	FGameplayTag AbilityStatus = FGameplayTag();

	UPROPERTY()
	int32 AbilityLevel = 1;
};
inline bool operator==(const FSavedAbility& Left,const FSavedAbility& Right)
{
	return Left.AbilityTag.MatchesTagExact(Right.AbilityTag);
}
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
	 * 存储内容,地图相关
	 */
	UPROPERTY()
	FString PlayerName = FString();

	UPROPERTY()
	TEnumAsByte<ELoadSlotStates> LoadSlotState = Vacant;

	UPROPERTY()
	FString MapName = FString();
	//TSoftObjectPtr<UWorld>存储的世界可以获得AssertName
	UPROPERTY()
	FString MapAssertName = FString();
	
	UPROPERTY()
	FName PlayerStartTag;

	UPROPERTY()
	bool bFirstLoad = true;
	/*
	 * Player
	 */
	UPROPERTY()
	int32 PlayerLevel = 0;

	UPROPERTY()
	int32 XP = 0;

	UPROPERTY()
	int32 AttributePoints = 0;

	UPROPERTY()
	int32 SpellPoints = 0;
	/*
	*Attributes 
	*/
	UPROPERTY()
	float Strength = 0;

	UPROPERTY()
	float Vigor = 0;

	UPROPERTY()
	float Intelligence = 0;

	UPROPERTY()
	float Resilence = 0;

	/*
	 * Ability
	 */
	UPROPERTY()
	TArray<FSavedAbility> SavedAbilities;

	/*
	 * SavedMap
	 */
	UPROPERTY()
	TArray<FSavedMap> SavedMaps;

	FSavedMap GetSavedMapFromMapName(const FString& InMapName);

	bool HasSavedMap(const FString& InMapName);
};
