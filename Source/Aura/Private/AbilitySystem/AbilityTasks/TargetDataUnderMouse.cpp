// Copyright Ziger


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"
#include "Player/AuraPlayerController.h"

class AAuraPlayerController;

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* TargetData = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return TargetData;
}

void UTargetDataUnderMouse::Activate()
{
	 if (Ability->GetCurrentActorInfo()->IsLocallyControlled())
	 {//客户端
	 	SendMouseHitData();
	 }else
	 {//服务端
	 	const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
	 	const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
	 	AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this,&UTargetDataUnderMouse::OnTargetDataReplicatedCallBack);//Unknown
	 	const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);//Unknown
	 	if (bCalledDelegate)
	 	{
	 		SetWaitingOnRemotePlayerData();//Unknown
	 	}
	 }
}

void UTargetDataUnderMouse::SendMouseHitData() const
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult Hit;
	PC->GetHitResultUnderCursor(ECC_Visibility,false,Hit);

	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = Hit;
	DataHandle.Add(Data);

	AbilitySystemComponent->ServerSetReplicatedTargetData(//向服务端发送数据
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	if (ShouldBroadcastAbilityTaskDelegates())//确保 Ability仍然 Active
	{
		ValidData.Broadcast(DataHandle);
	}
	
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallBack(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag GameplayTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(),GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

