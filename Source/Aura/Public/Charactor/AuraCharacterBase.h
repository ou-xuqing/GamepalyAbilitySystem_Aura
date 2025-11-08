// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UNiagaraSystem;
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
	
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;

	//CombatInterface
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& CombatSocketTag) override;
	
	virtual bool IsDead_Implementation() const override;

	virtual AActor* GetAvator_Implementation() override;

	virtual TArray<FTaggedMontage> GetAttackMontage_Implementation() override;
	
	virtual void Die() override;//在PostAttribute中调用

	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;

	virtual FTaggedMontage GetTaggedMontagebyTag_Implementation(const FGameplayTag& MontageTag) override;

	virtual int32 GetMinionsCount_Implementation() override;

	virtual void IncreaseMinionCount_Implementation(int MinionCount) override;
	
	UPROPERTY(EditAnywhere,Category = "Combat")
	TArray<FTaggedMontage> AttackMontage;
	//CombatInterface

	
	UFUNCTION(NetMulticast,Reliable)//广播标记
	virtual void MultiCastHandleDeath();//广播到客户端


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere,Category = "Combat")
	FName WeaponTipSocketName;

	UPROPERTY(EditAnywhere,Category = "Combat")
	FName LeftHandSocketName;

	UPROPERTY(EditAnywhere,Category = "Combat")
	FName RightHandSocketName;
	
	UPROPERTY(EditAnywhere,Category = "Combat")
	FName TailSocketName;
	
	UPROPERTY()
	bool bIsDead = false;
	
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

	virtual void InitializeDefaultAttributes() const;

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass, float Level) const;

	void AddCharacterAbilities();
	
	//Dissolve material

	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DisMaterialInsDyn);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DisMaterialInsDyn);
	
	UPROPERTY(EditAnywhere,Category= "Dissolve Material")
	TObjectPtr<UMaterialInstance> CharacterDissolveMaterialInsDyn;

	UPROPERTY(EditAnywhere,Category="Dissolve Material")
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInsDyn;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UNiagaraSystem* BloodEffect;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	USoundBase* DeathSound;

	//Minion
	UPROPERTY(EditAnywhere,Category = "Combat")
	int32 MinionsCount = 0;
private:
	UPROPERTY(EditAnywhere,Category = "Abilities")//主要是给Aura设置技能而不是Enemy，可能优化给AC中比较好。Enemy技能设置函数在AuraASLibrary，在自己中调用。
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere,Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	
};
