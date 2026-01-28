// Copyright Ziger


#include "CheckPoint/CheckPoint.h"

#include "Components/SphereComponent.h"
#include "Interaction/PlayerInterface.h"

ACheckPoint::ACheckPoint(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	CheckPointMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("CheckPointMesh"));
	CheckPointMesh->SetupAttachment(RootComponent);
	CheckPointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CheckPointMesh->SetCollisionResponseToAllChannels(ECR_Block);

	Sphere = CreateDefaultSubobject<USphereComponent>(FName("Sphere"));
	Sphere->SetupAttachment(CheckPointMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
}

void ACheckPoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//碰撞的是玩家才会产生效果
	if (OtherActor->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
		HandleGlowEffect();
	}
}

void ACheckPoint::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this,&ACheckPoint::OnSphereOverlap);
}

void ACheckPoint::HandleGlowEffect()
{
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UMaterialInstanceDynamic* MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(CheckPointMesh->GetMaterial(0), this);
	CheckPointMesh->SetMaterial(0, MaterialInstanceDynamic);
	CheckPointReached(MaterialInstanceDynamic);
}


