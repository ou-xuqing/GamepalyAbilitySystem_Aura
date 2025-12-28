// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBeamSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
public:

	virtual FString GetDescription(int Level) override;
	virtual FString GetDescriptionNextLevel(int Level) override;
	
	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);
	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();
	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& TargetLocation);
	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTarget(TArray<AActor*>& AdditionalTarget);
	UFUNCTION(BlueprintCallable)
	float GetDamageAtLevel();

	UFUNCTION(BlueprintImplementableEvent)
	void PrimaryTargetDeaded(AActor* DeadActor);

	UFUNCTION(BlueprintImplementableEvent)
	void AdditionalTargetDeaded(AActor* DeadActor);
	
protected:

	UPROPERTY(EditDefaultsOnly,Category="Beam")
	FScalableFloat SpreadRadius;
	
	UPROPERTY(BlueprintReadWrite,Category="Beam")
	FVector HitLocation;
	
	UPROPERTY(BlueprintReadWrite,Category="Beam")
	TObjectPtr<AActor> HitActor;

	UPROPERTY(BlueprintReadWrite,Category="Beam")
	TObjectPtr<APlayerController> OwnerController;

	UPROPERTY(BlueprintReadWrite,Category="Beam")
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(EditDefaultsOnly,Category="Beam")
	int32 NumsToAdditionalTargets = 5;
};
