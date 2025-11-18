// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)//用来标志Montage及相应声音
	FGameplayTag MontageTag;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)//可能会有多种用武器的攻击方式，原本的纯montageTag不能区分，所以将获取武器插槽位置单独抽离出来生成tag
	FGameplayTag CombatSocketTag;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	USoundBase* ImpactSound = nullptr;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI,BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//使用BlueprintNativeEvent后，虚幻宏会定义该函数。
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	TArray<FTaggedMontage> GetAttackMontage();

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	FTaggedMontage GetTaggedMontagebyTag(const FGameplayTag& MontageTag);

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	int32 GetPlayerLevel();
	/**
	* 使用BlueprintImplementableEvent函数无法设为virtual，使用BlueprintNativeEvent函数自动为virtual
	* 若在蓝图中实现则优先级大于C++实现
	*/
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)//在蓝图中调用motionwraping的函数来实现
	void UpdateFacingTarget(const FVector& TargetLocation);
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable) 
	FVector GetCombatSocketLocation(const FGameplayTag& CombatSocketTag);
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	bool IsDead() const;

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	AActor* GetAvator();
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable) 
	UAnimMontage* GetHitReactMontage();

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable) 
	int32 GetMinionsCount();

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void IncreaseMinionCount(int MinionCount);
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable) 
	UNiagaraSystem* GetBloodEffect();

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable) 
	ECharacterClass GetCharacterClass();
	
	virtual void Die() = 0;
};
