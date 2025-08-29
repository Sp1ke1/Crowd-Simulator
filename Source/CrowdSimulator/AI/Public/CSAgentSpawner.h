// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSTypes.h"
#include "CSNavigationPoint.h"
#include "CSAgentSpawner.generated.h"

class UBoxComponent;
class ACSAgent; 

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnAgentSpawned, ACSAgent *, Agent ); 

UCLASS()
class CROWDSIMULATOR_API ACSAgentSpawner : public ACSNavigationPoint
{
	GENERATED_BODY()
public:
	ACSAgentSpawner(); 

	FOnAgentSpawned OnAgentSpawnedDelegate; 
	
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION( BlueprintCallable )
	void SetSpawnRate ( float spawnRate );

	UFUNCTION( BlueprintCallable )
	void SetAgeDistribution ( const FAgentSpawnerAgeDistribution & NewDistribution );

	UFUNCTION( BlueprintCallable , BlueprintPure )
	const FAgentSpawnerAgeDistribution & GetAgeDistribution () const;  
	
	UFUNCTION( BlueprintCallable )
	void StartSpawning ();

	UFUNCTION( BlueprintCallable )
	void StopSpawning();

	UFUNCTION( BlueprintCallable )
	int32 GetAgentsGroupId() const;
	
	UFUNCTION( BlueprintCallable )
	void SetAgentsGroupId(int32 GroupId );

	UFUNCTION( BlueprintCallable ) 
	void SetupSpawnerFromParameters ( const FCSAgentSpawnerParameters & Parameters ); 

	UFUNCTION( BlueprintCallable )
	void SetSpawnerExtent ( FVector2D Extent );

	UFUNCTION( BlueprintCallable )
	FCSAgentParameters GenerateAgentParameters() const;

	UFUNCTION( BlueprintCallable, BlueprintPure )
	float GetSpawnRate() const ;

	UFUNCTION( BlueprintCallable )
	FCSAgentSpawnerParameters GetAgentSpawnerParameters();

protected: 

	virtual void BeginPlay() override;
	void SpawnAgent ();
	
	UPROPERTY( EditAnywhere )
	float SpawnRate = 2.0f;

	UPROPERTY( EditAnywhere ) 
	FAgentSpawnerAgeDistribution AgeDistribution;

	UPROPERTY( EditAnywhere )
	TSubclassOf <ACSAgent> AgentClassToSpawn; 

	UPROPERTY( EditAnywhere )
	int32 AgentsGroupId; 
	
	FTimerHandle SpawnTimerHandle;
};
