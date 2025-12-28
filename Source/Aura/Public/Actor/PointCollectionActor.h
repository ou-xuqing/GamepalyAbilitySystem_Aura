// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PointCollectionActor.generated.h"

UCLASS()
class AURA_API APointCollectionActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APointCollectionActor();
	//BlueprintPure在蓝图中没有执行引脚（白色的），并且当输入改变时，函数会被调用。所以如果有一个循环需要使用该函数，并且循环在本次执行中通过该函数获得的值相同，这个时候函数的输入是不能改变的。
	UFUNCTION(BlueprintPure)
	TArray<USceneComponent*> GetGroundPoints(const FVector& GroundLocation,int32 NumPoints,float YawOverride = 0.f);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TArray<USceneComponent*> PointCollection;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<USceneComponent> Pt_0;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<USceneComponent> Pt_1;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<USceneComponent> Pt_2;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<USceneComponent> Pt_3;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<USceneComponent> Pt_4;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<USceneComponent> Pt_5;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<USceneComponent> Pt_6;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<USceneComponent> Pt_7;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<USceneComponent> Pt_8;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<USceneComponent> Pt_9;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<USceneComponent> Pt_10;
};
