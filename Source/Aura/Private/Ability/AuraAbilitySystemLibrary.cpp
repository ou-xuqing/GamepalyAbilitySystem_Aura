// Copyright Ziger


#include "Aura/Public/AbilitySystem/AuraAbilitySystemLibrary.h"

#include "Aura/Public/AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Public/AbilitySystem/AuraAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (AAuraPlayerController *PC = Cast<AAuraPlayerController>( UGameplayStatics::GetPlayerController(WorldContextObject, 0)))
	{
		if (AAuraHUD *HUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState*PS = PC->GetPlayerState<AAuraPlayerState>();
			UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(PS->GetAbilitySystemComponent());
			UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(PS->GetAttributeSet());
			const FWidgetControllerParams WCParams(PC,PS,ASC,AS);
			return HUD->GetOverlayWidgetController(WCParams);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	if (AAuraPlayerController *PC = Cast<AAuraPlayerController>( UGameplayStatics::GetPlayerController(WorldContextObject, 0)))
	{
		if (AAuraHUD *HUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState*PS = PC->GetPlayerState<AAuraPlayerState>();
			UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(PS->GetAbilitySystemComponent());
			UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(PS->GetAttributeSet());
			const FWidgetControllerParams WCParams(PC,PS,ASC,AS);
			return HUD->GetAttributeMenuWidgetController(WCParams);
		}
	}
	return nullptr;
}
