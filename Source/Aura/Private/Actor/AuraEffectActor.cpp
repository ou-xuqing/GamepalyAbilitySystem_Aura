// Copyright Ziger


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AAuraEffectActor::AAuraEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneComponent"));
}

// Called when the game starts or when spawned
void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();
	CalculateLocation = InitialLocation;
	CalculateRotation = GetActorRotation();
}


void AAuraEffectActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	RunningTime += DeltaSeconds;
	float SinPeriod = 2 * PI / SinPeriodConstant;
	if (RunningTime > SinPeriod)
	{
		RunningTime = 0;
	}
	ItemMovement(DeltaSeconds);
}


void AAuraEffectActor::ItemMovement(float DeltaSeconds)
{
	if (bRotator)
	{
		const FRotator DeltaRotation(0.f,DeltaSeconds*RotationRate,0.f);
		CalculateRotation = UKismetMathLibrary::ComposeRotators(CalculateRotation,DeltaRotation);
	}
	if (bSinMovement)
	{
		const float Sine = SinAmplitude * FMath::Sin(RunningTime*SinPeriodConstant);
		CalculateLocation = InitialLocation + FVector(0.f,0.f,Sine);
	}
}


void AAuraEffectActor::StartSinMovement()
{
	bSinMovement = true;
	InitialLocation = GetActorLocation();
	CalculateLocation = InitialLocation;
}

void AAuraEffectActor::StartRotator()
{
	bRotator = true;
	CalculateRotation = GetActorRotation();
}


void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor,TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	if (!bIsApplyToEnemies && TargetActor->ActorHasTag("Enemy")) return;
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
	//不在蓝图中调用Destroy，直接将其合并到AETT函数中。
	if (!bIsInfinite && bDestroyOnEffectApplication)
	{
		Destroy();
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (!bIsApplyToEnemies && TargetActor->ActorHasTag("Enemy")) return;
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
	if (!bIsApplyToEnemies && TargetActor->ActorHasTag("Enemy")) return;
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
	//结束重叠时取消效果
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


