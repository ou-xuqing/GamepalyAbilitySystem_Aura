// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

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
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject );

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject );

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DefaultAttributes")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject,ECharacterClass CharacterClass,float Level,UAbilitySystemComponent* Asc);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DefaultAttributes")
	static void GiveAbilities(const UObject* WorldContextObject,UAbilitySystemComponent* Asc,ECharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DefaultAttributes")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	//蓝图或者PostAttribute中调用Get
	UFUNCTION(BlueprintPure , Category="AuraAbilitySystemLibrary|Effects")
	static bool IsBlocked(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure , Category="AuraAbilitySystemLibrary|Effects")
	static bool IsCritical(const FGameplayEffectContextHandle& EffectContextHandle);

	//ExecCal_Damage中调用Set
	UFUNCTION(BlueprintCallable,Category= "AuraAbilitySystemLibrary|Effects")//蓝图是不建议有set的，最好在C++中set，但是本次选择在蓝图中暴露set
	static void SetBlocked(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,bool bBlocked);

	UFUNCTION(BlueprintCallable,Category= "AuraAbilitySystemLibrary|Effects")//蓝图中未加const的参数默认为输出，所以要加上UPARAM来标志为输入
	static void SetCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,bool bCriticalHit);

	UFUNCTION(BlueprintCallable,Category= "AuraAbilitySystemLibrary|GameplayMechanics")
	static void GetLiversWithinRadius(const UObject* WorldContextObject,TArray<AActor*>& OutOverlappingActor,const TArray<AActor*>& ActorsToIgnore,float Radius,FVector SphereOrigin);

	UFUNCTION(BlueprintCallable,Category= "AuraAbilitySystemLibrary|GameplayMechanics")
	static bool IsNotFriend(AActor* FirstActor,AActor* SeconderActor);

};
