// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

struct FDamageAbilityEffectParams;
class UAbilityInfo;
class AAuraHUD;
struct FWidgetControllerParams;
class USpellMenuWidgetController;
class UAbilitySystemComponent;
enum class ECharacterClass : uint8;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//三个GetController内部方法相同，所以提取相同部分包装成函数
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject,FWidgetControllerParams& WCParams,AAuraHUD*& AuraHUD);
	//HUD不能随便调用，但是Library可以，所以用Library包装一下，在内部调用HUD的Get
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DefaultAttributes")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject,ECharacterClass CharacterClass,float Level,UAbilitySystemComponent* Asc);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DefaultAttributes")
	static void GiveAbilities(const UObject* WorldContextObject,UAbilitySystemComponent* Asc,ECharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DefaultAttributes")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DefaultAuraAbility")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);

	//蓝图或者PostAttribute中调用Get
	UFUNCTION(BlueprintPure , Category="AuraAbilitySystemLibrary|Effects")
	static bool IsBlocked(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure , Category="AuraAbilitySystemLibrary|Effects")
	static bool IsCritical(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure , Category="AuraAbilitySystemLibrary|Effects")
	static bool IsSuccessfulDeBuff(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure , Category="AuraAbilitySystemLibrary|Effects")
	static bool IsKnockback(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure , Category="AuraAbilitySystemLibrary|Effects")
	static float GetDeBuffDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure , Category="AuraAbilitySystemLibrary|Effects")
	static float GetDeBuffDuration(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure , Category="AuraAbilitySystemLibrary|Effects")
	static float GetDeBuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure , Category="AuraAbilitySystemLibrary|Effects")
	static FGameplayTag GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintCallable,Category= "AuraAbilitySystemLibrary|Effects")
	static FVector GetDeathImpulse(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable,Category= "AuraAbilitySystemLibrary|Effects")
	static FVector GetKnockback(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle);
	
	//ExecCal_Damage中调用Set
	UFUNCTION(BlueprintCallable,Category= "AuraAbilitySystemLibrary|Effects")//蓝图是不建议有set的，最好在C++中set，但是本次选择在蓝图中暴露set
	static void SetBlocked(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,bool bBlocked);

	UFUNCTION(BlueprintCallable,Category= "AuraAbilitySystemLibrary|Effects")//蓝图中未加const的参数默认为输出，所以要加上UPARAM来标志为输入
	static void SetCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,bool bCriticalHit);

	UFUNCTION(BlueprintCallable,Category= "AuraAbilitySystemLibrary|Effects")//蓝图中未加const的参数默认为输出，所以要加上UPARAM来标志为输入
	static void SetIsKnockback(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,bool bKnockback);
	
	UFUNCTION(BlueprintCallable,Category= "AuraAbilitySystemLibrary|Effects")
	static void SetDeBuffParams(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,bool bIsSuccessfulDeBuff,float DeBuffDamage,float DeBuffDuration,float DeBuffFrequency,FGameplayTag DamageType);

	UFUNCTION(BlueprintCallable,Category= "AuraAbilitySystemLibrary|Effects")//需要取出来用，所以单独Set和Get
	static void SetDeathImpulse(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,FVector InDeathImpulse);

	UFUNCTION(BlueprintCallable,Category= "AuraAbilitySystemLibrary|Effects")
	static void SetKnockback(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,FVector InKnockback);
	
	UFUNCTION(BlueprintCallable,Category= "AuraAbilitySystemLibrary|GameplayMechanics")
	static void GetLiversWithinRadius(const UObject* WorldContextObject,TArray<AActor*>& OutOverlappingActor,const TArray<AActor*>& ActorsToIgnore,float Radius,FVector SphereOrigin);
	
	//通过Tag来检查，但此Tag不是GameplayTag
	UFUNCTION(BlueprintCallable,Category= "AuraAbilitySystemLibrary|GameplayMechanics")
	static bool IsNotFriend(AActor* FirstActor,AActor* SeconderActor);

	UFUNCTION(BlueprintCallable,Category= "AuraAbilitySystemLibrary|GameplayMechanics")
	static int32 GetRewardXPForClassAndLevel(const UObject* WorldContextObject,ECharacterClass CharacterClass,int32 Level);

	UFUNCTION(BlueprintCallable,Category= "AuraAbilitySystemLibrary|GameplayMechanics")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageAbilityEffectParams& DamageParams);
};
