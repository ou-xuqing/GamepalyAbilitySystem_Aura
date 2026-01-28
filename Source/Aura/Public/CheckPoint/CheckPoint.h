// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "CheckPoint.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class AURA_API ACheckPoint : public APlayerStart
{
	GENERATED_BODY()
public:
	//和父类的构造函数类似
	ACheckPoint(const FObjectInitializer& ObjectInitializer);

protected:
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;

	//在蓝图中修改Actor参数
	UFUNCTION(BlueprintImplementableEvent)
	void CheckPointReached(UMaterialInstanceDynamic* DynamicMaterialInstance);

	//生成动态材质并且调用CPR函数
	void HandleGlowEffect();
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> CheckPointMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
};
