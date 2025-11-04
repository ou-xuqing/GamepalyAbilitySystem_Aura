// Copyright Ziger


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;//不加这个服务器上的火球无法在客户端上显示，而且客户端本来就不生成火球

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

// Called when the game starts or when spawned
void AAuraProjectile::BeginPlay()
{
	SetLifeSpan(LifeSpan);
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
	if (LoopingSoundComponent) LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound,GetRootComponent());
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//DamageSpecHandle只会在服务器中设置所以要检查其有效性。后面和otherActor比较是为了防止火球打到自己
	if (DamageSpecHandle.Data.IsValid() && DamageSpecHandle.Data.Get()->GetEffectContext().GetEffectCauser()== OtherActor)
	{
		return;
	}
	//和教程不一致，但是可以修改火球刚出生就出现爆炸特效的bug，不懂(也就是说在客户端不进行碰撞检测，等到destroy时，发出爆炸声音和特效（可能）)
	if (DamageSpecHandle.Data == nullptr) return;
	if (!UAuraAbilitySystemLibrary::IsNotFriend(DamageSpecHandle.Data.Get()->GetEffectContext().GetEffectCauser(),OtherActor))
	{
		return;
	}
	UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation(),FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ImpactEffect,GetActorLocation());
	if (LoopingSoundComponent) LoopingSoundComponent->Stop();
	
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageSpecHandle.Data.Get());
		}
		
		Destroy();
	}else
	{
		bHit = true;
	}
}

void AAuraProjectile::Destroyed()
{
	//在客户端未发生碰撞但已经销毁则发出声音和特效
	if (!bHit&&!HasAuthority())
	{
		UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation(),FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ImpactEffect,GetActorLocation());
		if (LoopingSoundComponent) LoopingSoundComponent->Stop();
	}
	Super::Destroyed();
}

