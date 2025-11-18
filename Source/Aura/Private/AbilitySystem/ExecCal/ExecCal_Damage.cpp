// Copyright Ziger


#include "AbilitySystem/ExecCal/ExecCal_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"

struct  AuraDamageStatic
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resilience_Fire);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resilience_Lightning);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resilience_Arcane);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resilience_Physical);

	TMap<FGameplayTag,FGameplayEffectAttributeCaptureDefinition> TagToCaptureDefs;
	bool bIsInitTagToCaptureDefs;
	
	AuraDamageStatic()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,Armor,Target,false);//敌人受到攻击所以要拿去敌人的护甲值来计算
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,ArmorPenetration,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,BlockChance,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitChance,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitDamage,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitResistance,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,Resilience_Fire,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,Resilience_Lightning,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,Resilience_Arcane,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,Resilience_Physical,Target,false);
		bIsInitTagToCaptureDefs = false;
	}
//可能是bug来源.在FDamageStatics构造函数里获取Tag实际比Tag注册要早（可能是由于UExecCalc_Damage的构造在CDO阶段，比AuraAssetManager的StartInitialLoading要早，这块我还不太懂）, 所以像视频中一样会导致key为空
	void InitTagToCaptureDefs()
	{
		const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();
		TagToCaptureDefs.Add(AuraGameplayTags.Attribute_Secondary_Armor,ArmorDef);
		TagToCaptureDefs.Add(AuraGameplayTags.Attribute_Secondary_ArmorPenetration,ArmorPenetrationDef);
		TagToCaptureDefs.Add(AuraGameplayTags.Attribute_Secondary_BlockChance,BlockChanceDef);
		TagToCaptureDefs.Add(AuraGameplayTags.Attribute_Secondary_CriticalHitChance,CriticalHitChanceDef);
		TagToCaptureDefs.Add(AuraGameplayTags.Attribute_Secondary_CriticalHitDamage,CriticalHitDamageDef);
		TagToCaptureDefs.Add(AuraGameplayTags.Attribute_Secondary_CriticalHitResistance,CriticalHitResistanceDef);
		
		TagToCaptureDefs.Add(AuraGameplayTags.Attribute_Resilience_Fire,Resilience_FireDef);
		TagToCaptureDefs.Add(AuraGameplayTags.Attribute_Resilience_Lightning,Resilience_LightningDef);
		TagToCaptureDefs.Add(AuraGameplayTags.Attribute_Resilience_Arcane,Resilience_ArcaneDef);
		TagToCaptureDefs.Add(AuraGameplayTags.Attribute_Resilience_Physical,Resilience_PhysicalDef);
		bIsInitTagToCaptureDefs = true;
	}
};

static AuraDamageStatic& DamageStatic()
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
	RelevantAttributesToCapture.Add(DamageStatic().Resilience_FireDef);
	RelevantAttributesToCapture.Add(DamageStatic().Resilience_LightningDef);
	RelevantAttributesToCapture.Add(DamageStatic().Resilience_ArcaneDef);
	RelevantAttributesToCapture.Add(DamageStatic().Resilience_PhysicalDef);
}

void UExecCal_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                             FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC?SourceASC->GetAvatarActor():nullptr;
	AActor* TargetAvatar = TargetASC?TargetASC->GetAvatarActor():nullptr;

	int32 SourceLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourceLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}
	int32 TargetLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>())
	{
		TargetLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}
	
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	if (!DamageStatic().bIsInitTagToCaptureDefs)
	{
		DamageStatic().InitTagToCaptureDefs();
	}
	
	float Damage = 0.f;
	//key:DamageType Value:ResilienceType
	for (auto& Pair:FAuraGameplayTags::Get().DamageTypesToResilience)//一种技能可能会有多种属性伤害
	{
		float Resistance = 0.f;
		//通过value找到ResilienceDef，获取属性值。
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatic().TagToCaptureDefs[Pair.Value],EvaluateParameters,Resistance);
		Resistance = FMath::Clamp(Resistance,0.f,80.f);

		float DamageTypeValue = EffectSpec.GetSetByCallerMagnitude(Pair.Key,false);//通过Tag取出相应Ability的伤害,在Effect创建时Tag与Value进行的绑定
		//百分比减伤
		DamageTypeValue *= (100.f - Resistance) /100.f;
		Damage += DamageTypeValue;
	}
	
	float TargetBlockChane = 0.f;
	//取出相应属性的值
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatic().BlockChanceDef,EvaluateParameters,TargetBlockChane);
	TargetBlockChane = FMath::Max<float>(0.f,TargetBlockChane);

	//Block
	const bool bBlocked = FMath::RandRange(1,100) < TargetBlockChane;
	Damage = bBlocked ? Damage * 0.5f : Damage;
	FGameplayEffectContextHandle EffectContextHandle = EffectSpec.GetContext();
	UAuraAbilitySystemLibrary::SetBlocked(EffectContextHandle,bBlocked);
	
	//Critical
	if (!bBlocked)
	{
		//整个图表中有多个曲线，所以要根据名字找到曲线（ScalabeFloat），如果设置的是曲线，直接找值即可（AuraAbilitySystemLibrary中GetXP）
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
		const bool bCriticalHit = FMath::RandRange(1,100) < (SourceCriticalChance - TargetCriticalHitResistance * CriticalHitResistanceCurve->Eval(TargetLevel));
		Damage = bCriticalHit ? 2.f * Damage + SourceCriticalDamage : Damage;
		UAuraAbilitySystemLibrary::SetCriticalHit(EffectContextHandle,bCriticalHit);
	}
	
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatic().ArmorDef,EvaluateParameters,TargetArmor);
	TargetArmor = FMath::Max<float>(0.f,TargetArmor);
	
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatic().ArmorPenetrationDef,EvaluateParameters,SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(0.f,SourceArmorPenetration);

	//要从表中取出百分之穿透
	FRealCurve* AromrPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"),FString());
	
	TargetArmor = (TargetArmor - SourceArmorPenetration) * (1.f - AromrPenetrationCurve->Eval(SourceLevel));//固穿与百分比穿透
	TargetArmor = FMath::Max<float>(0.f,TargetArmor);//护甲不能为0

	Damage *= (100 - TargetArmor*0.333f)/100.f;//护甲减伤
	Damage = FMath::Max<float>(5.f,Damage);//最低伤害为5
	
	FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetInComingDamageAttribute(),EGameplayModOp::Additive,Damage);//设置Modify
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
