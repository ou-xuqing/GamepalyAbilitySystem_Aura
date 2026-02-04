// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "AuraEnemySpawnVolume.generated.h"

class AAuraEnemySpawnPoint;
class UBoxComponent;

UCLASS()
class AURA_API AAuraEnemySpawnVolume : public AActor,public ISaveInterface
{
	GENERATED_BODY()
	
public:	
	AAuraEnemySpawnVolume();

	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/*
	 * SaveInterface
	 */
	virtual bool ShouldLoadTransform_Implementation() override {return false;}

	virtual void LoadActor_Implementation() override;
	
	UPROPERTY(BlueprintReadOnly,SaveGame)
	bool bReached = false;
	
	UPROPERTY(EditAnywhere, Category="SpawnPoints")
	TArray<AAuraEnemySpawnPoint*> SpawnPoints;
	
protected:
	virtual void BeginPlay() override;

private:
		
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Box;
};
