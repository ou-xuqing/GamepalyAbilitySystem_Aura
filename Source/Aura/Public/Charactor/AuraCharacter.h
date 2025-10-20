// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "Charactor/AuraCharacterBase.h"
#include "Player/AuraPlayerState.h"
#include "AuraCharacter.generated.h"

/**
 * 玩家控制角色
 * InitAbilityActorInfo初始化玩家角色，从playerstate中获取ASC和AttributeSet复制到自身的属性中，
 * 然后从playerController中得到HUD并初始化HUD(controller,playerstate,AbilitySystemComponent,AtrributeSet)
 * PossessedBy是服务器调用初始化的地方，OnRep_PlayerState是客户端调用初始化的地方
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()
public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/*Combat Interface*/
	virtual int32 GetPlayerLevel() override;
protected:
	virtual void BeginPlay() override;
	
private:
	virtual void InitAbilityActorInfo()override ;
};
