// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UAttributeMenuWidgetController;
struct FWidgetControllerParams;
class UAttributeSet;
class UAbilitySystemComponent;
class UOverlayWidgetController;
class UAuraUserWidget;

/**
 * HUD中包含Widget与WidgetController，如果WidgetController不存在GetOverlayWidgetController也会创建一个新的,并且创建的时候会绑定好属性变化时的委托
 * InitOverlay方法会初始化widget和widgetController，并且通过委托初始化属性值
 */

UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()
public: 
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCparams);
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCparams);

	void InitOverlay(APlayerController* PC, APlayerState* PS,UAbilitySystemComponent* ASC,UAttributeSet* AS);
	
protected:


private:
	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> OverlayWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
	
	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
};
