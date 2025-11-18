// Copyright Ziger


#include "Charactor/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Aura/Public/AbilitySystem/AuraAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/AuraPlayerController.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
	//自顶向下的游戏，固定视角，所以相机和弹簧臂不跟着人物旋转 
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;
	
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>("TopDownCameraComponent");
	TopDownCameraComponent->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;
	
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 400.f, 0);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	CharacterClass = ECharacterClass::Elementalist;
}

void AAuraCharacter::PossessedBy(AController* NewController)//在此时，PC已经设置完毕
{
	//work in server
	Super::PossessedBy(NewController);
	InitAbilityActorInfo();
	AddCharacterAbilities();
}

void AAuraCharacter::OnRep_PlayerState()
{
	//work in client
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
	AAuraPlayerState* AuraPS = GetPlayerState<AAuraPlayerState>();
	check(AuraPS);
	AuraPS->AddToXP(InXP);
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 Points)
{
	AAuraPlayerState* AuraPS = GetPlayerState<AAuraPlayerState>();
	check(AuraPS);
	AuraPS->AddToAttributePoints(Points);
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 Points)
{
	AAuraPlayerState* AuraPS = GetPlayerState<AAuraPlayerState>();
	check(AuraPS);
	AuraPS->AddToSpellPoints(Points);
}

void AAuraCharacter::AddToPlayerLevel_Implementation(int32 InLevel)
{
	AAuraPlayerState* AuraPS = GetPlayerState<AAuraPlayerState>();
	check(AuraPS);
	AuraPS->AddToLevel(InLevel);
}

int32 AAuraCharacter::GetXP_Implementation() const
{
	AAuraPlayerState* AuraPS = GetPlayerState<AAuraPlayerState>();
	check(AuraPS);
	return AuraPS->GetXP();
}

int32 AAuraCharacter::GetAttributePointsReward_Implementation(int32 Level) const
{
	AAuraPlayerState* AuraPS = GetPlayerState<AAuraPlayerState>();
	check(AuraPS);
	return AuraPS->LevelUpInfo->LevelUpInformation[Level].AttributePointReward;
}

int32 AAuraCharacter::GetSpellPointsReward_Implementation(int32 Level) const
{
	AAuraPlayerState* AuraPS = GetPlayerState<AAuraPlayerState>();
	check(AuraPS);
	return AuraPS->LevelUpInfo->LevelUpInformation[Level].SpellPointReward;
}

int32 AAuraCharacter::FindLevelForXP_Implementation(int32 InXP)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

void AAuraCharacter::LevelUP_Implementation()
{
	MulticastLevelUPParticles();
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetAttributePoints();
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetSpellPoints();
}

int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();//Apawn中的模板函数
	check(AuraPlayerState);
	//设置OwnerActor(拥有ASC)与AvatarActor(ASC实际作用的对象（ASC给Aura应用Effect）)
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState,this);
	//调用AbilityActorInfoSet，实现Effect委托，其中包含GameplayTag的使用
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();

	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController,AuraPlayerState,AbilitySystemComponent,AttributeSet);
		}
	}
	InitializeDefaultAttributes();
}

void AAuraCharacter::MulticastLevelUPParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		//使得特效总是面对摄像机
		const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation();
		const FVector NiagaraLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCameraRotation = (CameraLocation - NiagaraLocation).Rotation();
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
		
		LevelUpNiagaraComponent->Activate(true);
	}
}
