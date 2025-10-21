// Copyright Ziger


#include "Charactor/AuraEnemy.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Public/AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/WidgetComponent.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	HealthWidget = CreateDefaultSubobject<UWidgetComponent>("HealthWidget");
	HealthWidget->SetupAttachment(GetRootComponent());
}

void AAuraEnemy::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_RED);
	weapon->SetRenderCustomDepth(true); 
	weapon->SetCustomDepthStencilValue(CUSTOM_RED);
}

void AAuraEnemy::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	weapon->SetRenderCustomDepth(false);
}

int32 AAuraEnemy::GetPlayerLevel()
{
	return Level;
}

void AAuraEnemy:: BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();

	 if(UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthWidget->GetUserWidgetObject()))
	 {
	 	AuraUserWidget->SetWidgetController(this);
	 }
	
	 if (const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet))
	 {
	 	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
	 		   [this,AuraAttributeSet](const FOnAttributeChangeData& Data)
	 		   {
	 			   OnHealthChangedSignature.Broadcast(AuraAttributeSet->GetHealth());
	 		   }
	 	   );
	 	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
	 			   [this,AuraAttributeSet](const FOnAttributeChangeData& Data)
	 		   {
	 			   OnMaxHealthChangedSignature.Broadcast(AuraAttributeSet->GetHealth());//应该是GetMaxHealth()但是有BUG得不到MaxHealth，待解决
	 		   }
	 		   );
	 	OnHealthChangedSignature.Broadcast(AuraAttributeSet->GetHealth());
	 	OnMaxHealthChangedSignature.Broadcast(AuraAttributeSet->GetMaxHealth());
	 }
	
}

void AAuraEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	InitializeDefaultAttributes();
}

void AAuraEnemy::InitializeDefaultAttributes() const
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this,CharacterClass,Level,AbilitySystemComponent);
}
