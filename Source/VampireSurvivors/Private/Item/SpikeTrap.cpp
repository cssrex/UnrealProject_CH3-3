#include "Item/SpikeTrap.h"
#include "Components/SphereComponent.h"
#include "Character/VSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ASpikeTrap::ASpikeTrap()
{
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	FloorStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
	FloorStaticMesh->SetupAttachment(Scene);

	SpikeStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spike"));
	SpikeStaticMesh->SetCollisionProfileName(TEXT("NoCollision"));
	SpikeStaticMesh->SetupAttachment(Scene);
	
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(SpikeStaticMesh);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ASpikeTrap::OnItemOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ASpikeTrap::OnItemEndOverlap);

	TravelDistance = 100.0f;
	RiseDuration = 0.2f;
	RaisedWaitTime = 1.0f;
	LowerDuration = 0.2f;
	HiddenWaitTime = 3.0f;
	SpikeDamage = 30.0f;
}

void ASpikeTrap::BeginPlay()
{
	Super::BeginPlay();
	
	RaisedLocation = SpikeStaticMesh->GetRelativeLocation();
	HiddenLocation = RaisedLocation - FVector(0.0f, 0.0f, TravelDistance);

	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpikeStaticMesh->SetRelativeLocation(HiddenLocation);

	StartRising();
}

void ASpikeTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveElapsedTime += DeltaTime;

	const float Duration = bIsRising ? RiseDuration : LowerDuration;
	const float Alpha = Duration > 0.0f ? FMath::Clamp(MoveElapsedTime / Duration, 0.0f, 1.0f) : 1.0f;
	const float SmoothAlpha = Alpha * Alpha * (3.0f - 2.0f * Alpha);

	const FVector StartLocation = bIsRising ? HiddenLocation : RaisedLocation;
	const FVector TargetLocation = bIsRising ? RaisedLocation : HiddenLocation;

	SpikeStaticMesh->SetRelativeLocation(FMath::Lerp(StartLocation, TargetLocation, SmoothAlpha));

	if (Alpha < 1.0f)
	{
		return;
	}

	SpikeStaticMesh->SetRelativeLocation(TargetLocation);
	SetActorTickEnabled(false);

	if (bIsRising)
	{
		GetWorldTimerManager().SetTimer(RaiseTimerHandle, this, &ASpikeTrap::StartLowering, FMath::Max(RaisedWaitTime, 0.01f), false);
	}
	else
	{
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		GetWorldTimerManager().ClearTimer(AttackTimerHandle);
		OverlappedActor = nullptr;

		GetWorldTimerManager().SetTimer(HiddenTimerHandle, this, &ASpikeTrap::StartRising, FMath::Max(HiddenWaitTime, 0.01f), false);
	}
}

void ASpikeTrap::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && OtherActor->ActorHasTag("Player"))
	{
		OverlappedActor = OtherActor;
		GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &ASpikeTrap::AttackPlayer, 0.5f, true);
		AttackPlayer();
	}
}

void ASpikeTrap::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsValid(OtherActor) && OtherActor->ActorHasTag("Player"))
	{
		OverlappedActor = nullptr;
		GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	}
}

void ASpikeTrap::AttackPlayer()
{
	if (OverlappedActor && OverlappedActor->ActorHasTag("Player"))
	{
		UGameplayStatics::ApplyDamage(
			OverlappedActor,						// 대미지를 받을 액터
			SpikeDamage,				// 대미지의 양
			nullptr,					// 대미지를 유발한 주체의 플레이어 컨트롤러
			this,						// 대미지를 입힌 액터
			UDamageType::StaticClass()  // 대미지 유형 (여기서는 기본을 넘김)
		);
	}
}

void ASpikeTrap::StartRising()
{
	if (ActiveSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ActiveSound, GetActorLocation());
	}

	bIsRising = true;
	MoveElapsedTime = 0.0f;

	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	SetActorTickEnabled(true);
}

void ASpikeTrap::StartLowering()
{
	bIsRising = false;
	MoveElapsedTime = 0.0f;

	SetActorTickEnabled(true);
}

