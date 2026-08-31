#include "Core/VSGameMode.h"
#include "Character/VSCharacter.h"
#include "Character/VSPlayerController.h"
#include "Core/VSGameState.h"

AVSGameMode::AVSGameMode()
{
	DefaultPawnClass = AVSCharacter::StaticClass();
	PlayerControllerClass = AVSPlayerController::StaticClass();
	GameStateClass = AVSGameState::StaticClass();
}
