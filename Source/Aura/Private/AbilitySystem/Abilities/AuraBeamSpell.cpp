// Copyright Ziger


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

FString UAuraBeamSpell::GetDescription(int Level)
{
	//三个Get在父类中实现
	const float ManaCost = GetManaCost(Level);
	const float CooldownTime = GetCooldown(Level);
	const int32 CurDamage = GetDamage(Level);
	
	return FString::Printf(TEXT("<Tile>Electrocute</> \n"
		"<Small>Level: </><Level>%d</> \n"
		"<Small>ManaCost: </><ManaCost>%.1f</> \n"
		"<Small>Cooldown: </><Cooldown>%.1f</> \n"
		"<Default>Launch %d Electrocute,"
		"Cause </><Damage> %d </> <Default>Damage</> \n"
		),
		Level,
		ManaCost,
		CooldownTime,
		FMath::Min(Level,NumsToAdditionalTargets),
		CurDamage
		);
}

FString UAuraBeamSpell::GetDescriptionNextLevel(int Level)
{
	const float ManaCost = GetManaCost(Level);
	const float CooldownTime = GetCooldown(Level);
	const int32 CurDamage = GetDamage(Level);
	
	return FString::Printf(TEXT("<Tile>Next Level</> \n"
		"<Small>Level: </><Level>%d</> \n"
		"<Small>ManaCost: </><ManaCost>%.1f</> \n"
		"<Small>Cooldown: </><Cooldown>%.1f</> \n"
		"<Default>Launch %d Electrocute,"
		"Cause </><Damage> %d </> <Default>Damage</> \n"
		),
		Level,
		ManaCost,
		CooldownTime,
		FMath::Min(Level,NumsToAdditionalTargets),
		CurDamage
		);
}

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		HitLocation = HitResult.ImpactPoint;
		HitActor = HitResult.GetActor();
	}else
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),GetCurrentActivationInfoRef(),true,false);
	}

}

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
		
	}
	
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& TargetLocation)
{
	if (OwnerCharacter)
	{
		if (OwnerCharacter->Implements<UCombatInterface>())
		{
			FHitResult HitResult;
			USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter);
			FVector Start = Weapon->GetSocketLocation(FName("TipSocket"));
			TArray<AActor*> Ignore;
			Ignore.Add(OwnerCharacter);
			UKismetSystemLibrary::SphereTraceSingle(OwnerCharacter,
				Start,HitLocation,
				10.f,TraceTypeQuery2,false,
				Ignore,EDrawDebugTrace::None,HitResult,true);
			if (HitResult.bBlockingHit)
			{
				HitLocation = HitResult.ImpactPoint;
				HitActor = HitResult.GetActor();
			}
		}
	}
	//HitActor死亡时处理
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(HitActor))
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this,&UAuraBeamSpell::PrimaryTargetDeaded))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this,&UAuraBeamSpell::PrimaryTargetDeaded);
		}
	}
}

void UAuraBeamSpell::StoreAdditionalTarget(TArray<AActor*>& AdditionalTarget)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(HitActor);

	TArray<AActor*> OverlapActors;
	UAuraAbilitySystemLibrary::GetLiversWithinRadius(GetAvatarActorFromActorInfo(),
		OverlapActors,ActorsToIgnore,SpreadRadius.GetValueAtLevel(FMath::Min(GetAbilityLevel(),5)),HitActor->GetActorLocation());

	int32 AdditionalTargets = FMath::Min(GetAbilityLevel(),NumsToAdditionalTargets);
	UAuraAbilitySystemLibrary::GetClosetTarget(AdditionalTargets,OverlapActors,AdditionalTarget,HitActor->GetActorLocation());
	//AdditionalActor死亡时处理
	for (AActor* Actor:AdditionalTarget)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Actor))
		{
			if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this,&UAuraBeamSpell::AdditionalTargetDeaded))
			{
				CombatInterface->GetOnDeathDelegate().AddDynamic(this,&UAuraBeamSpell::AdditionalTargetDeaded);
			}
		}
	}
}

float UAuraBeamSpell::GetDamageAtLevel()
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}
