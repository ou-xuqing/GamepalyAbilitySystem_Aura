// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/HighLightInterface.h"
#include "Interaction/SaveInterface.h"
#include "CheckPoint.generated.h"

class USphereComponent;
/**
 * @class ACheckPoint
 * @brief Represents a checkpoint in the game, inheriting from APlayerStart and implementing ISaveInterface and IHighLightInterface.
 *
 * This class is responsible for managing the state of a checkpoint, including whether it has been reached by the player,
 * handling the glow effect when the checkpoint is activated, and providing functionality to highlight the actor.
 */
UCLASS()
class AURA_API ACheckPoint : public APlayerStart, public ISaveInterface, public IHighLightInterface
{
	GENERATED_BODY()
public:
	//和父类的构造函数类似
	ACheckPoint(const FObjectInitializer& ObjectInitializer);

	/*
	 * SaveInterface
	 */
	virtual bool ShouldLoadTransform_Implementation() override {return false;}

	virtual void LoadActor_Implementation() override;
	
	UPROPERTY(SaveGame, BlueprintReadWrite)
	bool bReached = false;

	//若为ture则绑定C++中的overlap函数
	UPROPERTY(EditAnywhere)
	bool bBindOverlap = false;
	/*
	 * HighLightInterface
	 */
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutLocation) override;
protected:
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;

	//在蓝图中修改Actor参数
	UFUNCTION(BlueprintImplementableEvent)
	void CheckPointReached(UMaterialInstanceDynamic* DynamicMaterialInstance);

	//生成动态材质并且调用CPR函数
	UFUNCTION(BlueprintCallable)
	void HandleGlowEffect();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CheckPointMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere; 

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> MoveLocation;

};
