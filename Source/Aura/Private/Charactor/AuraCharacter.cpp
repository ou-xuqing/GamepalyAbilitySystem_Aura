// Copyright Ziger


#include "Charactor/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Aura/Public/AbilitySystem/AuraAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
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

void AAuraCharacter::PossessedBy(AController* NewController)//在此时，PC已经设置完毕（Pawn被控制时会调用此函数，此时Pawn会设置自己的Controller）
{
	//work in server
	Super::PossessedBy(NewController);//Controller设置完毕
	InitAbilityActorInfo();//此时初始化可以取到controller了
	LoadProgress();
}

void AAuraCharacter::LoadProgress()
{
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	ULoadScreenSaveGame* SaveData = GameMode->GetCurGameSaveData();
	if (SaveData == nullptr) return;

	//第一次创建人物（之前没有保存数据时）不能从磁盘中获取属性所以需要赋给默认值, 可以用一个bool标记状态
	if (SaveData->bFirstLoad)//在第一个检查点处变为false（saveprogress函数）
	{
		InitializeDefaultAttributes();
		AddCharacterAbilities();
	}else
	{
		if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
		{
			AuraPlayerState->SetLevel(SaveData->PlayerLevel);
			AuraPlayerState->SetXP(SaveData->XP);
			AuraPlayerState->SetSpellPoints(SaveData->SpellPoints);
			AuraPlayerState->SetAttributePoints(SaveData->AttributePoints);
		}
		UAuraAbilitySystemLibrary::InitializeDefaultAttributesFromDisk(this,AbilitySystemComponent,SaveData);
		//Load Ability From Disk
		UAuraAbilitySystemComponent* Asc = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());
		Asc->LoadAbilitiesFromDisk(SaveData);
	}
	GameMode->LoadWorldState(GetWorld());
}

void AAuraCharacter::OnRep_PlayerState()//因为PC不会复制到客户端，当character生成时，PC将Character与PlayerStatus绑定到一起，PS会复制到客户端
{
	//work in client
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();//在PS复制到客户端时进行初始化，因为此时Controller是一定存在的
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
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AuraASC->UpdateAbilityStatus(AuraPS->GetPlayerLevel());
	}
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

void AAuraCharacter::SaveProgress_Implementation(const FName& CheckPointTag)
{
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		ULoadScreenSaveGame* SaveObject = GameMode->GetCurGameSaveData();
		if (SaveObject == nullptr) return;
		SaveObject->PlayerStartTag = CheckPointTag;
		if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
		{
			SaveObject->PlayerLevel = AuraPlayerState->GetPlayerLevel();
			SaveObject->XP = AuraPlayerState->GetXP();
			SaveObject->AttributePoints = AuraPlayerState->GetAttributePoints();
			SaveObject->SpellPoints = AuraPlayerState->GetSpellPoints();
		}
	
		SaveObject->Strength = UAuraAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
		SaveObject->Intelligence = UAuraAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
		SaveObject->Vigor = UAuraAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());
		SaveObject->Resilence = UAuraAttributeSet::GetResilienceAttribute().GetNumericValue(GetAttributeSet());
		SaveObject->bFirstLoad = false;//第一个检查点处保存

		if (!HasAuthority()) return;
		FForeachAbility ForeachAbility;
		UAuraAbilitySystemComponent* Asc = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());
		SaveObject->SavedAbilities.Empty();
		ForeachAbility.BindLambda([this,Asc,SaveObject](const FGameplayAbilitySpec& AbilitySpec)
		{
			FSavedAbility SavedAbility;
			UAbilityInfo* AbilityInfo= UAuraAbilitySystemLibrary::GetAbilityInfo(this);
			FAuraAbilityInfo Info = AbilityInfo->FindInfoFromAbilityTag(Asc->GetAbilityTagFromSpec(AbilitySpec));
			SavedAbility.AbilityClass = Info.Ability;
			SavedAbility.AbilityTag = Info.AbilityTag;
			SavedAbility.AbilityType = Info.AbilityType;
			SavedAbility.AbilitySlot = Asc->GetSlotFromAbilityTag(Info.AbilityTag);
			SavedAbility.AbilityStatus = Asc->GetStatusTagFromSpec(AbilitySpec);
			SavedAbility.AbilityLevel = AbilitySpec.Level;
			SaveObject->SavedAbilities.AddUnique(SavedAbility);
		});
		Asc->ForEachAbility(ForeachAbility);
		GameMode->SaveCurGameProgress(SaveObject);
		GameMode->SaveWorldState(GetWorld());
	}
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
	//InitializeDefaultAttributes();更换从磁盘加载
	OnAscRegister.Broadcast(AbilitySystemComponent);
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
