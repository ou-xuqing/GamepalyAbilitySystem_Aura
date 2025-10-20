// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
/**
 * 继承IAbilitySystemInterface重写GetAbiliySystemComponent方法
 * 并且在构造方法中初试化ASC和AttributSet
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState,public IAbilitySystemInterface
{
	GENERATED_BODY()
	
	public:
	AAuraPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const{return AttributeSet;}
	FORCEINLINE int32 GetPlayerLevel() const{return Level;}
	
	protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;//ASC指针，对于AuraCharacter在PlayerState里面初始化
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere,ReplicatedUsing=OnRep_Level)
	int32 Level=1;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);
};
