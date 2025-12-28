// Copyright Ziger


#include "Actor/PointCollectionActor.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APointCollectionActor::APointCollectionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Pt_0 = CreateDefaultSubobject<USceneComponent>("Pt_0");
	PointCollection.Add(Pt_0);
	SetRootComponent(Pt_0);
	
	Pt_1 = CreateDefaultSubobject<USceneComponent>("Pt_1");
	PointCollection.Add(Pt_1);
	Pt_1->SetupAttachment(GetRootComponent());
	
	Pt_2 = CreateDefaultSubobject<USceneComponent>("Pt_2");
	PointCollection.Add(Pt_2);
	Pt_2->SetupAttachment(GetRootComponent());
	
	Pt_3 = CreateDefaultSubobject<USceneComponent>("Pt_3");
	PointCollection.Add(Pt_3);
	Pt_3->SetupAttachment(GetRootComponent());
	
	Pt_4 = CreateDefaultSubobject<USceneComponent>("Pt_4");
	PointCollection.Add(Pt_4);
	Pt_4->SetupAttachment(GetRootComponent());
	
	Pt_5 = CreateDefaultSubobject<USceneComponent>("Pt_5");
	PointCollection.Add(Pt_5);
	Pt_5->SetupAttachment(GetRootComponent());
	
	Pt_6 = CreateDefaultSubobject<USceneComponent>("Pt_6");
	PointCollection.Add(Pt_6);
	Pt_6->SetupAttachment(GetRootComponent());
	
	Pt_7 = CreateDefaultSubobject<USceneComponent>("Pt_7");
	PointCollection.Add(Pt_7);
	Pt_7->SetupAttachment(GetRootComponent());
	
	Pt_8 = CreateDefaultSubobject<USceneComponent>("Pt_8");
	PointCollection.Add(Pt_8);
	Pt_8->SetupAttachment(GetRootComponent());
	
	Pt_9 = CreateDefaultSubobject<USceneComponent>("Pt_9");
	PointCollection.Add(Pt_9);
	Pt_9->SetupAttachment(GetRootComponent());
	
	Pt_10 = CreateDefaultSubobject<USceneComponent>("Pt_10");
	PointCollection.Add(Pt_10);
	Pt_10->SetupAttachment(GetRootComponent());
}

TArray<USceneComponent*> APointCollectionActor::GetGroundPoints(const FVector& GroundLocation, int32 NumPoints,float YawOverride)
{
	checkf(PointCollection.Num() >= NumPoints,TEXT("申请地面水晶生成点越界"));
	TArray<USceneComponent*> PointsCopy;
	for (auto Pt : PointCollection)
	{
		if (PointsCopy.Num() >= NumPoints) return PointsCopy;
		if (Pt != Pt_0)
		{
			FVector ToPoint = Pt->GetComponentLocation() - Pt_0->GetComponentLocation();
			ToPoint = ToPoint.RotateAngleAxis(YawOverride,FVector::UpVector);
			Pt->SetWorldLocation(Pt_0->GetComponentLocation() + ToPoint);
		}
		FVector StartPoint = FVector(Pt->GetComponentLocation().X,Pt->GetComponentLocation().Y,Pt->GetComponentLocation().Z + 500.f);
		FVector EndPoint = FVector(Pt->GetComponentLocation().X,Pt->GetComponentLocation().Y,Pt->GetComponentLocation().Z - 500.f);

		FHitResult HitResult;
		TArray<AActor*> IngActors;
		UAuraAbilitySystemLibrary::GetLiversWithinRadius(this,IngActors,TArray<AActor*>(),1500.f,GroundLocation);
		FCollisionQueryParams Params;
		Params.AddIgnoredActors(IngActors);
		GetWorld()->LineTraceSingleByProfile(HitResult,StartPoint,EndPoint,FName("BlockAll"),Params);
		
		const FVector& AdjustLocation = FVector(Pt->GetComponentLocation().X,Pt->GetComponentLocation().Y,HitResult.ImpactPoint.Z);
		Pt->SetWorldLocation(AdjustLocation);
		Pt->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));
		PointsCopy.Add(Pt);
	}
	return PointsCopy;
}

// Called when the game starts or when spawned
void APointCollectionActor::BeginPlay()
{
	Super::BeginPlay();
	
}



