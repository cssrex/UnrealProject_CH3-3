#include "Manager/SpawnManager.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

ASpawnManager::ASpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);

	ItemDataTable = nullptr;
}

bool ASpawnManager::GetRandomPointInVolume(FVector& OutSpawnLocation) const
{
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent(); // 스케일을 고려한 박스 크기의 절반을 가져옴
	FVector BoxOrigin = SpawningBox->GetComponentLocation(); // 박스의 중심 좌표

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	for (int32 i = 0; i < 30; i++)
	{
		const float RandomX = FMath::FRandRange(-BoxExtent.X, BoxExtent.X);
		const float RandomY = FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y);

		const FVector TraceStart(BoxOrigin.X + RandomX, BoxOrigin.Y + RandomY, BoxOrigin.Z + BoxExtent.Z);
		const FVector TraceEnd(TraceStart.X, TraceStart.Y, BoxOrigin.Z - BoxExtent.Z - TraceLength);

		FHitResult HitResult;

		const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

		if (bHit)
		{
			OutSpawnLocation = HitResult.ImpactPoint + FVector(0.0f, 0.0f, SpawnZOffset);
			return true;
		}
	}

	return false;
}

AActor* ASpawnManager::SpawnRandomItem()
{
	if (FItemSpawnRow* SelectedRow = GetRandomItem())
	{
		if (UClass* ActualClass = SelectedRow->ItemClass.Get()) // TSoftClassPtr 타입으로 가져와서 UClass로 반환됨
		{
			return SpawnItem(ActualClass);
		}
	}

	return nullptr;
}

AActor* ASpawnManager::SpawnExplosionZone()
{
	if (!ExplosionZoneClass) return nullptr;

	FVector SpawnLocation;

	if (!GetRandomPointInVolume(SpawnLocation))
	{
		return nullptr;
	}

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ExplosionZoneClass, SpawnLocation, FRotator::ZeroRotator);

	return SpawnedActor;
}

FItemSpawnRow* ASpawnManager::GetRandomItem() const
{
	if (!ItemDataTable) return nullptr;

	TArray<FItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("ItemSpawnContext")); // 디버깅시 추적용 string context 선언
	ItemDataTable->GetAllRows(ContextString, AllRows);

	if (AllRows.IsEmpty()) return nullptr;

	float TotalChance = 0.0f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->SpawnChance;
		}
	}

	const float RandValue = FMath::FRandRange(0.0f, TotalChance);
	float AccumulateChance = 0.0f;

	for (FItemSpawnRow* Row : AllRows)
	{
		AccumulateChance += Row->SpawnChance;
		if (RandValue <= AccumulateChance)
		{
			return Row;
		}
	}

	return nullptr;
}

AActor* ASpawnManager::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;

	FVector SpawnLocation;

	if (!GetRandomPointInVolume(SpawnLocation))
	{
		return nullptr;
	}

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ItemClass, SpawnLocation, FRotator::ZeroRotator);

	return SpawnedActor;
}

