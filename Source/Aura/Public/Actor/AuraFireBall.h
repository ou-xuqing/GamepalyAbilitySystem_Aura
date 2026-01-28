// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraProjectile.h"
#include "AuraFireBall.generated.h"

/**
 * 第一段释放后不返回，锁定最近目标攻击，然后发生爆炸造成范围伤害（完成）
 * 有两个伤害参数，第一个是碰撞到就造成伤害（在父类中），第二个是爆炸范围伤害ExplosionDamageEffectParams
 */
UCLASS()
class AURA_API AAuraFireBall : public AAuraProjectile
{
	GENERATED_BODY()
public:

	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartOutGoingTimeline();

	virtual void OnHit() override;
	
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> SpellActor;

	UFUNCTION(BlueprintCallable)
	bool FindNearestActorAndHoming();

	UPROPERTY(BlueprintReadWrite,meta=(ExposeOnSpawn=true))
	FDamageAbilityEffectParams ExplosionDamageEffectParams;

	UFUNCTION(BlueprintCallable)
	void FindNearActorAndExplosionDamage();
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly,Category="Homing")
	float HomingRadius = 500.f;
	
	UPROPERTY(EditDefaultsOnly,Category="Homing")
	float HomingMagnitudeMin = 4800.f;
	
	UPROPERTY(EditDefaultsOnly,Category="Homing")
	float HomingMagnitudeMax = 9600.f;

	UPROPERTY(EditDefaultsOnly,Category="Explosion")
    float ExplosionMaxRadius = 200.f;

	UPROPERTY(EditDefaultsOnly,Category="Explosion")
	float ExplosionMinRadius = 50.f;

	bool bHoming = false;

	UPROPERTY()
	TObjectPtr<AActor> HomingActor = nullptr;
};
