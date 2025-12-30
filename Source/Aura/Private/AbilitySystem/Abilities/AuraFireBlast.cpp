// Copyright Ziger


#include "AbilitySystem/Abilities/AuraFireBlast.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraFireBall.h"

FString UAuraFireBlast::GetDescription(int Level)
{
	//三个Get在父类中实现
	const float ManaCost = GetManaCost(Level);
	const float CooldownTime = GetCooldown(Level);
	const int32 CurDamage = GetDamage(Level);
	
	return FString::Printf(TEXT("<Tile>Fire Blast</> \n"
		"<Small>Level: </><Level>%d</> \n"
		"<Small>ManaCost: </><ManaCost>%.1f</> \n"
		"<Small>Cooldown: </><Cooldown>%.1f</> \n"
		"<Default>Launch %d FireBolt,"
		"Cause </><Damage> %d </> <Default>Damage</> \n"
		),
		Level,
		ManaCost,
		CooldownTime,
		NumBlastSpell,
		CurDamage
		);
}

FString UAuraFireBlast::GetDescriptionNextLevel(int Level)
{
	const float ManaCost = GetManaCost(Level);
	const float CooldownTime = GetCooldown(Level);
	const int32 CurDamage = GetDamage(Level);
	
	return FString::Printf(TEXT("<Tile>Next Level</> \n"
		"<Small>Level: </><Level>%d</> \n"
		"<Small>ManaCost: </><ManaCost>%.1f</> \n"
		"<Small>Cooldown: </><Cooldown>%.1f</> \n"
		"<Default>Launch %d FireBolt,"
		"Cause </><Damage> %d </> <Default>Damage</> \n"
		),
		Level,
		ManaCost,
		CooldownTime,
		NumBlastSpell,
		CurDamage
		);
}

TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBall()
{
	TArray<AAuraFireBall*> FireBalls;

	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	TArray<FRotator> Rotators  = UAuraAbilitySystemLibrary::EvenlySpacedRotator(Forward,360.f,FVector::UpVector,NumBlastSpell);
	for (const FRotator& Rotator : Rotators)
	{
		FTransform Transform;
		Transform.SetLocation(Location);
		Transform.SetRotation(Rotator.Quaternion());
		AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>(
			FireBallClass,
			Transform,
			GetOwningActorFromActorInfo(),
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		
		FireBall->DamageParams = MakeDamageAbilityEffectParams();
		FireBall->ExplosionDamageEffectParams = MakeDamageAbilityEffectParams();
		FireBall->SpellActor = GetAvatarActorFromActorInfo();
		FireBalls.Add(FireBall);
		FireBall->FinishSpawning(Transform);
	}
	
	return FireBalls;
}
