// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ObjectRename.h"
#include "AuraUserWidget.generated.h"

/**
 * 绑定一个widgetController
 * SetWidgetController中调用WidgetControllerSet
 * WidgetControllerSet由蓝图实现
 * 每次为widget设置controller的时候就会调用方法WidgetControllerSet
 * 然后对于不同的widget可以在蓝图中编写不同的WidgetControllerSet方法
 */
UCLASS()
class AURA_API UAuraUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
};
