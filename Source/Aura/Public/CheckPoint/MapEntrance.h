// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "CheckPoint/CheckPoint.h"
#include "MapEntrance.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AMapEntrance : public ACheckPoint
{
	GENERATED_BODY()
public:

	AMapEntrance(const FObjectInitializer& ObjectInitializer);
	
	virtual void LoadActor_Implementation() override;

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY(EditAnywhere)
	FName DestinationPlayerStartTag = FName();

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> DestinationWorld;
};
