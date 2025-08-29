// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CrowdSimulator/AI/Public/CSTypes.h"
#include "CSSaveGame.generated.h"



USTRUCT ( BlueprintType )
struct FCSSaveGameContainer
{

	GENERATED_BODY()
	UPROPERTY( EditAnywhere, BlueprintReadWrite, SaveGame )
	TArray<FCSAgentParameters> AgentParameters;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, SaveGame )
	TArray<FCSAgentSpawnerParameters> AgentSpawnerParameters;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, SaveGame )
	TArray<FCSNavigationPointParameters> NavigationPointsParameters;
};

/**
 * 
 */
UCLASS()
class CROWDSIMULATOR_API UCSSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UFUNCTION( BlueprintCallable )
	void SetLevelData  ( const FCSSaveGameContainer & LevelData );

	UFUNCTION( BlueprintCallable, BlueprintPure )
	const FCSSaveGameContainer & GetLevelData () const; 
	
private:
	
	
	UPROPERTY( SaveGame, VisibleAnywhere )
	FCSSaveGameContainer SaveGameContainer; 
};
