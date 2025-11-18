// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "Aura/Public/AbilitySystem/Data/AttributeInfo.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

struct FGameplayAttribute;
class UAuraAttributeSet;
class UAttributeInfo;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature,const FAuraAttributeInfo& ,Info);


UCLASS(BlueprintType, Blueprintable)
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;

	UPROPERTY(BlueprintAssignable,category="GAS|XP")
	FPlayerStateSignatrue OnAttributePointsChangedDelegate;

	UPROPERTY(BlueprintAssignable,category="GAS|XP")
	FPlayerStateSignatrue OnSpellPointsChangedDelegate;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;

	//蓝图可调用函数中不是const的在蓝图中是出口
	UFUNCTION(BlueprintCallable)
	void UpdateAttribute(const FGameplayTag& AttributeTag);
private:
	void BroadcastAttributeInfo(const FGameplayTag& Tag,const FGameplayAttribute& Attribute) const;
};
