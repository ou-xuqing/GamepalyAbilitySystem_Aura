// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "AuraAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 纯公式化制造角色属性，注意重写GetLifetimeReplicatedProps
 * 公式化设置属性的replicate，并且使用宏来得到属性的geter、seter、init
 */

struct FGameplayEffectContextHandle;

USTRUCT(BlueprintType)
struct FEffectProperties  //为了方便后续计算
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FGameplayEffectContextHandle EffectContextHandle;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetASC = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> TargetAvatarActor = nullptr ;

	UPROPERTY()
	TObjectPtr<AController> TargetController = nullptr;

	UPROPERTY()
	TObjectPtr<ACharacter> TargetCharacter = nullptr ;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> SourceAvatarActor = nullptr ;

	UPROPERTY()
	TObjectPtr<AController> SourceController = nullptr;

	UPROPERTY()
	TObjectPtr<ACharacter> SourceCharacter = nullptr ;
};

//函数指针（委托的内部构造）
template<class T>
using TStaticFunPtr = typename TBaseStaticDelegateInstance<T,FDefaultDelegateUserPolicy>::FFuncPtr;


UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UAuraAttributeSet();
	//预处理，修改Attribute的CurrentValue前用来约束修改的值不能越界（但是他的约束并没有改变ASC中的modify的值，只是改变了返回的modify的值）（InstantGameEffect会调用该函数）
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//后处理，在属性(BaseValue)修改完成后进行调用。（InstantGameEffect会调用该函数）
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	//设置可复制属性固定函数
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	TMap<FGameplayTag,TStaticFunPtr<FGameplayAttribute()>> TagsToAttribute;

	//Meta Attributes（不会复制给客户端的属性，只为了计算）
	UPROPERTY(BlueprintReadOnly,Category="Meta Attributes")
	FGameplayAttributeData InComingDamage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,InComingDamage);
	
	UPROPERTY(BlueprintReadOnly,Category="Meta Attributes")
	FGameplayAttributeData InComingXP;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,InComingXP);
	
	//Vital Attributes
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_Health,Category="Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);//宏:GetHealthAttribute,GetHealth,SetHealth,InitHealth
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_Mana,Category="Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana);
	
	//Primary Attributes
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_Strength,Category="Primary Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Strength);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_Intelligence,Category="Primary Attributes")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Intelligence)

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_Resilience,Category="Primary Attributes")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Resilience)

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_Vigor,Category="Primary Attributes")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Vigor)

	//Secondary Attributes
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_Armor,Category="Secondary Attributes")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Armor)

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_ArmorPenetration,Category="Secondary Attributes")
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ArmorPenetration)

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_BlockChance,Category="Secondary Attributes")
	FGameplayAttributeData BlockChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, BlockChance)

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_CriticalHitChance,Category="Secondary Attributes")
	FGameplayAttributeData CriticalHitChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitChance)

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_CriticalHitDamage,Category="Secondary Attributes")
	FGameplayAttributeData CriticalHitDamage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitDamage)

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_CriticalHitResistance,Category="Secondary Attributes")
	FGameplayAttributeData CriticalHitResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitResistance)

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_HealthRegeneration,Category="Secondary Attributes")
	FGameplayAttributeData HealthRegeneration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, HealthRegeneration)

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_ManaRegeneration,Category="Secondary Attributes")
	FGameplayAttributeData ManaRegeneration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ManaRegeneration)
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_MaxHealth,Category="Secondary Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth);
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_MaxMana,Category="Secondary Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_Resilience_Fire,Category="Secondary Attributes")
	FGameplayAttributeData Resilience_Fire;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Resilience_Fire);
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_Resilience_Lightning,Category="Secondary Attributes")
	FGameplayAttributeData Resilience_Lightning;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Resilience_Lightning);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_Resilience_Arcane,Category="Secondary Attributes")
	FGameplayAttributeData Resilience_Arcane;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Resilience_Arcane);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing= OnRep_Resilience_Physical,Category="Secondary Attributes")
	FGameplayAttributeData Resilience_Physical;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Resilience_Physical);
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth)const;

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana)const;

	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength)const;

	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence)const;

	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience)const;

	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor)const;

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor)const;

	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration)const;

	UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance)const;

	UFUNCTION()
	void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance)const;

	UFUNCTION()
	void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage)const;

	UFUNCTION()
	void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance)const;

	UFUNCTION()
	void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration)const;

	UFUNCTION()
	void OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration)const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)const;

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)const;

	UFUNCTION()
	void OnRep_Resilience_Fire(const FGameplayAttributeData& OldResilience_Fire)const;

	UFUNCTION()
	void OnRep_Resilience_Lightning(const FGameplayAttributeData& OldResilience_Lightning)const;
	
	UFUNCTION()
	void OnRep_Resilience_Arcane(const FGameplayAttributeData& OldResilience_Arcane)const;
	
	UFUNCTION()
	void OnRep_Resilience_Physical(const FGameplayAttributeData& OldResilience_Physical)const;
private:
	void SetEffectProperties(const struct FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;

	void ShowFloatingText(const struct FEffectProperties& Props,float Number,bool IsBlocked,bool IsCritical);

	void SendXPEvent(const FEffectProperties& Prop);
};

