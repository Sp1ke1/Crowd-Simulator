// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CSInteractionState.h"
#include "CSInteractionStateAddAgentSpawner.generated.h"

class UBoxComponent;
class ACSAgentSpawner;
class ACSAgentSpawnerPreview;
/**
 * 
 */
UCLASS()
class CROWDSIMULATOR_API UCSInteractionStateAddAgentSpawner : public UCSInteractionState
{
protected:
	GENERATED_BODY()

	virtual void Update(float dt) override;

	virtual void OnInteraction() override;

	virtual void OnDestroy() override;

	virtual void Initialize() override;

	UPROPERTY( EditAnywhere, BlueprintReadWrite ) 
	TSubclassOf <ACSAgentSpawner> AgentSpawnerClass;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TSubclassOf <ACSAgentSpawnerPreview> AgentSpawnerPreviewClass;

	UPROPERTY()
	TArray<int32> PotentialNeighbours; 

	UPROPERTY()
	ACSAgentSpawnerPreview * AgentSpawnerPreview; 
};
