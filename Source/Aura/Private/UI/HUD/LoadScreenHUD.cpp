// Copyright Ziger


#include "UI/HUD/LoadScreenHUD.h"

#include "UI/ViewModel/MVVM_LoadScreen.h"
#include "UI/Widget/LoadScreenWidget.h"

void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	LoadScreenViewModel = NewObject<UMVVM_LoadScreen>(this,LoadScreenViewModelClass);
	LoadScreenViewModel->InitialLoadSlotViewModel();
	
	LoadScreen = CreateWidget<ULoadScreenWidget>(GetWorld(),LoadScreenClass);
	LoadScreen->AddToViewport();
	LoadScreen->BlueprintInitializeWidget();

	LoadScreenViewModel->LoadData();
}
