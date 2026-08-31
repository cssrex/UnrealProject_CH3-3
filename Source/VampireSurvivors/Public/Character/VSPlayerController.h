#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VSPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class VAMPIRESURVIVORS_API AVSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "HUD")
	UUserWidget* GetHUDWidget() const;

	UFUNCTION(BlueprintCallable)
	void ShowGameHUD();
	UFUNCTION(BlueprintCallable)
	void ShowMainMenu(bool bIsRestart);
	UFUNCTION(BlueprintCallable)
	void StartGame();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* SprintAction;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|HUD")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI|HUD")
	UUserWidget* HUDWidgetInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Menu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI|Menu")
	UUserWidget* MainMenuWidgetInstance;

public:
	AVSPlayerController();

protected:
	virtual void BeginPlay() override;


};
