// Copyright Ziger


#include "Aura/Public/AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AuraAbilityTypes.h"
#include "SWarningOrErrorBox.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Aura/Public/AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Public/AbilitySystem/AuraAttributeSet.h"
#include "Engine/GameEngine.h"
#include "Engine/OverlapResult.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

struct FAuraGameplayEffectContext;

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
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);

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
//给Enemy赋予能力，因为Aura的能力大部分都要和玩家的操作相关，所以在ASC有单独的一套赋予能力的函数AddCharacterAbility。
void UAuraAbilitySystemLibrary::GiveAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* Asc,ECharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;//因为GetCharacterClassInfo中存在只能在服务器执行的函数，所以当取不到Info时就直接跳过
	for (TSubclassOf<UGameplayAbility> AbilityClass:CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec =  FGameplayAbilitySpec(AbilityClass,1);
		Asc->GiveAbility(AbilitySpec);
	}
	//不同职业有不同的Info
	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetDefaultCharacterClassInfo(CharacterClass);
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Asc->GetAvatarActor()))
	{
		for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
		{
			//和上面不同，因为上面是一些公共能力，比如受击表现和死亡等，不需要等级。但是这些是和攻击相关的，需要等级。
			FGameplayAbilitySpec AbilitySpec =  FGameplayAbilitySpec(AbilityClass,CombatInterface->GetPlayerLevel());
			Asc->GiveAbility(AbilitySpec);
		}
	}

}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	//GetGameMode只能在服务器端调用，客户端无权限
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (GameMode == nullptr) return nullptr;
	return GameMode->CharacterClassInfo;
}

bool UAuraAbilitySystemLibrary::IsBlocked(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsBlockedHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCritical(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsCriticalHit();
	}
	return false;
}

void UAuraAbilitySystemLibrary::SetBlocked(FGameplayEffectContextHandle& EffectContextHandle, bool bBlocked)
{
	FGameplayEffectContext* EffectContext = EffectContextHandle.Get();
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContext))
	{
		AuraEffectContext->SetIsBlockedHit(bBlocked);
	}
	
}

void UAuraAbilitySystemLibrary::SetCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bCriticalHit)
{
	FGameplayEffectContext* EffectContext = EffectContextHandle.Get();
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContext))
	{
		AuraEffectContext->SetIsCriticalHit(bCriticalHit);
	}
}

//参考ApplyRadialDamageWithFalloff
void UAuraAbilitySystemLibrary::GetLiversWithinRadius(const UObject* WorldContextObject,
	TArray<AActor*>& OutOverlappingActor, const TArray<AActor*>& ActorsToIgnore, float Radius, FVector SphereOrigin)
{
	//设置碰撞查询参数，内部有许多参数，存在默认的构造
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);
	//碰撞结果
	TArray<FOverlapResult> Overlaps;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		//找到球形范围内碰撞的结果
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult & OverlapResult : Overlaps)
		{
			//实现CombatInterface接口和还没死亡的Actor加入到输出数组
			 if (OverlapResult.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(OverlapResult.GetActor()))
			 {
				 OutOverlappingActor.AddUnique(ICombatInterface::Execute_GetAvator(OverlapResult.GetActor()));
			 }
		}
	}
}

bool UAuraAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SeconderActor)
{
	const bool IsPlayer =  FirstActor->ActorHasTag(FName("Player")) && SeconderActor->ActorHasTag(FName("Player"));
	const bool IsEnemy = FirstActor->ActorHasTag(FName("Enemy")) && SeconderActor->ActorHasTag(FName("Enemy"));

	return !(IsPlayer || IsEnemy);
}
