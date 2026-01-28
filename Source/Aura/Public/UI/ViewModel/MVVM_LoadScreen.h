// Copyright Ziger

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadSlot.h"
#include "MVVM_LoadScreen.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectedSlot);
/*
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:
	void InitialLoadSlotViewModel();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot> LoadSlotViewModelClass;

	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot* GetLoadSlotViewModel(int32 Index) const;

	//在HUD中调用
	void LoadData();
	
	UFUNCTION(BlueprintCallable)
	void SelectButtonPressed(int32 Index);

	UFUNCTION(BlueprintCallable)
	void DeleteSlot();

	UFUNCTION(BlueprintCallable)
	void PlayButtonPressed();
	
	UPROPERTY(BlueprintAssignable)
	FSelectedSlot SelectedSlotDelegate;

	int32 SelectedSlotIndex = -1;
	
	void SetLoadScreenName(FString InName);
	
	FString GetLoadScreenName() const {return LoadScreenName;}


private:

	UPROPERTY()
	TArray<UMVVM_LoadSlot*> LoadSlotViewModels;
	
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlotViewModel_0;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlotViewModel_1;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlotViewModel_2;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,FieldNotify,Setter,Getter,meta=(AllowPrivateAccess="true"))
	FString LoadScreenName = "LoadScreen";
};
