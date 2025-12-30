// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraFireBlast.generated.h"

class AAuraFireBall;
/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBlast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
public:
	virtual FString GetDescription(int Level) override;
	virtual FString GetDescriptionNextLevel(int Level) override;

	UFUNCTION(BlueprintCallable)
	TArray<AAuraFireBall*> SpawnFireBall();
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	int32 NumBlastSpell = 12;
	
private:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAuraFireBall> FireBallClass;
};
