// Copyright Ziger


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

// Sets default values
AAuraEffectActor::AAuraEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneComponent"));
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor,TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent *TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC==nullptr)return;
	check(GameplayEffectClass);
	//important template 通过ActiveGameplayEffectHandle将效果作用于ASC上面
	FGameplayEffectContextHandle GameplayEffectContextHandle = TargetASC->MakeEffectContext(); 
	GameplayEffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle GameplayEffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass,ActorLevel,GameplayEffectContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle =  TargetASC->ApplyGameplayEffectSpecToSelf(*GameplayEffectSpecHandle.Data.Get());
	//important template
	bool bIsInfinite =  GameplayEffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle,TargetASC);
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor,InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor,DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor,InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor,InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor,DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor,InfiniteGameplayEffectClass);
	}
	if (InfiniteEffectRemovalPolicy==EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent *TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (TargetASC==nullptr)return;
		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (auto Handle : ActiveEffectHandles)
		{
			if (Handle.Value==TargetASC)
			{
				TargetASC->RemoveActiveGameplayEffect(Handle.Key,1);
				HandlesToRemove.Add((Handle.Key));//不能在遍历Map时将其从Map中删除，先把key存到array当中
			}
		}
		for (auto& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}

// Called when the game starts or when spawned
void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

