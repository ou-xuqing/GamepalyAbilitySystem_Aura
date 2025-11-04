// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayAbility;
class UGameplayEffect;

UENUM(BlueprintType)
enum class  ECharacterClass: uint8
{
	Elementalist,
	Warrior,
	Ranger
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,Category="class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	UPROPERTY(EditDefaultsOnly,Category="class Defaults")//每种职业的攻击方式不同
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
};

/**
 * 
 */
UCLASS()//装载在GameMode中，因为该AssetData用来存储Enemy的基本属性和技能，属于GameMode，并且放在GameMode中也便于使用。
class AURA_API UCharacterClassInfo : public UDataAsset 
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,Category="Character class Info")
	TMap<ECharacterClass,FCharacterClassDefaultInfo> DefaultCharacterClassInformation;
	
	UPROPERTY(EditDefaultsOnly,Category="Common class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;

	UPROPERTY(EditDefaultsOnly,Category="Common class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	UPROPERTY(EditDefaultsOnly,Category="Common Ability Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;

	UPROPERTY(EditDefaultsOnly,Category="Common Ability Defaluts | Damage")
	TObjectPtr<UCurveTable> DamageCalculationCoefficients;
	
	FCharacterClassDefaultInfo GetDefaultCharacterClassInfo(ECharacterClass CharacterClass);
};
