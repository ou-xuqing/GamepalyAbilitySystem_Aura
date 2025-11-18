// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "Charactor/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "Player/AuraPlayerState.h"
#include "AuraCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UNiagaraComponent;
/**
 * 玩家控制角色
 * InitAbilityActorInfo初始化玩家角色，从playerstate中获取ASC和AttributeSet复制到自身的属性中，
 * 然后从playerController中得到HUD并初始化HUD(controller,playerstate,AbilitySystemComponent,AtrributeSet)
 * PossessedBy是服务器调用初始化的地方，OnRep_PlayerState是客户端调用初始化的地方
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase,public IPlayerInterface
{
	GENERATED_BODY()
public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	//Player Interface
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void AddToAttributePoints_Implementation(int32 Points) override;
	virtual void AddToSpellPoints_Implementation(int32 Points) override;
	virtual void AddToPlayerLevel_Implementation(int32 InLevel) override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 GetAttributePointsReward_Implementation(int32 Level) const override;
	virtual int32 GetSpellPointsReward_Implementation(int32 Level) const override;
	virtual int32 FindLevelForXP_Implementation(int32 InXP) override;
	virtual void LevelUP_Implementation() override;
	
	/*Combat Interface*/
	virtual int32 GetPlayerLevel_Implementation() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UCameraComponent> TopDownCameraComponent;
private:
	virtual void InitAbilityActorInfo()override ;

	//广播到所有客户端
	UFUNCTION(NetMulticast,Reliable)
	void MulticastLevelUPParticles() const;
};
