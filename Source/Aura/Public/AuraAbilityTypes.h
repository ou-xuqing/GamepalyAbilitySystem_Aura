#pragma once

#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"
//因为需要的属性较多所以设置了一个结构体来存储这些属性（其实一个一个设置也行）
USTRUCT(BlueprintType)
struct FDamageAbilityEffectParams
{
	GENERATED_BODY()
	FDamageAbilityEffectParams(){}

	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceAsc;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetAsc;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageEffectClass =  nullptr;
	
	UPROPERTY()
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY()
	int32 AbilityLevel = 1;
	
	UPROPERTY()
	float Damage = 0.f;
	
	UPROPERTY()
	float DeBuffChance = 0.f;
	
	UPROPERTY()
	float DeBuffDamage = 0.f;
	
	UPROPERTY()
	float DeBuffDuration = 0.f;
	
	UPROPERTY()
	float DeBuffFrequency = 0.f;

	UPROPERTY()
	float DeathImpulseMagnitude = 0.f;

	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;
	
	UPROPERTY()
	float KnockbackMagnitude = 0.f;
	
	UPROPERTY()
	float KnockbackChance = 0.f;

	UPROPERTY()
	FVector Knockback = FVector::ZeroVector;
	
};



/*
 * 制作自己的EffectContext
 */

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext:public FGameplayEffectContext
{
	GENERATED_BODY()
public:
	bool IsCriticalHit() const {return bIsCriticalHit;}
	bool IsBlockedHit() const {return bIsBlockedHit;}
	bool IsSuccessfulDeBuff() const {return bIsSuccessfulDebuff;}
	bool IsKnockback() const {return bIsKnockback;}
	float GetDeBuffDamage() const {return DeBuffDamage;}
	float GetDeBuffDuration() const {return DeBuffDuration;}
	float GetDeBuffFrequency() const {return DeBuffFrequency;}
	TSharedPtr<FGameplayTag> GetDamageType() const {return DamageType;}
	FVector GetDeathImpulse() const {return DeathImpulse;}
	FVector GetKnockback() const {return Knockback;}
	
	
	void SetIsCriticalHit(bool bInIsCriticalHit){bIsCriticalHit = bInIsCriticalHit;}
	void SetIsBlockedHit(bool bInIsBlockedHit){bIsBlockedHit = bInIsBlockedHit;}
	void SetIsSuccessfulDeBuff(bool bInIsSuccessfulDeBuff){bIsSuccessfulDebuff = bInIsSuccessfulDeBuff;}
	void SetIsKnockback(bool bInKnockback){bIsKnockback = bInKnockback;}
	void SetDamageType(const TSharedPtr<FGameplayTag>& InDamageType){DamageType = InDamageType;}
	void SetDeBuffDuration(float InDuration){DeBuffDuration = InDuration;}
	void SetDeBuffDamage(float InDamage){DeBuffDamage = InDamage;}
	void SetDeBuffFrequency(float InFrequency){DeBuffFrequency = InFrequency;}
	void SetDeathImpulse(FVector InDeathImpulse){DeathImpulse = InDeathImpulse;}
	void SetKnockback(FVector InKnockback) {Knockback = InKnockback;}
	//子类必须覆写
	virtual UScriptStruct* GetScriptStruct() const
	{
		return StaticStruct();
	}

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FAuraGameplayEffectContext* Duplicate() const
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}
	
	//子类必须覆写
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
protected:

	UPROPERTY()
	bool bIsBlockedHit = false;
	UPROPERTY()
	bool bIsCriticalHit = false;
	UPROPERTY()
	bool bIsSuccessfulDebuff = false;
	UPROPERTY()
	bool bIsKnockback = false;
	UPROPERTY()
	float DeBuffDamage = 0.f;
	UPROPERTY()
	float DeBuffDuration = 0.f;
	UPROPERTY()
	float DeBuffFrequency = 0.f;
	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;
	UPROPERTY()
	FVector Knockback = FVector::ZeroVector;
	
	TSharedPtr<FGameplayTag> DamageType;//智能指针有自己的垃圾回收机制,FGameplayTag没有序列化函数，所以包装成智能指针
};

//必要的模板结构体枚举，当在EffectContext中使用相关功能时，需要将相关功能的枚举改为true，父类全是false，子类覆写。具体种类可以去父类里面看
template<>
struct TStructOpsTypeTraits< FAuraGameplayEffectContext >:public TStructOpsTypeTraitsBase2< FAuraGameplayEffectContext >
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true //Copy就是Duplicate，没有更好的词了
	};
};
