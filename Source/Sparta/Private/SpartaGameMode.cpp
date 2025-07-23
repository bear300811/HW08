#include "SpartaGameMode.h"
#include "SpartaCharacter.h"
#include "MyPlayerController.h"
#include "SpartaGameState.h"

ASpartaGameMode::ASpartaGameMode()
{
	DefaultPawnClass = ASpartaCharacter::StaticClass();
	PlayerControllerClass = AMyPlayerController::StaticClass();
	GameStateClass = ASpartaGameState::StaticClass();
}
