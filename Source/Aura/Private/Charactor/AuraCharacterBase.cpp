// Copyright Ziger


#include "Charactor/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile,ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile,ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
	
}

FVector AAuraCharacterBase::GetCombatSocketLocation()
{
	check(weapon);
	return weapon->GetSocketLocation(WeaponTipSocketName);
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttribute,1.f);
	ApplyEffectToSelf(DefaultSecondaryAttribute,1.f);
	ApplyEffectToSelf(DefaultVitalAttribute,1.f);
}


void AAuraCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass, float Level) const
{
		check(IsValid(GetAbilitySystemComponent()));
    	check(EffectClass);
    	FGameplayEffectContextHandle GameplayEffectContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
		GameplayEffectContextHandle.AddSourceObject(this);
    	FGameplayEffectSpecHandle GameplayEffectSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(EffectClass,Level,GameplayEffectContextHandle);
    	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*GameplayEffectSpecHandle.Data.Get(),GetAbilitySystemComponent());
}

void AAuraCharacterBase::AddCharacterAbilities()
{
	if (!HasAuthority())return;
	UAuraAbilitySystemComponent *ASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	ASC->AddCharacterAbilities(StartupAbilities);
}

