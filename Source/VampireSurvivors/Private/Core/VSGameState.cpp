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
	WaveConfigs.SetNum(MaxWave);
	WaveConfigs[0].WaveDuration = 30.0f;
	WaveConfigs[0].ItemToSpawn = 30;
	WaveConfigs[1].WaveDuration = 25.0f;
	WaveConfigs[1].ItemToSpawn = 40;
	WaveConfigs[2].WaveDuration = 20.0f;
	WaveConfigs[2].ItemToSpawn = 50;

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

	const int32 WaveIndex = CurWave - 1;

	if (!WaveConfigs.IsValidIndex(WaveIndex))
	{
		return;
	}
	const FWaveConfig& CurrentWaveConfig = WaveConfigs[WaveIndex];
	const int32 ItemToSpawn = CurrentWaveConfig.ItemToSpawn;

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

	GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &AVSGameState::OnWaveTimeUp, CurrentWaveConfig.WaveDuration, false);

	UpdateHUD();
}

void AVSGameState::OnWaveTimeUp()
{
	if (CurWave < MaxWave)
	{
		CurWave++;

		StartWave();
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
		OnGameOver(true);
		return;
	}

	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver(false);
	}
}

void AVSGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	UpdateHUD();

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
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level %d"), CurrentLevelIndex + 1)));
				}

				if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("WaveText"))))
				{
					WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave %d"), CurWave)));
				}

				if (UTextBlock* CoinText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("CoinText"))))
				{
					CoinText->SetText(FText::FromString(FString::Printf(TEXT("Coin: %d / %d"), CollectedCoinCount, SpawnedCoinCount)));
				}
			}
		}
	}
}

void AVSGameState::OnGameOver(bool bIsClear)
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AVSPlayerController* VSPlayerController = Cast<AVSPlayerController>(PlayerController))
		{
			VSPlayerController->SetPause(true);
			VSPlayerController->ShowMainMenu(true);

			if (UUserWidget* MainMenuWidget = VSPlayerController->GetMainMenuWidget())
			{
				if (UTextBlock* GameOverText = Cast<UTextBlock>(MainMenuWidget->GetWidgetFromName(TEXT("GameOverText"))))
				{
					if (bIsClear)
					{
						GameOverText->SetText(FText::FromString(TEXT("Game Clear!")));
						GameOverText->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
					}
					else
					{
						GameOverText->SetText(FText::FromString(TEXT("Game Over!")));
						GameOverText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
					}
					
				}
			}
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