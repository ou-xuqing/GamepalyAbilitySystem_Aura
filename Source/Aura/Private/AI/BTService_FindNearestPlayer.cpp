// Copyright Ziger


#include "AI/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* OwingPawn = AIOwner->GetPawn();
	//Player的目标是Enemy，反之Enemy的目标是player
	const FName TargetTag= OwingPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

	TArray<AActor*> ActorsWithTag;
	UGameplayStatics::GetAllActorsWithTag(OwingPawn,TargetTag,ActorsWithTag);

	float ClosetDistance = TNumericLimits<float>::Max();
	AActor* ClosetActor = nullptr;

	for (AActor* Actor : ActorsWithTag)
	{
		if (IsValid(Actor) && IsValid(OwingPawn))
		{
			const float Distance = OwingPawn->GetDistanceTo(Actor);
			if (Distance < ClosetDistance)
			{
				ClosetDistance = Distance;
				ClosetActor = Actor;
			}
		}
	}

	UBTFunctionLibrary::SetBlackboardValueAsObject(this,TargetToFollowSelector,ClosetActor);
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this,DistanceToTargetSelector,ClosetDistance);
}
