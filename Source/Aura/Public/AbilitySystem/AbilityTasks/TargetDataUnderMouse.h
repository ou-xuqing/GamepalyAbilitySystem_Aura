// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature,const FGameplayAbilityTargetDataHandle&,DataHandle);
/**
 * 
 */
UCLASS()
class AURA_API UTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",meta=(DisplayName = "TargetDataUnderMouse",HidePin="OwningAbility",DefaultToSelf="OwningAbility", BlueprintInternalUseOnly = "true"))
	static UTargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility *OwningAbility);

	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;
private:
	virtual void Activate() override;
	void SendMouseHitData() const;
	void OnTargetDataReplicatedCallBack(const FGameplayAbilityTargetDataHandle& DataHandle,FGameplayTag GameplayTag);
};
