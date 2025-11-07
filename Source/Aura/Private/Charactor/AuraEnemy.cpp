// Copyright Ziger


#include "Charactor/AuraEnemy.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Public/AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	//关闭直接使用控制器旋转
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	//使用运动组件来旋转，因为可以调整旋转速度
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f,360.f,0.f);
	
	HealthWidget = CreateDefaultSubobject<UWidgetComponent>("HealthWidget");
	HealthWidget->SetupAttachment(GetRootComponent());
}

void AAuraEnemy::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_RED);
	Weapon->SetRenderCustomDepth(true); 
	Weapon->SetCustomDepthStencilValue(CUSTOM_RED);
}

void AAuraEnemy::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void AAuraEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AAuraEnemy::GetCombatTarget_Implementation()
{
	return CombatTarget;
}

int32 AAuraEnemy::GetPlayerLevel()
{
	return Level;
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//AI只在服务器上运行
	if (!HasAuthority()) return;
	AuraAIController = Cast<AAuraAIController>(NewController);
	//固定写法，将AC中的blackboard通过BT中的BBA初始化
	AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	AuraAIController->RunBehaviorTree(BehaviorTree);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"),false);
	if (CharacterClass != ECharacterClass::Warrior)
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangerAttacker"),true);
	}else
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangerAttacker"),false);
	}
}

void AAuraEnemy::Die()
{
	SetLifeSpan(LifeSpan);
	if (AuraAIController&&AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"),true);
	}
	Super::Die();
}

void AAuraEnemy:: BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();
	if (HasAuthority())
	{
		UAuraAbilitySystemLibrary::GiveAbilities(this,AbilitySystemComponent,CharacterClass);//赋予能力,在服务器调用
	}
	
	 if(UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthWidget->GetUserWidgetObject()))
	 {
	 	AuraUserWidget->SetWidgetController(this);
	 }
	
	 if (const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet))
	 {
	 	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
	 		   [this,AuraAttributeSet](const FOnAttributeChangeData& Data)
	 		   {
	 			   OnHealthChangedSignature.Broadcast(AuraAttributeSet->GetHealth());//蓝图中绑定了该委托
	 		   }
	 	   );
	 	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
	 			   [this,AuraAttributeSet](const FOnAttributeChangeData& Data)
	 		   {
	 			   OnMaxHealthChangedSignature.Broadcast(AuraAttributeSet->GetMaxHealth());//应该是GetMaxHealth()但是有BUG得不到MaxHealth，待解决
	 		   }
	 		   );

	 	
	 	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Effect_HitReact,EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&AAuraEnemy::HitReactTagChanged
	 	);//TagEvent事件有两种，一种是当该Tag数量发生变化时就调用，还有一种是当该Tag是新赋予或者完全删除时才调用
	 	
	 	OnHealthChangedSignature.Broadcast(AuraAttributeSet->GetHealth());
	 	OnMaxHealthChangedSignature.Broadcast(AuraAttributeSet->GetMaxHealth());
	 }
}

void AAuraEnemy::HitReactTagChanged(FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;//根据tag数量看是否被击中
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f:BaseWalkSpeed;//被击中将移速置为0
	if (AuraAIController&&AuraAIController->GetBlackboardComponent())//AuraAIController只在服务器有效
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"),bHitReacting);
	}
	
}

void AAuraEnemy::InitAbilityActorInfo()//初始化ASC的Actor信息
{
	//客户端和服务器都要调用
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	UAuraAbilitySystemComponent* AuraAsc = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	AuraAsc->AbilityActorInfoSet();
	if (HasAuthority())
	{
		//需要用到GetGameMode，但其只能在服务器调用
		InitializeDefaultAttributes();
	}
}

void AAuraEnemy::InitializeDefaultAttributes() const//赋予属性
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this,CharacterClass,Level,AbilitySystemComponent);
}
