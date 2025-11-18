// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class ULevelUpInfo;
class UAttributeSet;
class UAbilitySystemComponent;
//不在蓝图中绑定的委托可以不用dynamic
DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerStateChanged,int32);
/**
 * 继承IAbilitySystemInterface重写GetAbiliySystemComponent方法
 * 并且在构造方法中初试化ASC和AttributSet
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState,public IAbilitySystemInterface
{
	GENERATED_BODY()
	
	public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category="Widget Data")
	TObjectPtr<ULevelUpInfo> LevelUpInfo;
	
	AAuraPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const{return AttributeSet;}
	//Level
	FORCEINLINE int32 GetPlayerLevel() const{return Level;}
	void AddToLevel(int32 InLevel);
	void SetLevel(int32 InLevel);
	FPlayerStateChanged OnLevelChangedDelegate;
	//
	
	//XP
	FORCEINLINE int32 GetXP() const{return XPMember;}
	void AddToXP(int32 InXP);
	void SetXP(int32 InXP);
	FPlayerStateChanged OnXPChangedDelegate;
	//

	//AttributePoints
	FORCEINLINE int32 GetAttributePoints() const {return AttributePointsMember;}
	void AddToAttributePoints(int32 InPoints);
	void SetAttributePoints(int32 InPoints);
	FPlayerStateChanged OnAttributePointsChangedDelegate;
	//

	//SpellPoints
	FORCEINLINE int32 GetSpellPoints() const {return SpellPointsMember;}
	void AddToSpellPoints(int32 InPoints);
	void SetSpellPoints(int32 InPoints);
	FPlayerStateChanged OnSpellPointsChangedDelegate;
	//
	
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;//ASC指针，对于AuraCharacter在PlayerState里面初始化
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;


private:
	UPROPERTY(VisibleAnywhere,ReplicatedUsing=OnRep_Level)
	int32 Level=1;

	UPROPERTY(VisibleAnywhere,ReplicatedUsing=OnRep_XP)
	int32 XPMember = 0;

	UPROPERTY(VisibleAnywhere,ReplicatedUsing=OnRep_AttributePoints)
	int32 AttributePointsMember = 0;

	UPROPERTY(VisibleAnywhere,ReplicatedUsing=OnRep_SpellPoints)
	int32 SpellPointsMember = 0;
	
	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UFUNCTION()
	void OnRep_XP(int32 OldXP);

	UFUNCTION()
	void OnRep_AttributePoints(int32 OldXP);

	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints);
};
