#pragma once

#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"
/*
 *因为需要的属性较多所以设置了一个结构体来存储这些属性（其实一个一个设置也行）
 * 这些属性中，有些会放入EffectContext随着同步将数据传递出去，使用时直接在EffectContext中取，有些通过SetMagnitiudebyTag将数据传送出去。
 * 第一种会占用网络资源。第二种会使得项目中有着多种tag，因为是根据tag来取数据的。
 */
USTRUCT(BlueprintType)
struct FDamageAbilityEffectParams
{
	GENERATED_BODY()
	FDamageAbilityEffectParams(){}

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceAsc;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetAsc;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageEffectClass =  nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY(BlueprintReadWrite)
	int32 AbilityLevel = 1;
	
	UPROPERTY(BlueprintReadWrite)
	float Damage = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float DeBuffChance = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float DeBuffDamage = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float DeBuffDuration = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float DeBuffFrequency = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DeathImpulseMagnitude = 0.f;

	UPROPERTY(BlueprintReadWrite)
	FVector DeathImpulse = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite)
	float KnockbackMagnitude = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float KnockbackChance = 0.f;

	UPROPERTY(BlueprintReadWrite)
	FVector Knockback = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	bool bIsRadialDamage = false;

	UPROPERTY(BlueprintReadWrite)
	float RadialDamageInnerRadius = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float RadialDamageOuterRadius = 0.f;

	UPROPERTY(BlueprintReadWrite)
	FVector RadialDamageOrigin = FVector::ZeroVector;
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
	bool IsRadialDamage() const {return bIsRadialDamage;}
	float GetDeBuffDamage() const {return DeBuffDamage;}
	float GetDeBuffDuration() const {return DeBuffDuration;}
	float GetDeBuffFrequency() const {return DeBuffFrequency;}
	TSharedPtr<FGameplayTag> GetDamageType() const {return DamageType;}
	FVector GetDeathImpulse() const {return DeathImpulse;}
	FVector GetKnockback() const {return Knockback;}
	float GetRadialDamageInnerRadius() const {return RadialDamageInnerRadius;}
	float GetRadialDamageOuterRadius() const {return RadialDamageOuterRadius;}
	FVector GetRadialDamageOrigin() const {return RadialDamageOrigin;}

	
	void SetIsCriticalHit(const bool bInIsCriticalHit){bIsCriticalHit = bInIsCriticalHit;}
	void SetIsBlockedHit(const bool bInIsBlockedHit){bIsBlockedHit = bInIsBlockedHit;}
	void SetIsSuccessfulDeBuff(const bool bInIsSuccessfulDeBuff){bIsSuccessfulDebuff = bInIsSuccessfulDeBuff;}
	void SetIsRadialDamage(const bool bInIsRadialDamage){bIsRadialDamage = bInIsRadialDamage;}
	void SetIsKnockback(const bool bInKnockback){bIsKnockback = bInKnockback;}
	void SetDamageType(const TSharedPtr<FGameplayTag>& InDamageType){DamageType = InDamageType;}
	void SetDeBuffDuration(const float InDuration){DeBuffDuration = InDuration;}
	void SetDeBuffDamage(const float InDamage){DeBuffDamage = InDamage;}
	void SetDeBuffFrequency(float InFrequency){DeBuffFrequency = InFrequency;}
	void SetDeathImpulse(const FVector& InDeathImpulse){DeathImpulse = InDeathImpulse;}
	void SetKnockback(const FVector& InKnockback) {Knockback = InKnockback;}
	void SetRadialDamageInnerRadius(const float InRadialDamageInnerRadius){RadialDamageInnerRadius = InRadialDamageInnerRadius;}
	void SetRadialDamageOuterRadius(const float InRadialDamageOuterRadius){RadialDamageOuterRadius = InRadialDamageOuterRadius;}
	void SetRadialDamageOrigin(const FVector& InRadialDamageOrigin){RadialDamageOrigin = InRadialDamageOrigin;}
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
	
	UPROPERTY()
	bool bIsRadialDamage = false;

	UPROPERTY()
	float RadialDamageInnerRadius = 0.f;

	UPROPERTY()
	float RadialDamageOuterRadius = 0.f;

	UPROPERTY()
	FVector RadialDamageOrigin = FVector::ZeroVector;
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
