// Copyright Epic Games, Inc. All Rights Reserved.

#include "BA_ContainersGameMode.h"
#include "BA_ContainersCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABA_ContainersGameMode::ABA_ContainersGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
