// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CSSettings.generated.h"

class ACSAgent;
class ACSAgentSpawner;
class ACSNavigationPoint;
class ACSNavigationSystem;

/**
 * 
 */
UCLASS( config = Game, DefaultConfig, meta = (DisplayName = "Crowd Simulator" ) )
class CROWDSIMULATOR_API UCSSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	
	UFUNCTION( BlueprintCallable ) 
	TSubclassOf<ACSNavigationSystem> GetNavigationSystemClass () const;

	UFUNCTION( BlueprintCallable )
	TSubclassOf<ACSAgent> GetAgentClass() const;

	UFUNCTION( BlueprintCallable )
	TSubclassOf<ACSNavigationPoint> GetNavigationPointClass () const;

	UFUNCTION( BlueprintCallable )
	TSubclassOf<ACSAgentSpawner> GetAgentSpawnerClass () const;
	

	UFUNCTION( BlueprintCallable )
	float GetMaxDensityFactor () const;

	UFUNCTION( BlueprintCallable )
	float GetMaxRandomFactor() const;

protected:
	UPROPERTY( Config, EditAnywhere, Category = "Navigation" )
	TSubclassOf<ACSNavigationSystem> NavigationSystemClass;

	UPROPERTY( Config, EditAnywhere, Category = "Navigation" )
	TSubclassOf<ACSAgentSpawner> AgentSpawnerClass;
	
	UPROPERTY( Config, EditAnywhere, Category = "Navigation" )
	TSubclassOf<ACSNavigationPoint> NavigationPointClass;

	UPROPERTY( Config, EditAnywhere, Category = "Navigation" )
	TSubclassOf<ACSAgent> AgentClass;
	
	UPROPERTY( Config, EditAnywhere, Category = "Naivgation" )
	float MaxDensityFactor = 50.f;
	
	UPROPERTY( Config, EditAnywhere, Category = "Naivgation" )
	float MaxRandomFactor = 200.f; 
};
