// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSGameMode.h"
#include "TPSPlayerController.h"
#include "TPSCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATPSGameMode::ATPSGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATPSPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Character/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}