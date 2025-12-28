// Copyright Ziger


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interaction/CombatInterface.h"

//在ASC中被调用
FString UAuraFireBolt::GetDescription(int Level)
{
	//三个Get在父类中实现
	const float ManaCost = GetManaCost(Level);
	const float CooldownTime = GetCooldown(Level);
	const int32 CurDamage = GetDamage(Level);
	
	return FString::Printf(TEXT("<Tile>Fire Bolt</> \n"
		"<Small>Level: </><Level>%d</> \n"
		"<Small>ManaCost: </><ManaCost>%.1f</> \n"
		"<Small>Cooldown: </><Cooldown>%.1f</> \n"
		"<Default>Launch %d FireBolt,"
		"Cause </><Damage> %d </> <Default>Damage</> \n"
		),
		Level,
		ManaCost,
		CooldownTime,
		FMath::Min(Level,NumProjectileSpell),
		CurDamage
		);
}

FString UAuraFireBolt::GetDescriptionNextLevel(int Level)
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
		FMath::Min(Level,NumProjectileSpell),
		CurDamage
		);
}

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag,bool bIsOverridePitch, float OverridePitch, AActor* Target)
{
	const bool bIsOnServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsOnServer)return;
	//静态函数调用时如果要得到返回值，需要给调用对象（因为静态函数不是通过对象调用）
	const FVector CombatSocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo()
		,SocketTag);
	FRotator Rotation = (ProjectileTargetLocation-CombatSocketLocation).Rotation();
	if (bIsOverridePitch)
	{
		Rotation.Pitch = OverridePitch;
	}
	const FVector Forward = Rotation.Vector();
	//FMath::Min(GetAbilityLevel(),NumProjectileSpell)
	const int NumProjectiles = FMath::Min(GetAbilityLevel(),NumProjectileSpell);

	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotator(Forward,SpawnSpread,FVector::UpVector,NumProjectiles);

	for (FRotator Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(CombatSocketLocation);
		SpawnTransform.SetRotation(Rotator.Quaternion());
			
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetAvatarActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		Projectile->DamageParams = MakeDamageAbilityEffectParams(Target);
		if (Target && Target->Implements<UCombatInterface>())
		{
			Projectile->ProjectileMovement->HomingTargetComponent = Target->GetRootComponent();
		}else
		{
			Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(StaticClass());
			Projectile->HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation);
			Projectile->ProjectileMovement->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
		}
		Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(HomingMagnitudeMin,HomingMagnitudeMax);
		Projectile->ProjectileMovement->bIsHomingProjectile = true;
	
		Projectile->FinishSpawning(SpawnTransform);
	}
}
