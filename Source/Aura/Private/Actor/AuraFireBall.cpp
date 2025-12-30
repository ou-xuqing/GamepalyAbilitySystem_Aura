// Copyright Ziger


#include "Actor/AuraFireBall.h"
#include "Components/AudioComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "GameplayCueManager.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interaction/PlayerInterface.h"

void AAuraFireBall::BeginPlay()
{
	Super::BeginPlay();
	StartOutGoingTimeline();
}

void AAuraFireBall::Tick(float DeltaSeconds)
{
	if (bHoming)
	{
		Super::Tick(DeltaSeconds);
	}
}

void AAuraFireBall::OnHit()
{
	if (GetOwner())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetOwner(),FAuraGameplayTags::Get().GameplayCue_FireBlast,CueParams);
	}
	
	if (LoopingSoundComponent) LoopingSoundComponent->Stop();
	bHit = true;
}

void AAuraFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//检查是否符合碰撞条件
	if (!IsCanOverlap(OtherActor)) return;

	if (bHoming && !bHit)
	{
		OnHit();
	}
	
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			DamageParams.TargetAsc = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageParams);
		}else
		{	//碰到墙体等障碍发生爆炸
			OnHit();
			Destroy();
		}
		if (bHoming)
		{
			OnHit();
			FindNearActorAndExplosionDamage();
			Destroy();
		}
	}else
	{
		if (bHoming)
		{
			bHit = true;
		}
	}
}

bool AAuraFireBall::FindNearestActorAndHoming()
{
	TArray<AActor*> HomingActors;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(SpellActor);
	UAuraAbilitySystemLibrary::GetLiversWithinRadius(SpellActor,HomingActors,IgnoreActors,HomingRadius,GetActorLocation());
	if (HomingActors.Num() > 0)
	{
		//选择最近的追踪
		float MinDistance = (GetActorLocation()-HomingActors[0]->GetActorLocation()).Length();
		for (AActor* HActor : HomingActors)
		{
			if (HActor->Implements<UPlayerInterface>())//不追踪玩家
			{
				continue;
			}
			float CurDistance = (GetActorLocation()-HActor->GetActorLocation()).Length();
			if (CurDistance <= MinDistance)
			{
				MinDistance = CurDistance;
				HomingActor = HActor;
			}
		}
		
		if (IsValid(HomingActor))
		{
			//设置追踪参数 TODO::阅读追踪组件源码看看怎么实现
			ProjectileMovement->HomingTargetComponent = HomingActor->GetRootComponent();
			ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(HomingMagnitudeMin,HomingMagnitudeMax);
			ProjectileMovement->bIsHomingProjectile = true;
			//此时激活移动组件，因为开始移动只是通过Timeline控制location
			ProjectileMovement->Activate(true);
			bHoming = true;
			return true;
		}
	}
	return  false;
}

void AAuraFireBall::FindNearActorAndExplosionDamage()
{
	//补充参数，在计算伤害时会调用UE的范围伤害函数，通过给出的范围来计算enemy受到多少伤害
	ExplosionDamageEffectParams.bIsRadialDamage = true;
	ExplosionDamageEffectParams.RadialDamageOrigin = GetActorLocation();
	ExplosionDamageEffectParams.RadialDamageInnerRadius = 50.f;
	ExplosionDamageEffectParams.RadialDamageOuterRadius = 200.f;

	TArray<AActor*> NearActors;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(SpellActor);
	UAuraAbilitySystemLibrary::GetLiversWithinRadius(SpellActor,NearActors,IgnoreActors,200.f,GetActorLocation());

	for (AActor* NearActor : NearActors)
	{
		if (NearActor->Implements<UPlayerInterface>()) continue;
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(NearActor))
		{
			//重新计算方向
			FVector ToTarget = (GetActorLocation() - NearActor->GetActorLocation());
			ToTarget.Normalize();
			ExplosionDamageEffectParams.DeathImpulse = ToTarget * ExplosionDamageEffectParams.DeathImpulseMagnitude;
			ExplosionDamageEffectParams.Knockback = ToTarget * ExplosionDamageEffectParams.KnockbackMagnitude;
			ExplosionDamageEffectParams.TargetAsc = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(ExplosionDamageEffectParams);
		}
	}
}