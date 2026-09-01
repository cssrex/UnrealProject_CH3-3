#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "VSGameState.generated.h"

class USoundBase;
class UAudioComponent;

USTRUCT(BlueprintType)
struct FWaveConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WaveDuration = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ItemToSpawn = 40;
};

UCLASS()
class VAMPIRESURVIVORS_API AVSGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	UFUNCTION()
	void OnGameOver(bool bIsClear);
	UFUNCTION(BlueprintCallable)
	void GoToNextLevel();
	void OnCoinCollected();
	void UpdateHUD();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 MaxWave;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave", meta = (EditFixedSize))
	TArray<FWaveConfig> WaveConfigs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 ExplosionDelay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TArray<TObjectPtr<USoundBase>> LevelBGMs;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> WaveStartSound = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> LevelClearSound = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> GameOverSound = nullptr;

public:
	AVSGameState();

	virtual void BeginPlay() override;

private:
	void StartLevel();
	void StartWave();
	void OnWaveTimeUp();
	void EndLevel();

	UFUNCTION()
	void SpawnExplosion();

private:
	FTimerHandle WaveTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;
	FTimerHandle ExplosionZoneTimerHandle;

	int32 CurWave;
	bool bLevelEnded = false;

	TArray<TWeakObjectPtr<AActor>> WaveItems;

	UPROPERTY()
	TObjectPtr<UAudioComponent> BGMComponent = nullptr;
};
