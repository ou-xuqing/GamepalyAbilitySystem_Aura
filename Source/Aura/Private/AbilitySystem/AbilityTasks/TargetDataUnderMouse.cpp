// Copyright Ziger


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"
#include "Player/AuraPlayerController.h"

class AAuraPlayerController;
//AbilityTask模板
UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* TargetData = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);//new一个对象，然后执行Activate，所以具体逻辑在Activate中
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
	 	//先用Handle和APK绑定数据（Map），当携带该Key的数据到达时触发Callback。
	 	AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this,&UTargetDataUnderMouse::OnTargetDataReplicatedCallBack);//Unknown
	 	//当数据到达时但服务器还未绑定回调，调用此函数。使用缓存数据进行广播
	 	const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);//Unknown
	 	if (bCalledDelegate)//如果还失败，调用此函数告诉AbilityTask服务器还在等待数据
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
	//本函数主要传送鼠标碰撞，所以用这个类来保存数据,因为他里面也只有一个HitResult
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = Hit;
	DataHandle.Add(Data);//蓝图函数的的一个output

	AbilitySystemComponent->ServerSetReplicatedTargetData(//向服务器发送TargetData，Original == Activation
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	if (ShouldBroadcastAbilityTaskDelegates())//广播前先检测该能力是否还是激活状态
	{
		ValidData.Broadcast(DataHandle);
	}
	
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallBack(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag GameplayTag)
{
	//Handle和AP是一个key，TargetData是Value，通过Key找到value并删除他，因为客户端先实现，然后将数据给服务器，服务器用完后就可以删掉了
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(),GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);//服务器进行广播
	}
}

