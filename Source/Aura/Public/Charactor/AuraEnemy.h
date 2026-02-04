// Copyright Ziger

#pragma once

#include "CoreMinimal.h"

#include "AI/AuraAIController.h"
#include "Charactor/AuraCharacterBase.h"
#include "Interaction/AuraEnemyInterface.h"
#include "Interaction/HighLightInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

/**
 * 继承IAuraEnemyInterface重写两个高光方法
 * 并且在构造方法中初始化ASC和AttributSet
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IAuraEnemyInterface, public IHighLightInterface
{
	GENERATED_BODY()
public:
	AAuraEnemy();
	
	/* EnemyInterface */
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutLocation) override;
	//Set在BTT_Attack中设置（通过读取行为树中的targetToFollow）
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() override;//Get直接在蓝图中调用
	/*CombatInterface */
	virtual int32 GetPlayerLevel_Implementation() override;

	virtual void PossessedBy(AController* NewController) override;
	
	void HitReactTagChanged(FGameplayTag CallbackTag,int32 NewCount);
	void StunTagChanged(FGameplayTag CallbackTag,int32 NewCount);

	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthWidget;

	UPROPERTY(BlueprintAssignable)
	FOnAtrributeChangedSignature OnHealthChangedSignature;

	UPROPERTY(BlueprintAssignable)
	FOnAtrributeChangedSignature OnMaxHealthChangedSignature;

	virtual void Die(FVector InDeathImpulse) override;

	virtual void Knockback(FVector InKnockback) override;

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnLoot();
	
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	virtual void InitializeDefaultAttributes() const override;
	
	UPROPERTY(BlueprintReadOnly,Category="Combat")
	bool bHitReacting = false;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	float BaseWalkSpeed = 250.f;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Combat")
	float LifeSpan = 5.f;
	UPROPERTY(BlueprintReadOnly,Category="Combat")
	bool bStunning = false;
	
	UPROPERTY(BlueprintReadWrite,Category="Combat")
	AActor* CombatTarget;
	
	UPROPERTY(EditAnywhere,Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;
};
