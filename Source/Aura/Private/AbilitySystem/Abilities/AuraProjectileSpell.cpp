// Copyright Ziger


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	const bool bIsOnServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsOnServer)return;
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		const FVector CombatSocketLocation = CombatInterface->GetCombatSocketLocation();
		FRotator Rotation = (ProjectileTargetLocation-CombatSocketLocation).Rotation();
		Rotation.Pitch = 0.f;
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(CombatSocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetAvatarActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffectClass,GetAbilityLevel(),ASC->MakeEffectContext());
		Projectile->DamageSpecHandle = SpecHandle;
		
		Projectile->FinishSpawning(SpawnTransform);
	}
	
	
}
