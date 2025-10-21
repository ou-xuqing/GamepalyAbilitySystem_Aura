// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Charactor/AuraCharacterBase.h"
#include "Interaction/AuraEnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

/**
 * 继承IAuraEnemyInterface重写两个高光方法
 * 并且在构造方法中初始化ASC和AttributSet
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IAuraEnemyInterface
{
	GENERATED_BODY()
public:
	AAuraEnemy();
	
	/* EnemyInterface */
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/*CombatInterface */
	virtual int32 GetPlayerLevel() override;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthWidget;

	UPROPERTY(BlueprintAssignable)
	FOnAtrributeChangedSignature OnHealthChangedSignature;

	UPROPERTY(BlueprintAssignable)
	FOnAtrributeChangedSignature OnMaxHealthChangedSignature;
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	virtual void InitializeDefaultAttributes() const override;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character class Default")
	int32 Level=1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Character class Default")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;
	
};
