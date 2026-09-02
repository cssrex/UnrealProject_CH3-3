#pragma once

#include "CoreMinimal.h"
#include "Item/ItemSpawnRow.h"
#include "GameFramework/Actor.h"
#include "SpawnManager.generated.h"

class UBoxComponent;

UCLASS()
class VAMPIRESURVIVORS_API ASpawnManager : public AActor
{
	GENERATED_BODY()
	
public:
	ASpawnManager();

	AActor* SpawnRandomItem();
	AActor* SpawnExplosionZone();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	UBoxComponent* SpawningBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	UDataTable* ItemDataTable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> ExplosionZoneClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	float SpawnZOffset = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (ClampMin = "0.0"))
	float TraceLength = 1000.0f;

private:
	FItemSpawnRow* GetRandomItem() const;
	AActor* SpawnItem(TSubclassOf<AActor> ItemClass);
	bool GetRandomPointInVolume(FVector& OutSpawnLocation) const;
};
