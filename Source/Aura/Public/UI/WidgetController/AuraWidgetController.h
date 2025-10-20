// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AuraWidgetController.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
/**
 * WidgetController父类，设置属性PlayerController,PlayerState,ASC,AtrributeSet
 */

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()
	
	FWidgetControllerParams(){};
	FWidgetControllerParams(APlayerController* PC,APlayerState* PS,UAbilitySystemComponent* ASC, UAttributeSet* AS)
	:PlayerController(PC),PlayerState(PS),AbilitySystemComponent(ASC),AttributeSet(AS){};

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState= nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent>AbilitySystemComponent = nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;
};


UCLASS()
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);

	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues();
	
	virtual void BindCallbacksToDependencies();
	
protected:
	UPROPERTY(BlueprintReadOnly,Category = "WidgetController")
	TObjectPtr<APlayerController> PlayerController;
	
	UPROPERTY(BlueprintReadOnly,Category = "WidgetController")
	TObjectPtr<APlayerState> PlayerState;
	
	UPROPERTY(BlueprintReadOnly,Category = "WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(BlueprintReadOnly,Category = "WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;
	
};
