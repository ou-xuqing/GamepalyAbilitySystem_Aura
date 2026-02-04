// Copyright Ziger


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputComponent.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/AuraEnemyInterface.h"
#include "Interaction/HighLightInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	Spline =  CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float Damage, ACharacter* TargetCharacter, bool bBlocked,bool bCriticalHit)
{
	//只在客户端上显示
	if (IsValid(TargetCharacter) && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter,DamageTextComClass);//outer：新对象的输出者
		DamageText->RegisterComponent();//C++中所有组件都要加这一句，如果是在UE中引擎自动加
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(Damage,bBlocked,bCriticalHit);//蓝图事件
	}

}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	
	FInputModeGameAndUI InputModeData;//设置InputMode
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);

}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);//将IMC加入到LocalPlayer当中
	}
	
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);//将InputAction绑定到InputComponent里面
	AuraInputComponent->BindAction(ShiftAction,ETriggerEvent::Started,this,&AAuraPlayerController::ShiftKeyPressed);
	AuraInputComponent->BindAction(ShiftAction,ETriggerEvent::Completed,this,&AAuraPlayerController::ShiftKeyReleased);
	AuraInputComponent->BindAbilityAction(
		InputConfig,
		this,
		&ThisClass::AbilityInputTagPressed,
		&ThisClass::AbilityInputTagReleased,
		&ThisClass::AbilityInputTagHeld);

	
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	if (bAutoRunning)AutoRun();
}

void AAuraPlayerController::AutoRun()
{
	if (APawn* ControlledPawn  = GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(),ESplineCoordinateSpace::World);
		const FVector Direction  = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline,ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (CachedDestination-LocationOnSpline).Length();
		if (DistanceToDestination < AutoRunAcceptanceRadius) bAutoRunning = false;
	}
}
//移动使用Visibility，攻击使用Target
void AAuraPlayerController::CursorTrace()
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_CursorTrace))
	{
		
		if (IsValid(ThisActor) && ThisActor->Implements<UHighLightInterface>()) IHighLightInterface::Execute_UnHighlightActor(ThisActor);
		if (IsValid(LastActor) && LastActor->Implements<UHighLightInterface>())	IHighLightInterface::Execute_UnHighlightActor(LastActor);
		ThisActor = nullptr;
		LastActor = nullptr;
		return;
	}
	if (!GetHitResultUnderCursor(ECC_Visibility,false,CursorHit))return;
	
	LastActor = ThisActor;
	if (IsValid(CursorHit.GetActor()) && CursorHit.GetActor()->Implements<UHighLightInterface>())
	{
		ThisActor =  CursorHit.GetActor();
	}else
	{
		ThisActor = nullptr;
	}
	
	if (ThisActor!=LastActor)
	{
		if (IsValid(ThisActor) && ThisActor->Implements<UHighLightInterface>())	IHighLightInterface::Execute_HighlightActor(ThisActor);
		if (IsValid(LastActor) && LastActor->Implements<UHighLightInterface>())	IHighLightInterface::Execute_UnHighlightActor(LastActor);
	}
}

//Input Function

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPress))
	{
		return;
	}
	if (InputTag.MatchesTag(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (IsValid(ThisActor))
		{
			TargetingStatus = ThisActor->Implements<UAuraEnemyInterface>() ? ETargetingStatus::TargetingEnemy:ETargetingStatus::TargetingNoEnemy;
			bAutoRunning = false;
		}else
		{
			TargetingStatus = ETargetingStatus::NoTargeting;
		}
	}
	if (GetASC()) GetASC()->AbilityInputTagPress(InputTag);
}


void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputRelease))
	{
		return;
	}
	if (!InputTag.MatchesTag(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC())GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}
	if (GetASC())GetASC()->AbilityInputTagReleased(InputTag);
	if (TargetingStatus != ETargetingStatus::TargetingEnemy && !bShiftKeyDown)
	{
		const APawn* ControlledPawn = GetPawn();
		if (FollowTime<ShortPressThreshold && ControlledPawn)
		{
			if (IsValid(ThisActor) && ThisActor->Implements<UHighLightInterface>())
			{
				IHighLightInterface::Execute_SetMoveToLocation(ThisActor,CachedDestination);
			}else
			{
				if (GetASC() && !GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_CursorTrace))
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ClickNiagaraSystem,CachedDestination);
				}
			}
			if (UNavigationPath* NavPath =  UNavigationSystemV1::FindPathToLocationSynchronously(this,ControlledPawn->GetActorLocation(),CachedDestination))
			{
				Spline->ClearSplinePoints();
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc,ESplineCoordinateSpace::World);
				}
				if (NavPath->PathPoints.Num()>0)
				{
					if (!NavPath->PathPoints.IsEmpty()) CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num()-1];//防止CachedDestination无法到达
					bAutoRunning = true;
				}
			}
		}
		FollowTime = 0.f;
		TargetingStatus = ETargetingStatus::NoTargeting;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHeld))
	{
		return;
	}
	if (!InputTag.MatchesTag(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC())GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}
	if (TargetingStatus == ETargetingStatus::TargetingEnemy|| bShiftKeyDown)
	{
		if (GetASC())GetASC()->AbilityInputTagHeld(InputTag);
	}else{
		FollowTime += GetWorld()->GetDeltaSeconds();
		if (CursorHit.bBlockingHit)CachedDestination = CursorHit.ImpactPoint;
        
		if (APawn* ControlledPawn = GetPawn())
		{
			FVector WorldDestination = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDestination);
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}



void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation  = GetControlRotation();
	const FRotator YawRotation = FRotator(0.0f,Rotation.Yaw,0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);//pawn前进方向
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);//pawn右侧方向
	bAutoRunning = false;
	if (APawn * ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection,InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection,InputAxisVector.X);
	}
}


