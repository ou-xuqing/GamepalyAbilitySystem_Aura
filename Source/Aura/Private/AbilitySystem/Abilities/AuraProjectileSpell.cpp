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

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation,const FGameplayTag& SocketTag,bool bIsOverridePitch,float OverridePitch)
{
	const bool bIsOnServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsOnServer)return;
	//静态函数调用时如果要得到返回值，需要给调用对象（因为静态函数不是通过对象调用）
	const FVector CombatSocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo()
		,SocketTag);
	FRotator Rotation = (ProjectileTargetLocation-CombatSocketLocation).Rotation();
	if (bIsOverridePitch)
	{
		Rotation.Pitch = OverridePitch;
	}
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
	//生成EffectSpecHandle传给Projectile让他在发生overlap后在应用到targetActor上
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffectClass,GetAbilityLevel(),ASC->MakeEffectContext());
	for (auto& Pair:DamageTypes)//一种技能可能会有多种属性伤害
	{
		const float CurDamage =  Pair.Value.GetValueAtLevel(GetAbilityLevel());//根据技能等级分配伤害
		//GE中SetbyCaller，通过调用者设置伤害，根据Tag来找到对应伤害。此时将Tag和DamageValue进行绑定，到应用该Effect时进行计算。
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,Pair.Key,CurDamage);
	}
		
	Projectile->DamageSpecHandle = SpecHandle;
		
	Projectile->FinishSpawning(SpawnTransform);
}
