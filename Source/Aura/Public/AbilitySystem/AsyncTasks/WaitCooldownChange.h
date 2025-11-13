// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "WaitCooldownChange.generated.h"

struct FActiveGameplayEffectHandle;
struct FGameplayEffectSpec;
class UAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangeSignatrue, float, TimeRemainig);

/**
 * 
 */
UCLASS(BlueprintType,meta=(ExposedAsyncProxy = "AsyncTask"))
class AURA_API UWaitCooldownChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	//Task的两个出口
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignatrue CooldownStart;
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignatrue CooldownEnd;

	//每个spellGlobe的Cooldown不同所以需要传入，ASC是为了绑定Tag委托
	UFUNCTION(BlueprintCallable , meta= (BlueprintInternalUseOnly = "true" ))
	static UWaitCooldownChange* WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,const FGameplayTag& InCooldownTag);
	UFUNCTION(BlueprintCallable)
	void EndTask();
	//这两个函数的参数都是相应委托所传入的参数
	void CooldownTagChanged(FGameplayTag InCooldownTag,int32 NewCount);
	void OnActiveEffectAdded(UAbilitySystemComponent* TargetAsc,const FGameplayEffectSpec& AppliedSpec,FActiveGameplayEffectHandle ActiveGameplayEffectHandle);
	
protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> Asc;
	UPROPERTY()
	FGameplayTag CooldownTag;
};
