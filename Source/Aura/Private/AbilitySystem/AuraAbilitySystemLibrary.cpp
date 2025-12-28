// Copyright Ziger


#include "Aura/Public/AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Aura/Public/AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Public/AbilitySystem/AuraAttributeSet.h"
#include "Engine/OverlapResult.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

struct FAuraGameplayEffectContext;

bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject,FWidgetControllerParams& WCParams,AAuraHUD*& AuraHUD)
{
	if (AAuraPlayerController *PC = Cast<AAuraPlayerController>( UGameplayStatics::GetPlayerController(WorldContextObject, 0)))
	{
		if (AAuraHUD *HUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AuraHUD = HUD;
			AAuraPlayerState*PS = PC->GetPlayerState<AAuraPlayerState>();
			UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(PS->GetAbilitySystemComponent());
			UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(PS->GetAttributeSet());
			WCParams.AbilitySystemComponent = ASC;
			WCParams.AttributeSet = AS;
			WCParams.PlayerController = PC;
			WCParams.PlayerState = PS;
			return true;
		}
	}
	return false;
}

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject,WCParams,AuraHUD))
	{
		return AuraHUD->GetOverlayWidgetController(WCParams);
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject,WCParams,AuraHUD))
	{
		return AuraHUD->GetAttributeMenuWidgetController(WCParams);
	}
	return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject,WCParams,AuraHUD))
	{
		return AuraHUD->GetSpellMenuWidgetController(WCParams);
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
	if (Asc->GetAvatarActor()->Implements<UCombatInterface>())
	{
		for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
		{
			//和上面不同，因为上面是一些公共能力，比如受击表现和死亡等，不需要等级。但是这些是和攻击相关的，需要等级。
			FGameplayAbilitySpec AbilitySpec =  FGameplayAbilitySpec(AbilityClass,ICombatInterface::Execute_GetPlayerLevel(Asc->GetAvatarActor()));
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

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	//GetGameMode只能在服务器端调用，客户端无权限
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (GameMode == nullptr) return nullptr;
	return GameMode->AbilityInfo;
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

bool UAuraAbilitySystemLibrary::IsSuccessfulDeBuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsSuccessfulDeBuff();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsKnockback(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsKnockback();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetDeBuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDeBuffDamage();
	}
	return 0;
}

float UAuraAbilitySystemLibrary::GetDeBuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDeBuffDuration();
	}
	return 0;
}

float UAuraAbilitySystemLibrary::GetDeBuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDeBuffFrequency();
	}
	return 0;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (AuraEffectContext->GetDamageType().IsValid())
		{
			return *AuraEffectContext->GetDamageType();
		}
	}
	return FGameplayTag();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (!AuraEffectContext->GetDeathImpulse().IsZero())
		{
			return AuraEffectContext->GetDeathImpulse();
		}
	}
	return FVector::ZeroVector;
}

FVector UAuraAbilitySystemLibrary::GetKnockback(FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (!AuraEffectContext->GetKnockback().IsZero())
		{
			return AuraEffectContext->GetKnockback();
		}
	}
	return FVector::ZeroVector;
}

bool UAuraAbilitySystemLibrary::IsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsRadialDamage();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetRadialDamageInnerRadius();
	}
	return 0;
}

float UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetRadialDamageOuterRadius();
	}
	return 0;
}

FVector UAuraAbilitySystemLibrary::GetRadialDamageOrigin(FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (!AuraEffectContext->GetRadialDamageOrigin().IsZero())
		{
			return AuraEffectContext->GetRadialDamageOrigin();
		}
	}
	return FVector::ZeroVector;
}

void UAuraAbilitySystemLibrary::SetBlocked(FGameplayEffectContextHandle& EffectContextHandle, bool bInBlocked)
{
	FGameplayEffectContext* EffectContext = EffectContextHandle.Get();
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContext))
	{
		AuraEffectContext->SetIsBlockedHit(bInBlocked);
	}
}

void UAuraAbilitySystemLibrary::SetCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInCriticalHit)
{
	FGameplayEffectContext* EffectContext = EffectContextHandle.Get();
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContext))
	{
		AuraEffectContext->SetIsCriticalHit(bInCriticalHit);
	}
}

void UAuraAbilitySystemLibrary::SetIsKnockback(FGameplayEffectContextHandle& EffectContextHandle, bool bInKnockback)
{
	FGameplayEffectContext* EffectContext = EffectContextHandle.Get();
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContext))
	{
		AuraEffectContext->SetIsKnockback(bInKnockback);
	}
}

void UAuraAbilitySystemLibrary::SetDeBuffParams(FGameplayEffectContextHandle& EffectContextHandle,
                                                bool bIsSuccessfulDeBuff, float DeBuffDamage, float DeBuffDuration, float DeBuffFrequency, FGameplayTag DamageType)
{
	FGameplayEffectContext* EffectContext = EffectContextHandle.Get();
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContext))
	{
		AuraEffectContext->SetIsSuccessfulDeBuff(bIsSuccessfulDeBuff);
		TSharedPtr<FGameplayTag> InDamageType = MakeShared<FGameplayTag>(DamageType);
		AuraEffectContext->SetDamageType(InDamageType);
		AuraEffectContext->SetDeBuffDamage(DeBuffDamage);
		AuraEffectContext->SetDeBuffDuration(DeBuffDuration);
		AuraEffectContext->SetDeBuffFrequency(DeBuffFrequency);
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle,FVector InDeathImpulse)
{
	FGameplayEffectContext* EffectContext = EffectContextHandle.Get();
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContext))
	{
		AuraEffectContext->SetDeathImpulse(InDeathImpulse);
	}
}

void UAuraAbilitySystemLibrary::SetKnockback(FGameplayEffectContextHandle& EffectContextHandle, FVector InKnockback)
{
	FGameplayEffectContext* EffectContext = EffectContextHandle.Get();
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContext))
	{
		AuraEffectContext->SetKnockback(InKnockback);
	}
}

void UAuraAbilitySystemLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle,bool bInRadialDamage)
{
	FGameplayEffectContext* EffectContext = EffectContextHandle.Get();
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContext))
	{
		AuraEffectContext->SetIsRadialDamage(bInRadialDamage);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle& EffectContextHandle,float InRadialDamageInnerRadius)
{
	FGameplayEffectContext* EffectContext = EffectContextHandle.Get();
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContext))
	{
		AuraEffectContext->SetRadialDamageInnerRadius(InRadialDamageInnerRadius);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle& EffectContextHandle,float InRadialDamageOuterRadius)
{
	FGameplayEffectContext* EffectContext = EffectContextHandle.Get();
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContext))
	{
		AuraEffectContext->SetRadialDamageOuterRadius(InRadialDamageOuterRadius);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOrigin(FGameplayEffectContextHandle& EffectContextHandle,FVector InRadialDamageOrigin)
{
	FGameplayEffectContext* EffectContext = EffectContextHandle.Get();
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContext))
	{
		AuraEffectContext->SetRadialDamageOrigin(InRadialDamageOrigin);
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

void UAuraAbilitySystemLibrary::GetClosetTarget(int32 MaxTarget, const TArray<AActor*>& Actors,TArray<AActor*>& OutClosetTarget, const FVector& Origin)
{
	if (Actors.Num() <MaxTarget)
	{
		OutClosetTarget = Actors;
		return;
	}
	
	TArray<double> Distances;
	for (auto& Actor : Actors)
	{
		double Distance = (Origin - Actor->GetActorLocation()).Length();
		Distances.Add(Distance);
	}
	for (int32 i = 0;i < MaxTarget;i++)
	{
		double CurMin = Distances[i];
		int32 CurIndex = i;
		for (int32 j = i;j<Distances.Num();j++)
		{
			if (CurMin > Distances[j])
			{
				CurMin = Distances[j];
				CurIndex = j;
			}
		}
		Distances[CurIndex] = Distances[i];
		OutClosetTarget.AddUnique(Actors[CurIndex]);
	}
}

bool UAuraAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SeconderActor)
{
	const bool IsPlayer =  FirstActor->ActorHasTag(FName("Player")) && SeconderActor->ActorHasTag(FName("Player"));
	const bool IsEnemy = FirstActor->ActorHasTag(FName("Enemy")) && SeconderActor->ActorHasTag(FName("Enemy"));

	return !(IsPlayer || IsEnemy);
}

int32 UAuraAbilitySystemLibrary::GetRewardXPForClassAndLevel(const UObject* WorldContextObject,ECharacterClass CharacterClass, int32 Level)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return 0;
	FCharacterClassDefaultInfo CharacterClassDefaultInfo = CharacterClassInfo->GetDefaultCharacterClassInfo(CharacterClass);
	const int32 XP = CharacterClassDefaultInfo.XPReward.GetValueAtLevel(Level);//已经设置好曲线直接用等级查找
	return XP;
}

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageAbilityEffectParams& DamageParams)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	UAbilitySystemComponent* SourceAsc = DamageParams.SourceAsc;
	FGameplayEffectContextHandle EffectContextHandle = SourceAsc->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAsc->GetAvatarActor());
	FGameplayEffectSpecHandle DamageSpecHandle = SourceAsc->MakeOutgoingSpec(DamageParams.DamageEffectClass,DamageParams.AbilityLevel,EffectContextHandle);
	SetDeathImpulse(EffectContextHandle,DamageParams.DeathImpulse);
	SetKnockback(EffectContextHandle,DamageParams.Knockback);
	SetIsRadialDamage(EffectContextHandle,DamageParams.bIsRadialDamage);
	SetRadialDamageInnerRadius(EffectContextHandle,DamageParams.RadialDamageInnerRadius);
	SetRadialDamageOuterRadius(EffectContextHandle,DamageParams.RadialDamageOuterRadius);
	SetRadialDamageOrigin(EffectContextHandle,DamageParams.RadialDamageOrigin);
	//设置多种伤害参数
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle,DamageParams.DamageType,DamageParams.Damage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle,GameplayTags.DeBuff_Damage,DamageParams.DeBuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle,GameplayTags.DeBuff_Chance,DamageParams.DeBuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle,GameplayTags.DeBuff_Duration,DamageParams.DeBuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle,GameplayTags.DeBuff_Frequency,DamageParams.DeBuffFrequency);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle,GameplayTags.Abilities_Knockback,DamageParams.KnockbackChance);
	
	DamageParams.TargetAsc->ApplyGameplayEffectSpecToSelf(*DamageSpecHandle.Data);
	
	return EffectContextHandle;
}

TArray<FRotator> UAuraAbilitySystemLibrary::EvenlySpacedRotator(const FVector& Forward, const float& Spread,const FVector& Axis, const int32& NumVectors)
{
	TArray<FRotator> Rotators;
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread/2,Axis);
	if (NumVectors > 1)
	{
		float DeltaSpread = Spread/(NumVectors - 1);
		for (int32 i = 0; i < NumVectors; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i,Axis);
			const FRotator CurRotation = Direction.Rotation();
			Rotators.Add(CurRotation);
		}
	}else
	{
		Rotators.Add(Forward.Rotation());	
	}
	return Rotators;
}

TArray<FVector> UAuraAbilitySystemLibrary::EvenlyRotatedVector(const FVector& Forward, const float& Spread,const FVector& Axis, const int32& NumVectors)
{
	TArray<FVector> Vectors;
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread/2,Axis);
	if (NumVectors > 1)
	{
		float DeltaSpread = Spread/(NumVectors - 1);
		for (int32 i = 0; i < NumVectors; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i,Axis);
			Vectors.Add(Direction);
		}
	}else
	{
		Vectors.Add(Forward);	
	}
	return Vectors;
}

