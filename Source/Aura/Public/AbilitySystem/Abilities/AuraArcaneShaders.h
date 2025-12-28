// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "Actor/MagicCircle.h"
#include "AuraArcaneShaders.generated.h"


/**
 * 与技能释放有bug，当把该技能放到左键时，会和移动冲突
 */
UCLASS()
class AURA_API UAuraArcaneShaders : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
public:

	virtual FString GetDescription(int Level) override;
	virtual FString GetDescriptionNextLevel(int Level) override;
	
	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);

	UFUNCTION(BlueprintCallable)
	void HideMagicCircle();

protected:
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMagicCircle> MagicCircleClass;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AMagicCircle> MagicCircle;

	UPROPERTY(BlueprintReadOnly)
	int32 NumsToSpell = 1;
};
