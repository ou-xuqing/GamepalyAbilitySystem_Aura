// Copyright Ziger


#include "Actor/MagicCircle.h"

#include "Components/DecalComponent.h"

// Sets default values
AMagicCircle::AMagicCircle()
{
	PrimaryActorTick.bCanEverTick = true;
	
	DecalComponent = CreateDefaultSubobject<UDecalComponent>("DecalCompone");
	DecalComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AMagicCircle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMagicCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMagicCircle::SetupLocation(const FVector& HitLocation)
{
	this->SetActorLocation(HitLocation);
}

UDecalComponent* AMagicCircle::GetDecalComponent() const
{
	return DecalComponent;
}

