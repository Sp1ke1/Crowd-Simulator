// Copyright Epic Games, Inc. All Rights Reserved.


#include "CrowdSimulator/Core/Public/CSGameMode.h"

#include "CrowdSimulator/AI/Public/CSSubsystem.h"
#include "CrowdSimulator/Core/Public/CSCore.h"
#include "CrowdSimulator/Core/Public/CSGameInstance.h"
#include "CrowdSimulator/Core/Public/CSSaveGame.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

ACSGameMode::ACSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/CrowdSimulator/Player/PlayerPawn/BP_CS_PlayerPawn"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ACSGameMode::TryLoadSaveGame() const
{
	const UCSGameInstance * GameInstance = Cast<UCSGameInstance> ( UGameplayStatics::GetGameInstance( this ) );
	if ( ! GameInstance )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT("ACSGameMode::TryLoadSaveGame() => Error. Game Instnace class doesn't derive from UCSGAmeInstance." ) );
		return; 
	}
	const FCSSaveSlotData SaveSlotData = GameInstance -> GetLevelSlotData();
	const UCSSaveGame * SaveGame = Cast<UCSSaveGame> ( UGameplayStatics::LoadGameFromSlot( SaveSlotData.SlotName, SaveSlotData.SlotIndex ) );
	if ( !SaveGame )
	{
		UE_LOG ( LogCrowdSimulator, Log, TEXT("ACSGameMode::TryLoadSaveGame() => Didn't find any save data for current level." ) );
		return;
	}
	UCSSubsystem * CSSubsystem = GetWorld() -> GetSubsystem<UCSSubsystem>();
	if ( !CSSubsystem )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "ACSGameMode::TryLoadSaveGame() => Error. CSSubsystem is invalid. Can't load save. ") );
		return; 
	}
	CSSubsystem -> LoadLevelFromSaveData ( SaveGame -> GetLevelData() ); 
}

void ACSGameMode::BeginPlay()
{
	Super::BeginPlay();
	APlayerController * PlayerController = UGameplayStatics::GetPlayerController( this, 0 );
	if ( !IsValid ( PlayerController ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "ACSGameMode::BeginPlay () -> Invalid player controller" ) );
		return; 
	}

	const FInputModeGameOnly InputModeGameOnly; 
	PlayerController -> SetShowMouseCursor( true );
	TryLoadSaveGame(); 
}
