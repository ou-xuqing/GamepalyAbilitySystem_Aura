// Copyright Ziger


#include "Aura/Public/AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/AuraLogChannels.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Net/UnrealNetwork.h"
#include "Player/AuraPlayerController.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	//Pirmary Attributes
	TagsToAttribute.Add(GameplayTags.Attribute_Primary_Strength, GetStrengthAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Primary_Resilience, GetResilienceAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Primary_Vigor, GetVigorAttribute);
	//Secondary Attributes
	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_Armor, GetArmorAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Secondary_MaxMana, GetMaxManaAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Resilience_Fire,GetResilience_FireAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Resilience_Lightning,GetResilience_LightningAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Resilience_Arcane,GetResilience_ArcaneAttribute);
	TagsToAttribute.Add(GameplayTags.Attribute_Resilience_Physical,GetResilience_PhysicalAttribute);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//Vital Attribute
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Health,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Mana,COND_None,REPNOTIFY_Always);
	
	//Primary Attribute
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Strength,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Intelligence,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Resilience,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Vigor,COND_None,REPNOTIFY_Always);
	//Secondary Attribute
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Armor,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,ArmorPenetration,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,BlockChance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,CriticalHitChance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,CriticalHitDamage,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,CriticalHitResistance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,HealthRegeneration,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,ManaRegeneration,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxHealth,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxMana,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Resilience_Fire,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Resilience_Lightning,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Resilience_Arcane,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Resilience_Physical,COND_None,REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());//设置Attribute上下限
	}
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	FEffectProperties Props;
	SetEffectProperties(Data, Props);//收集本次Effect的Source和Target信息。
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(),0.f,GetMaxHealth()));
		//UE_LOG(LogTemp,Warning,TEXT("Changed Health on %s, Health %f"),*Props.TargetAvatarActor->GetName(),GetHealth());
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(),0.f,GetMaxMana()));
	}
	if (Data.EvaluatedData.Attribute == GetInComingDamageAttribute())
	{
		float LocalInComingDamage = GetInComingDamage();//元属性不是可复制的，只在服务器中使用，通过这个计算出health的该变量然后改变health，此时进行health的复制
		SetInComingDamage(0.f);
		if (LocalInComingDamage >0.f)
		{
			float NewHealth = GetHealth() - LocalInComingDamage;
			SetHealth(FMath::Clamp(NewHealth,0.f,GetMaxHealth()));
			const bool bFatal = GetHealth() <= 0.f;
			if (bFatal)
			{
				if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor))//通过接口调用减少耦合
				{
					CombatInterface->Die();
				}
				SendXPEvent(Props);
			}else
			{
				FGameplayTagContainer TagContainer = FGameplayTagContainer();
				TagContainer.AddTag(FAuraGameplayTags::Get().Effect_HitReact);
				//props在PostEffect时存储（SetEffectProperties）
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);//通过Tag来调用能力（如果能力有该Tag就调用）
			}
			
			if (Props.SourceCharacter != Props.TargetCharacter)//显示伤害文字
			{
				//格挡或者暴击会显示不同的颜色
				bool IsBlocked = UAuraAbilitySystemLibrary::IsBlocked(Props.EffectContextHandle);
				bool IsCritical = UAuraAbilitySystemLibrary::IsCritical(Props.EffectContextHandle);
				ShowFloatingText(Props,LocalInComingDamage,IsBlocked,IsCritical);
			}
		}
	}
	if (Data.EvaluatedData.Attribute == GetInComingXPAttribute())
	{
		const float LocalInComingXP = GetInComingXP();
		SetInComingXP(0.f);
		//检查是否升级,以及升级后的处理
		if (Props.SourceCharacter->Implements<UPlayerInterface>())
		{
			int32 CurLevel = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);
			int32 CurXP = IPlayerInterface::Execute_GetXP(Props.SourceCharacter) + LocalInComingXP;

			int32 Level = IPlayerInterface::Execute_FindLevelForXP(Props.SourceCharacter,CurXP);
			int32 NumLevelUPs = Level - CurLevel;
			if (NumLevelUPs > 0)
			{
				const int32 AttributeReward = IPlayerInterface::Execute_GetAttributePointsReward(Props.TargetCharacter,CurLevel);
				const int32 SpellPointReward = IPlayerInterface::Execute_GetSpellPointsReward(Props.TargetCharacter,CurLevel);

				IPlayerInterface::Execute_LevelUP(Props.SourceCharacter);
				IPlayerInterface::Execute_AddToPlayerLevel(Props.SourceCharacter,NumLevelUPs);
				IPlayerInterface::Execute_AddToAttributePoints(Props.SourceCharacter,AttributeReward);
				IPlayerInterface::Execute_AddToSpellPoints(Props.SourceCharacter,SpellPointReward);

				SetHealth(GetMaxHealth());
				SetMana(GetMaxMana());

				IPlayerInterface::Execute_AddToXP(Props.SourceCharacter,LocalInComingXP);
			}
			
			IPlayerInterface::Execute_AddToXP(Props.SourceCharacter,LocalInComingXP);
		}
	}
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties& Prop)
{
	if (Prop.TargetCharacter->Implements<UCombatInterface>())
	{
		const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Prop.TargetCharacter);
		ECharacterClass CharacterClass = ICombatInterface::Execute_GetCharacterClass(Prop.TargetCharacter);//蓝图原生事件在C++中必须这样调用
		const int32 XP = UAuraAbilitySystemLibrary::GetRewardXPForClassAndLevel(Prop.TargetCharacter,CharacterClass,TargetLevel);

		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		FGameplayEventData Payload;
		Payload.EventTag = GameplayTags.Attribute_Meta_InComingXP;
		Payload.EventMagnitude = XP;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Prop.SourceCharacter,GameplayTags.Attribute_Meta_InComingXP,Payload);
	}
}


void UAuraAttributeSet::ShowFloatingText(const struct FEffectProperties& Props, float Number,bool IsBlocked,bool IsCritical)
{
	//获得自己的Controller
	if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.SourceCharacter->Controller))
	{	//传递TargetCharacter，给目标生成TextWidget
		PC->ShowDamageNumber(Number,Props.TargetCharacter,IsBlocked,IsCritical);
		return;
	}
	//Enemy攻击时，SourceCharacter为Enemy，它没有AuraPlayerController，所以要用Target来生成
	if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.TargetCharacter->Controller))
	{	//传递TargetCharacter，给目标生成TextWidget
		PC->ShowDamageNumber(Number,Props.TargetCharacter,IsBlocked,IsCritical);
	}
}

void UAuraAttributeSet::SetEffectProperties(const struct FGameplayEffectModCallbackData& Data,
	FEffectProperties& Props) const
{
	//source is cause of effect, target is target of effect。Data是一个结构体，里面包含Effect的各种信息
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	
	if (IsValid(Props.SourceASC)&&Props.SourceASC->AbilityActorInfo.IsValid()&&Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController==nullptr && Props.SourceAvatarActor!=nullptr)
		{
			if (APawn* SourcePawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = SourcePawn->GetController();
			}
		}
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid()&&Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}


void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Health,OldHealth);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxHealth,OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Mana,OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxMana,OldMaxMana);
}

void UAuraAttributeSet::OnRep_Resilience_Fire(const FGameplayAttributeData& OldResilience_Fire) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Resilience_Fire,OldResilience_Fire);
}

void UAuraAttributeSet::OnRep_Resilience_Lightning(const FGameplayAttributeData& OldResilience_Lightning) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Resilience_Lightning,OldResilience_Lightning);
}

void UAuraAttributeSet::OnRep_Resilience_Arcane(const FGameplayAttributeData& OldResilience_Arcane) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Resilience_Arcane,OldResilience_Arcane);
}

void UAuraAttributeSet::OnRep_Resilience_Physical(const FGameplayAttributeData& OldResilience_Physical) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Resilience_Physical,OldResilience_Physical);
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Strength,OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Intelligence,OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Resilience,OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Vigor,OldVigor);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Armor,OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,ArmorPenetration,OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,BlockChance,OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,CriticalHitChance,OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,CriticalHitDamage,OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,CriticalHitResistance,OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,HealthRegeneration,OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,ManaRegeneration,OldManaRegeneration);
}
