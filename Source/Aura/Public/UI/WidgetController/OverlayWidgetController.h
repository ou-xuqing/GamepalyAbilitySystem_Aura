// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase//MessageTable行结构，GameplayTag为主键
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};

struct FOnAttributeChangeData;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAtrributeChangedSignature, float, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetDelegate, FUIWidgetRow, Row);



/**
 * 对需要的AttributeSet中的属性设置委托
 * 其中,BroadcastInitialValues方法是初始化的时候调用委托
 * BindCallbacksToDependencies方法是在属性变化时调用委托，这中间还需通过另一层委托GetGameplayAttributeValueChangeDelegate来实现
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
	UPROPERTY(BlueprintAssignable,category="GAS|Attributes")
	FOnAtrributeChangedSignature OnHealthChanged;//四个属性改变委托，具体在蓝图中实现

	 UPROPERTY(BlueprintAssignable,category="GAS|Attributes")
	 FOnAtrributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable,category="GAS|Attributes")
	FOnAtrributeChangedSignature OnManaChanged;

	 UPROPERTY(BlueprintAssignable,category="GAS|Attributes")
	 FOnAtrributeChangedSignature OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable,category="GAS|Message")
	FMessageWidgetDelegate MessageWidgetDelegate;//拾取物品信息委托，具体在蓝图中实现
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category="Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;//存储以GameTag为主键的表，行结构使用UIWidgetRow，目的是用SubWidget显示pickup的信息

	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);//使用GameplayTag寻找表项的模板方法
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(),TEXT(""));
}
