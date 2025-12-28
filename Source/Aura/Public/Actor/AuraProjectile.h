// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "GameFramework/Actor.h"
#include "GameplayCueNotifyTypes.h"
#include "AuraProjectile.generated.h"


class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class AURA_API AAuraProjectile : public AActor//projectile is actor class
{
	GENERATED_BODY()
	
public:	
	AAuraProjectile();
	virtual void Tick(float DeltaSeconds) override;
	//在C++中实现了,这些参数都是模板参数
	UFUNCTION()	
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OnHit();

	UPROPERTY(BlueprintReadWrite,meta=(ExposeOnSpawn=true))
	FDamageAbilityEffectParams DamageParams;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY()
	TObjectPtr<USceneComponent> HomingTargetSceneComponent;
	//TODO:锁定的目标死亡后自动搜索周围目标进行跟踪
protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
private:

	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 7.f;
	
	bool bHit = false;

	FVector LastLocation;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;
};
