#include "Core/VSGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/SpawnManager.h"
#include "Item/CoinItem.h"
#include "Core/VSGameInstance.h"
#include "Character/VSPlayerController.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

AVSGameState::AVSGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	MaxWave = 3;

	LevelDuration = 30.0f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;

	CurWave = 1;
}

void AVSGameState::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();

	GetWorldTimerManager().SetTimer(HUDUpdateTimerHandle, this, &AVSGameState::UpdateHUD, 0.1f, true);
}

void AVSGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AVSPlayerController* VSPlayerController = Cast<AVSPlayerController>(PlayerController))
		{
			VSPlayerController->ShowGameHUD();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UVSGameInstance* VSGameInstance = Cast<UVSGameInstance>(GameInstance);
		if (VSGameInstance)
		{
			CurrentLevelIndex = VSGameInstance->GetCurrentLevelIndex();
		}
	}

	StartWave();
}

void AVSGameState::StartWave()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, FString::Printf(TEXT("Wave %d 시작!"), CurWave));

	for (const TWeakObjectPtr<AActor>& WaveItem : WaveItems)
	{
		if (AActor* Item = WaveItem.Get())
		{
			Item->Destroy();
		}
	}

	WaveItems.Reset();
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnManager::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = 40;
	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnManager* SpawnVolume = Cast<ASpawnManager>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (IsValid(SpawnedActor))
				{
					WaveItems.Add(TWeakObjectPtr<AActor>(SpawnedActor));

					if (SpawnedActor->IsA(ACoinItem::StaticClass()))
					{
						SpawnedCoinCount++;
					}
				}
				
			}
		}
	}

	GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &AVSGameState::OnWaveTimeUp, LevelDuration, false);

	UpdateHUD();
}

void AVSGameState::OnWaveTimeUp()
{
	if (CurWave < MaxWave)
	{
		StartWave();

		CurWave++;
	}
	else
	{
		EndLevel();
	}
}

void AVSGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UVSGameInstance* VSGameInstance = Cast<UVSGameInstance>(GameInstance);
		if (VSGameInstance)
		{
			AddScore(Score);
			CurrentLevelIndex++;
			VSGameInstance->SetCurrentLevelIndex(CurrentLevelIndex);
		}
	}

	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}

	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

void AVSGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		OnWaveTimeUp();
	}
}

void AVSGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AVSPlayerController* VSPlayerController = Cast<AVSPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = VSPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("TimeText"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}

				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("ScoreText"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						UVSGameInstance* VSGameInstance = Cast<UVSGameInstance>(GameInstance);
						if (VSGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), VSGameInstance->GetTotalScore())));
						}
					}
				}

				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("LevelText"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
				}
			}
		}
	}
}

void AVSGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AVSPlayerController* VSPlayerController = Cast<AVSPlayerController>(PlayerController))
		{
			VSPlayerController->SetPause(true);
			VSPlayerController->ShowMainMenu(true);
		}
	}
}

int32 AVSGameState::GetScore() const
{
	return Score;
}

void AVSGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UVSGameInstance* VSGameInstance = Cast<UVSGameInstance>(GameInstance);
		if (VSGameInstance)
		{
			VSGameInstance->AddToScore(Amount);
		}
	}
}