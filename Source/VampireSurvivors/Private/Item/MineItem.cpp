#include "Item/MineItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AMineItem::AMineItem()
{
	ExplosionDelay = 5.0f;
	ExplosionRadius = 300.0f;
	ExplosionDamage = 30;
	ItemType = "Mine";
	bHasExploded = false;

	ExplosionArea = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionArea"));
	ExplosionArea->InitSphereRadius(ExplosionRadius);
	ExplosionArea->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExplosionArea->SetupAttachment(Scene);
}

void AMineItem::ActivateItem(AActor* Activator)
{
	if (bHasExploded) return;

	Super::ActivateItem(Activator);

	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AMineItem::Explode, ExplosionDelay, false);

	bHasExploded = true;
}

void AMineItem::Explode()
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

	DestroyItem();

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
