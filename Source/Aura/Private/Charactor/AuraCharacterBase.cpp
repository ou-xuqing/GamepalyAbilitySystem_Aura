// Copyright Ziger


#include "Charactor/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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
	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnDebuffComponent");
	BurnDebuffComponent->SetupAttachment(GetRootComponent());
	BurnDebuffComponent->DebuffTag = FAuraGameplayTags::Get().DeBuff_Burn;

	StunDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("StunDebuffComponent");
	StunDebuffComponent->SetupAttachment(GetRootComponent());
	StunDebuffComponent->DebuffTag = FAuraGameplayTags::Get().DeBuff_Stun;

	PassiveAttachComponent = CreateDefaultSubobject<USceneComponent>("PassiveAttachComponent");
	PassiveAttachComponent->SetupAttachment(GetRootComponent());

	HaloPassiveNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("HaloPassiveNiagaraComponent");
	HaloPassiveNiagaraComponent->SetupAttachment(PassiveAttachComponent);
	HaloPassiveNiagaraComponent->PassiveSpellTag = FAuraGameplayTags::Get().Abilities_Passive_HaloOfProtection;
	
	LifePassiveNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("LifePassiveNiagaraComponent");
	LifePassiveNiagaraComponent->SetupAttachment(PassiveAttachComponent);
	LifePassiveNiagaraComponent->PassiveSpellTag = FAuraGameplayTags::Get().Abilities_Passive_LifeSiphon;
	
	ManaPassiveNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("ManaPassiveNiagaraComponent");
	ManaPassiveNiagaraComponent->SetupAttachment(PassiveAttachComponent);
	ManaPassiveNiagaraComponent->PassiveSpellTag = FAuraGameplayTags::Get().Abilities_Passive_ManaSiphon;
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
	return bIsDead;
}

AActor* AAuraCharacterBase::GetAvator_Implementation()
{
	return this;
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontage_Implementation()
{
	return AttackMontage;
}

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

FTaggedMontage AAuraCharacterBase::GetTaggedMontagebyTag_Implementation(const FGameplayTag& MontageTag)
{
	for (FTaggedMontage TaggedMontage:AttackMontage)
	{
		if (TaggedMontage.MontageTag == MontageTag)
		{
			return TaggedMontage;
		}
	}
	return FTaggedMontage();
}

int32 AAuraCharacterBase::GetMinionsCount_Implementation()
{
	return MinionsCount;
}

void AAuraCharacterBase::IncreaseMinionCount_Implementation(int MinionCount)
{
	MinionsCount += MinionCount;
}

ECharacterClass AAuraCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

FOnASCRegister& AAuraCharacterBase::GetOnASCRegisterDelegate()
{
	return OnAscRegister;
}

FOnDeath& AAuraCharacterBase::GetOnDeathDelegate()
{
	return OnDeath;
}

FOnDamage& AAuraCharacterBase::GetOnDamageDelegate()
{
	return OnDamage;
}

void AAuraCharacterBase::Knockback(FVector InKnockback)
{
	LaunchCharacter(InKnockback,true,true);
}

USkeletalMeshComponent* AAuraCharacterBase::GetWeapon_Implementation()
{
	return Weapon;
}

void AAuraCharacterBase::SetBeingShocked_Implementation(bool bInBeingShocked)
{
	bIsBeingShocked = bInBeingShocked;
}

bool AAuraCharacterBase::GetIsBeingShocked_Implementation()
{
	return bIsBeingShocked;
}

void AAuraCharacterBase::Die(FVector InDeathImpulse)
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld,true));//自动复制到客户端
	MultiCastHandleDeath(InDeathImpulse);
}

void AAuraCharacterBase::MultiCastHandleDeath_Implementation(FVector InDeathImpulse)//复制到客户端
{
	UGameplayStatics::PlaySoundAtLocation(this,DeathSound,GetActorLocation(),GetActorRotation());
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->AddImpulse(InDeathImpulse,NAME_None,true);
	
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);
	GetMesh()->AddImpulse(InDeathImpulse * 10,NAME_None,true);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dissolve();
	bIsDead = true;
	BurnDebuffComponent->Deactivate();
	BurnDebuffComponent->Deactivate();
	OnDeath.Broadcast(this);
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
	
}

void AAuraCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraCharacterBase,bIsBeingShocked);
}

float AAuraCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,class AController* EventInstigator, AActor* DamageCauser)
{
	const float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	OnDamage.Broadcast(Damage);
	return Damage;
}

//可以使用TMap<Tag,FName>来进行查找，这里使用三个if是因为本项目只有这三个
FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& CombatSocketTag)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	if (CombatSocketTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon))
	{
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	if (CombatSocketTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if (CombatSocketTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	if (CombatSocketTag.MatchesTagExact(GameplayTags.CombatSocket_Tail))
	{
		return GetMesh()->GetSocketLocation(TailSocketName);
	}
	return FVector();
}

void AAuraCharacterBase::InitializeDefaultAttributes() const //给Aura的初始化和Enemy不同
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

void AAuraCharacterBase::AddCharacterAbilities()//给Aura的能力，和Enemy不同
{
	if (!HasAuthority())return;
	UAuraAbilitySystemComponent *ASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	ASC->AddCharacterAbilities(StartupAbilities);//给Aura设置技能而不是Enemy，因为Enemy不需要输入按键（Tag）
	ASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

void AAuraCharacterBase::Dissolve()
{
	if (IsValid(CharacterDissolveMaterialInsDyn))
	{
		UMaterialInstanceDynamic *DynamicMaterialIns = UMaterialInstanceDynamic::Create(CharacterDissolveMaterialInsDyn,this);
		GetMesh()->SetMaterial(0,DynamicMaterialIns);
		StartDissolveTimeline(DynamicMaterialIns);
	}
	if (IsValid(WeaponDissolveMaterialInsDyn))
	{
		UMaterialInstanceDynamic *DynamicMaterialIns = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInsDyn,this);
		Weapon->SetMaterial(0,DynamicMaterialIns);
		StartWeaponDissolveTimeline(DynamicMaterialIns);
	}
	
}

