// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AttributeInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraAttributeInfo
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag AttributeTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FText AttributeName = FText();

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FText AttributeDescription = FText();

	UPROPERTY(BlueprintReadWrite)
	float AttributeValue = 0.f;
};
/**
 * 
 */
UCLASS()
class AURA_API UAttributeInfo : public UDataAsset
{
	GENERATED_BODY()
public:

	FAuraAttributeInfo FindAttributeInfoByTag(const FGameplayTag& Tag, bool bLogIsNotFound = false)const;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TArray<FAuraAttributeInfo> AttributeInformation;
};
