#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "VSGameState.generated.h"

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
	void OnGameOver();
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	float LevelDuration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;

public:
	AVSGameState();

	virtual void BeginPlay() override;

private:
	void StartLevel();
	void StartWave();
	void OnWaveTimeUp();
	void EndLevel();

private:
	FTimerHandle WaveTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;

	int32 CurWave;

	TArray<TWeakObjectPtr<AActor>> WaveItems;

};
