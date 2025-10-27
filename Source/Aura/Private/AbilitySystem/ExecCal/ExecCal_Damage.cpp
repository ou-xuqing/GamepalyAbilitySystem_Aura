// Copyright Ziger


#include "AbilitySystem/ExecCal/ExecCal_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

struct  AuraDamageStatic
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	AuraDamageStatic()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,Armor,Target,false);//敌人受到攻击所以要拿去敌人的护甲值来计算
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,ArmorPenetration,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,BlockChance,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitChance,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitDamage,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitResistance,Target,false);
	}
};

static const AuraDamageStatic DamageStatic()
{
	static AuraDamageStatic DamageStatic;
	return DamageStatic;
}

UExecCal_Damage::UExecCal_Damage()
{
	//RelevantAttributesToCapture是父类中的数组
	RelevantAttributesToCapture.Add(DamageStatic().ArmorDef);//和MMC中捕获属性相同，不过这里用了宏来定义，而MMC中是手动定义
	RelevantAttributesToCapture.Add(DamageStatic().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatic().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatic().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatic().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatic().CriticalHitResistanceDef);
}

void UExecCal_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                             FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC?SourceASC->GetAvatarActor():nullptr;
	AActor* TargetAvatar = TargetASC?TargetASC->GetAvatarActor():nullptr;

	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);
	
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	
	float Damage = EffectSpec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);//通过Tag取出相应Ability的伤害,在Effect创建时Tag与Value进行的绑定
	
	float TargetBlockChane = 0.f;
	//取出相应属性的值
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatic().BlockChanceDef,EvaluateParameters,TargetBlockChane);
	TargetBlockChane = FMath::Max<float>(0.f,TargetBlockChane);

	//Block
	const bool bBlocked = FMath::RandRange(1,100) < TargetBlockChane;
	Damage = bBlocked ? Damage * 0.5f : Damage;

	//Critical
	if (!bBlocked)
	{
		FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"),FString());
		float SourceCriticalChance = 0.f;
		float TargetCriticalHitResistance = 0.f;
		float SourceCriticalDamage = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatic().CriticalHitChanceDef,EvaluateParameters,SourceCriticalChance);
		SourceCriticalChance = FMath::Max<float>(0.f,SourceCriticalChance);
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatic().CriticalHitResistanceDef,EvaluateParameters,TargetCriticalHitResistance);
		TargetCriticalHitResistance = FMath::Max<float>(0.f,TargetCriticalHitResistance);
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatic().CriticalHitDamageDef,EvaluateParameters,SourceCriticalDamage);
		SourceCriticalDamage = FMath::Max<float>(0.f,SourceCriticalDamage);
		//暴击判定
		const bool bCriticalHit = FMath::RandRange(1,100) < (SourceCriticalChance - TargetCriticalHitResistance * CriticalHitResistanceCurve->Eval(TargetCombatInterface->GetPlayerLevel()));
		Damage = bCriticalHit ? 2.f * Damage + SourceCriticalDamage : Damage;
	}
	
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatic().ArmorDef,EvaluateParameters,TargetArmor);
	TargetArmor = FMath::Max<float>(0.f,TargetArmor);
	
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatic().ArmorPenetrationDef,EvaluateParameters,SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(0.f,SourceArmorPenetration);

	//要从表中取出百分之穿透
	FRealCurve* AromrPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"),FString());
	
	TargetArmor = (TargetArmor - SourceArmorPenetration) * (1.f - AromrPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel()));//固穿与百分比穿透
	TargetArmor = FMath::Max<float>(0.f,TargetArmor);//护甲不能为0

	Damage *= (100 - TargetArmor*0.333f)/100.f;//护甲减伤
	Damage = FMath::Max<float>(5.f,Damage);//最低伤害为5
	
	FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetInComingDamageAttribute(),EGameplayModOp::Additive,Damage);//设置Modify
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
