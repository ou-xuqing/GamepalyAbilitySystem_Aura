// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex,int32,WidgetSwitcherIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectButtonPressed,bool,Enable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeleteSlot);
UENUM()
enum ELoadSlotStates
{
	Vacant,
	EnterName,
	Taken
};


/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndex;

		
	UPROPERTY(BlueprintAssignable)
	FSelectButtonPressed SelectButtonPressedDelegate;

	UPROPERTY(BlueprintAssignable)
	FDeleteSlot DeleteSlotDelegate;
	
	void InitializeSlot();

	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed();

	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(FString Name);
	
	void SetLoadSlotName(const FString& InName);
	void SetPlayerName(const FString& InName);
	void SetMapName(const FString& InMapName);
	
	FString GetLoadSlotName()const {return LoadSlotName;}
	FString GetPlayerName()const {return PlayerName;}
	FString GetMapName()const {return MapName;}
	
	UPROPERTY()
	int32 LoadSlotIndex;

	UPROPERTY()
	TEnumAsByte<ELoadSlotStates> LoadSlotState = Vacant;

	UPROPERTY()
	FName PlayerStartTag;
	
private:
	//添加AllowPrivateAccess后蓝图可访问
	UPROPERTY(EditAnywhere,BlueprintReadWrite,FieldNotify,Setter,Getter,meta=(AllowPrivateAccess="true"))
	FString LoadSlotName;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,FieldNotify,Setter,Getter,meta=(AllowPrivateAccess="true"))
	FString PlayerName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,FieldNotify,Setter,Getter,meta=(AllowPrivateAccess="true"))
	FString MapName;
};
