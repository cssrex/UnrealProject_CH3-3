#include "Core/VSGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/SpawnManager.h"
#include "Item/CoinItem.h"
#include "Core/VSGameInstance.h"
#include "Character/VSPlayerController.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

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

	LevelBGMs.SetNum(MaxLevels);

	ExplosionDelay = 5.0f;

	CurWave = 1;
}

void AVSGameState::BeginPlay()
{
	Super::BeginPlay();

	const FString LevelName = UGameplayStatics::GetCurrentLevelName(this, true);

	if (LevelName == TEXT("L_MainMenu"))
	{
		return;
	}

	StartLevel();

	GetWorldTimerManager().SetTimer(HUDUpdateTimerHandle, this, &AVSGameState::UpdateHUD, 0.1f, true);
}

void AVSGameState::StartLevel()
{
	bLevelEnded = false;

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

	if (IsValid(BGMComponent))
	{
		BGMComponent->Stop();
	}

	BGMComponent = nullptr;

	if (LevelBGMs.IsValidIndex(CurrentLevelIndex) && LevelBGMs[CurrentLevelIndex])
	{
		BGMComponent = UGameplayStatics::SpawnSound2D(this, LevelBGMs[CurrentLevelIndex]);
	}

	StartWave();
}

void AVSGameState::StartWave()
{
	GetWorldTimerManager().ClearTimer(ExplosionZoneTimerHandle);
	
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

	if (CurrentLevelIndex + 1 >= 3)
	{
		SpawnExplosion();
		GetWorldTimerManager().SetTimer(ExplosionZoneTimerHandle, this, &AVSGameState::SpawnExplosion, ExplosionDelay, true);
	}

	GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &AVSGameState::OnWaveTimeUp, CurrentWaveConfig.WaveDuration, false);

	UpdateHUD();

	if (CurWave > 1 && WaveStartSound)
	{
		UGameplayStatics::PlaySound2D(this, WaveStartSound);
	}
}

void AVSGameState::OnWaveTimeUp()
{
	if (bLevelEnded)
	{
		return;
	}

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
	if (bLevelEnded)
	{
		return;
	}

	if (CurrentLevelIndex + 1 >= MaxLevels)
	{
		OnGameOver(true);
		return;
	}

	bLevelEnded = true;

	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	GetWorldTimerManager().ClearTimer(ExplosionZoneTimerHandle);
	GetWorldTimerManager().ClearTimer(HUDUpdateTimerHandle);

	if (IsValid(BGMComponent))
	{
		BGMComponent->Stop();
	}

	if (AVSPlayerController* PC = Cast<AVSPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PC->SetPause(true);
		if (LevelClearSound)
		{
			UGameplayStatics::PlaySound2D(this, LevelClearSound);
		}
		PC->ShowLevelClearMenu(CurrentLevelIndex + 1);
	}
}

void AVSGameState::SpawnExplosion()
{
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnManager::StaticClass(), FoundVolumes);

	if (FoundVolumes.Num() > 0)
	{
		ASpawnManager* SpawnVolume = Cast<ASpawnManager>(FoundVolumes[0]);
		if (SpawnVolume)
		{
			AActor* SpawnedActor = SpawnVolume->SpawnExplosionZone();
		}
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
			VSPlayerController->UpdateDebuffUI();

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
	if (bLevelEnded)
	{
		return;
	}

	bLevelEnded = true;

	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	GetWorldTimerManager().ClearTimer(ExplosionZoneTimerHandle);
	GetWorldTimerManager().ClearTimer(HUDUpdateTimerHandle);

	if (IsValid(BGMComponent))
	{
		BGMComponent->Stop();
	}

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AVSPlayerController* VSPlayerController = Cast<AVSPlayerController>(PlayerController))
		{
			VSPlayerController->SetPause(true);

			USoundBase* ResultSound = bIsClear ? LevelClearSound.Get() : GameOverSound.Get();

			if (ResultSound)
			{
				UGameplayStatics::PlaySound2D(this, ResultSound);
			}

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

void AVSGameState::GoToNextLevel()
{
	if (!bLevelEnded)
	{
		return;
	}

	const int32 NextLevelIndex = CurrentLevelIndex + 1;

	if (NextLevelIndex >= MaxLevels || !LevelMapNames.IsValidIndex(NextLevelIndex) || LevelMapNames[NextLevelIndex].IsNone())
	{
		return;
	}

	UVSGameInstance* GameInstance = Cast<UVSGameInstance>(GetGameInstance());

	if (!GameInstance)
	{
		return;
	}

	bLevelEnded = false;

	GameInstance->SetCurrentLevelIndex(NextLevelIndex);

	UGameplayStatics::SetGamePaused(this, false);
	UGameplayStatics::OpenLevel(this, LevelMapNames[NextLevelIndex]);
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