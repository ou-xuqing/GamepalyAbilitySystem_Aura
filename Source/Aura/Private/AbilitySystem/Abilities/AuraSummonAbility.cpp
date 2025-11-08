// Copyright Ziger


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();

	float DeltaSpread = SpawnSpread / NumMinions;
	FVector RightOfSpread = Forward.RotateAngleAxis(SpawnSpread / 2.f,FVector::UpVector);
	TArray<FVector> SpawnLocations;
	for (int i = 0;i<NumMinions;i++)
	{
		//从右向左选择位置
		const FVector Direction = RightOfSpread.RotateAngleAxis(-DeltaSpread * i,FVector::UpVector);
		FVector SpawnLocation = Location + Direction * FMath::RandRange(MinSpawnDistance,MaxSpawnDistance);
		//让召唤物生成在地面上
		FHitResult Hit;
		//在召唤点的垂直方向使用射线检测，发生碰撞后选择碰撞点为生成点
		GetWorld()->LineTraceSingleByChannel(Hit,SpawnLocation + FVector(0.f,0.f,400.f),SpawnLocation - FVector(0.f,0.f,400.f),ECC_Visibility);
		if (Hit.bBlockingHit)
		{
			SpawnLocation = Hit.ImpactPoint;
		}
		SpawnLocations.Add(SpawnLocation);
	}
	
	FVector LeftOfSpread = Forward.RotateAngleAxis(- SpawnSpread / 2.f,FVector::UpVector);
	

	return SpawnLocations;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
	int32 Index = FMath::RandRange(0,MinionClass.Num()-1);
	return MinionClass[Index];
}
