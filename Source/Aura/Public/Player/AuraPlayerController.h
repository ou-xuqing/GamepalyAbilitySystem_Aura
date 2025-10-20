// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "Components/SplineComponent.h"
#include "AuraPlayerController.generated.h"

class UAuraAbilitySystemComponent;
class UAuraInputConfig;
class UInputMappingContext;
struct FInputActionValue;
class IAuraEnemyInterface;
/**
 * 增强输入，设置InputMappingContext和InputAction，在BeginPlay和SetupInputComponent和move中绑定增强输入
 * 在playertick和cursortrace中调用LastActor与ThisActor的高光函数
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;
private:
	UPROPERTY(EditAnywhere,Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere,Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	void Move(const FInputActionValue& InputActionValue);
	
	void CursorTrace();
	TScriptInterface<IAuraEnemyInterface> LastActor;
	TScriptInterface<IAuraEnemyInterface> ThisActor;
	FHitResult CursorHit;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditAnywhere,Category = "Input")
	TObjectPtr<UInputAction> ShiftAction;
	void ShiftKeyPressed(){bShiftKeyDown = true;};
	void ShiftKeyReleased(){bShiftKeyDown = false;};

	UPROPERTY(EditDefaultsOnly,Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetASC();

	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;

	bool bTargeting = false;
	bool bAutoRunning  = false;
	bool bShiftKeyDown = false;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	void AutoRun();
};


