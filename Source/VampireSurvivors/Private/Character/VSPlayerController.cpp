#include "Character/VSPlayerController.h"
#include "Character/VSCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Core/VSGameInstance.h"
#include "Core/VSGameState.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ProgressBar.h"
#include "Item/DebuffInfo.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AVSPlayerController::AVSPlayerController()
	: InputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
	SprintAction(nullptr),
	HUDWidgetClass(nullptr),
	HUDWidgetInstance(nullptr),
	MainMenuWidgetClass(nullptr),
	MainMenuWidgetInstance(nullptr)
{
}

void AVSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("L_MainMenu"))
	{
		ShowMainMenu(false);
	}
}

UUserWidget* AVSPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

UUserWidget* AVSPlayerController::GetMainMenuWidget() const
{
	return MainMenuWidgetInstance;
}

void AVSPlayerController::UpdateBlindEffect(int32 BlindCount)
{
	if (!HUDWidgetInstance)
	{
		return;
	}

	UImage* BlindImage = Cast<UImage>(HUDWidgetInstance->GetWidgetFromName(TEXT("BlindImage")));
	if (BlindImage)
	{
		if (BlindCount <= 0)
		{
			BlindImage->SetVisibility(ESlateVisibility::Hidden);
			return;
		}
		else
		{
			BlindImage->SetVisibility(ESlateVisibility::HitTestInvisible);

			const float BlindOpacity = FMath::Clamp(0.4f + (BlindCount - 1) * 0.3f, 0.4f, 1.0f);
			BlindImage->SetRenderOpacity(BlindOpacity);
		}
	}
}

void AVSPlayerController::ShowGameHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}

		AVSGameState* VSGameState = GetWorld() ? GetWorld()->GetGameState<AVSGameState>() : nullptr;
		if (VSGameState)
		{
			VSGameState->UpdateHUD();
		}
	}
}

void AVSPlayerController::ShowMainMenu(bool bIsRestart)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();
			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}

		if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
		{
			if (bIsRestart)
			{
				ButtonText->SetText(FText::FromString(TEXT("재시작")));
			}
			else
			{
				ButtonText->SetText(FText::FromString(TEXT("시작하기")));
			}
		}

		if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("EndButtonText"))))
		{
			if (bIsRestart)
			{
				ButtonText->SetText(FText::FromString(TEXT("메인메뉴로")));
			}
			else
			{
				ButtonText->SetText(FText::FromString(TEXT("종료")));
			}
		}

		if (bIsRestart)
		{
			UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
			if (PlayAnimFunc)
			{
				MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
			}

			if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("TotalScoreText")))
			{
				if (UVSGameInstance* VSGameInstance = Cast<UVSGameInstance>(UGameplayStatics::GetGameInstance(this)))
				{
					TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Total Score: %d"), VSGameInstance->GetTotalScore())));
				}
			}
		}
	}
}

void AVSPlayerController::StartGame()
{
	if (UVSGameInstance* VSGameInstance = Cast<UVSGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		VSGameInstance->SetCurrentLevelIndex(0);
		VSGameInstance->SetTotalScore(0);
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("L_Basic"));
	SetPause(false);
}

void AVSPlayerController::EndGame()
{
	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("L_MainMenu"))
	{
		UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
	}
	else
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName("L_MainMenu"));
	}
}

void AVSPlayerController::UpdateDebuffUI()
{
	if (!HUDWidgetInstance)
	{
		return;
	}

	UHorizontalBox* DebuffBox = Cast<UHorizontalBox>(HUDWidgetInstance->GetWidgetFromName(TEXT("DebuffHorizontalBox")));

	if (!DebuffBox)
	{
		return;
	}

	AVSCharacter* PlayerCharacter = Cast<AVSCharacter>(GetPawn());

	if (!PlayerCharacter)
	{
		DebuffBox->ClearChildren();
		return;
	}

	const TArray<FDebuffInfo>& Debuffs = PlayerCharacter->GetActiveDebuffs();

	while (DebuffBox->GetChildrenCount() > Debuffs.Num())
	{
		DebuffBox->RemoveChildAt(DebuffBox->GetChildrenCount() - 1);
	}

	while (DebuffBox->GetChildrenCount() < Debuffs.Num())
	{
		if (!DebuffIconWidgetClass)
		{
			return;
		}

		UUserWidget* NewIconWidget = CreateWidget<UUserWidget>(this, DebuffIconWidgetClass);

		if (!NewIconWidget)
		{
			return;
		}

		UHorizontalBoxSlot* IconSlot = DebuffBox->AddChildToHorizontalBox(NewIconWidget);

		if (!IconSlot)
		{
			return;
		}

		IconSlot->SetPadding(FMargin(4.0f, 0.0f));
		IconSlot->SetHorizontalAlignment(HAlign_Center);
		IconSlot->SetVerticalAlignment(VAlign_Center);
	}

	for (int32 i = 0; i < Debuffs.Num(); ++i)
	{
		UUserWidget* IconWidget = Cast<UUserWidget>(DebuffBox->GetChildAt(i));

		if (!IconWidget)
		{
			continue;
		}

		const FDebuffInfo& Debuff = Debuffs[i];

		if (UImage* IconImage = Cast<UImage>(IconWidget->GetWidgetFromName(TEXT("DebuffIcon"))))
		{
			IconImage->SetBrushFromTexture(Debuff.Icon);
		}

		const float RemainingRatio = Debuff.Duration > 0.0f ? FMath::Clamp(Debuff.RemainingTime / Debuff.Duration, 0.0f, 1.0f) : 0.0f;

		if (UProgressBar* DurationBar = Cast<UProgressBar>(IconWidget->GetWidgetFromName(TEXT("DurationProgressBar"))))
		{
			DurationBar->SetPercent(RemainingRatio);
		}

		if (UTextBlock* TimeText = Cast<UTextBlock>(IconWidget->GetWidgetFromName(TEXT("RemainingTimeText"))))
		{
			const int32 Seconds = FMath::CeilToInt(FMath::Max(Debuff.RemainingTime, 0.0f));

			TimeText->SetText(FText::AsNumber(Seconds));
		}
	}
}
