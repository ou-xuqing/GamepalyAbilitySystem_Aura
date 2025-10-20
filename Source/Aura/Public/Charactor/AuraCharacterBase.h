// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UWidgetComponent;
class UGameplayAbility;
class UAuraGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
class UAbilitySystemComponent;

/*
 * 角色基础类，继承IAbilitySystemInterface重写GetAbilitySystemComponent方法，不继承这个接口可以用UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent代替
 */


UCLASS(ABSTRACT)
class AURA_API AAuraCharacterBase : public ACharacter,public IAbilitySystemInterface,public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAuraCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const{return AttributeSet;}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> weapon;

	UPROPERTY(EditAnywhere,Category = "Combat")
	FName WeaponTipSocketName;
	
	virtual FVector GetCombatSocketLocation() override;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	

	UPROPERTY(EditAnywhere,Category = "Attribute")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttribute;

	UPROPERTY(EditAnywhere,Category = "Attribute")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttribute;

	UPROPERTY(EditAnywhere,Category = "Attribute")
	TSubclassOf<UGameplayEffect> DefaultVitalAttribute;

	
	
	virtual	void InitAbilityActorInfo();

	void InitializeDefaultAttributes() const;

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass, float Level) const;

	void AddCharacterAbilities();
private:
	UPROPERTY(EditAnywhere,Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
};
