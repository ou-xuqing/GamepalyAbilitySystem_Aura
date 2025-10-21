// Copyright Ziger


#include "Aura/Public/AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Aura/Public/AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Public/AbilitySystem/AuraAttributeSet.h"
#include "Game/AuraGameModeBase.h"
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

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* Asc)
{
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (GameMode == nullptr) return;
	UCharacterClassInfo* CharacterClassInfo = GameMode->CharacterClassInfo;

	FGameplayEffectContextHandle PrimaryContextHandle= Asc->MakeEffectContext();
	PrimaryContextHandle.AddSourceObject(Asc->GetAvatarActor());//MMC对于数值的计算中需要用到SourceObject所以在应用Effect前将SO写入Context中
	const FGameplayEffectSpecHandle PrimarySpecHandle = Asc->MakeOutgoingSpec(CharacterClassInfo->GetDefaultCharacterClassInfo(CharacterClass).PrimaryAttributes,Level,PrimaryContextHandle);
	Asc->ApplyGameplayEffectSpecToSelf(*PrimarySpecHandle.Data.Get());//成功应用，但是调试时未显示

	FGameplayEffectContextHandle SecondaryContextHandle= Asc->MakeEffectContext();
	SecondaryContextHandle.AddSourceObject(Asc->GetAvatarActor());
	const FGameplayEffectSpecHandle SecondarySpecHandle = Asc->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes,Level,SecondaryContextHandle);
	Asc->ApplyGameplayEffectSpecToSelf(*SecondarySpecHandle.Data.Get());
	
	FGameplayEffectContextHandle VitalContextHandle= Asc->MakeEffectContext();
	VitalContextHandle.AddSourceObject(Asc->GetAvatarActor());
	const FGameplayEffectSpecHandle VitalSpecHandle = Asc->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes,Level,VitalContextHandle);
	Asc->ApplyGameplayEffectSpecToSelf(*VitalSpecHandle.Data.Get());
}
