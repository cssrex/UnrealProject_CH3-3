#include "Item/ExplosionZone.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AExplosionZone::AExplosionZone()
{
	PrimaryActorTick.bCanEverTick = false;

	ExplosionDelay = 5.0f;
	ExplosionRadius = 300.0f;
	ExplosionDamage = 30;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	ExplosionArea = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionArea"));
	ExplosionArea->InitSphereRadius(ExplosionRadius);
	ExplosionArea->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExplosionArea->SetupAttachment(Scene);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMesh->SetupAttachment(Scene);

}

void AExplosionZone::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AExplosionZone::Explode, ExplosionDelay, false);
}

void AExplosionZone::OnItemOverlap(
	UPrimitiveComponent* OverlappedComp,	// 오버랩이 발생한 자기 자신 (Collision 같은거)
	AActor* OtherActor,						// 부딪힌 상대방 액터
	UPrimitiveComponent* OtherComp,			// 부딪힌 액터의 정확히 어떤 컴포넌트에 충돌했는지
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{

}
void AExplosionZone::OnItemEndOverlap(
	UPrimitiveComponent* OverlappedComp,	// 오버랩이 발생한 자기 자신 (Collision 같은거)
	AActor* OtherActor,						// 부딪힌 상대방 액터
	UPrimitiveComponent* OtherComp,			// 부딪힌 액터의 정확히 어떤 컴포넌트에 충돌했는지
	int32 OtherBodyIndex)
{

}

void AExplosionZone::Explode()
{
	UParticleSystemComponent* Particle = nullptr;

	if (ExplosionParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, GetActorLocation(), GetActorRotation(), false);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
	}


	TArray<AActor*> OverlappingActors;
	ExplosionArea->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(
				Actor,						// 대미지를 받을 액터
				ExplosionDamage,			// 대미지의 양
				nullptr,					// 대미지를 유발한 주체의 플레이어 컨트롤러
				this,						// 대미지를 입힌 액터
				UDamageType::StaticClass()  // 대미지 유형 (여기서는 기본을 넘김)
			);
		}
	}

	Destroy();

	if (Particle)
	{
		TWeakObjectPtr<UParticleSystemComponent> WeakParticle = Particle;

		FTimerHandle DestroyParticleTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(DestroyParticleTimerHandle,
			[WeakParticle]()
			{
				if (WeakParticle.IsValid())
				{
					WeakParticle->DestroyComponent();
				}
			},
			2.0f,
			false
		);
	}
}