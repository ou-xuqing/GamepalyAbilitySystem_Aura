// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature,const FGameplayAbilityTargetDataHandle&,DataHandle);
/**
 * GA是一帧内完成的，使用GATask可以实现异步逻辑（）
 */
UCLASS()
class AURA_API UTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()

public:
	//UFUNCTION中的这些东西是每个AbilityTask基本上都要填写的，蓝图中调用时函数名是DisplayName，具体实现在Activate中写
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",meta=(DisplayName = "TargetDataUnderMouse",HidePin="OwningAbility",DefaultToSelf="OwningAbility", BlueprintInternalUseOnly = "true"))
	static UTargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility *OwningAbility);

	UPROPERTY(BlueprintAssignable)//TargetDataUnderMouse的一个出口
	FMouseTargetDataSignature ValidData;
private:
	//具体干什么在此函数中写
	virtual void Activate() override;
	
	void SendMouseHitData() const;
	
	void OnTargetDataReplicatedCallBack(const FGameplayAbilityTargetDataHandle& DataHandle,FGameplayTag GameplayTag);
};
