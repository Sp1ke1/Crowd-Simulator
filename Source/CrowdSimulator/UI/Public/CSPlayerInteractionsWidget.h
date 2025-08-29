// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrowdSimulator/AI/Public/CSTypes.h"
#include "CSPlayerInteractionsWidget.generated.h"

class UCSInteractionState;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionModeChanged, TSubclassOf<UCSInteractionState>, InteractionState );
DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnSimulationSignature ); 

/**
 * 
 */
UCLASS()
class CROWDSIMULATOR_API UCSPlayerInteractionsWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	
public: 

	UFUNCTION( BlueprintNativeEvent )
	FCSAgentParameters GetAgentParameters () const;

	FCSAgentParameters GetAgentParameters_Implementation () const; 

	UFUNCTION( BlueprintNativeEvent )
	FCSNavigationPointParameters GetNavigationPointParameters () const;

	FCSNavigationPointParameters GetNavigationPointParameters_Implementation() const ;

	UFUNCTION( BlueprintNativeEvent )
	FCSAgentSpawnerParameters GetAgentSpawnerParameters () const;

	FCSAgentSpawnerParameters GetAgentSpawnerParameters_Implementation() const;

	UFUNCTION( BlueprintNativeEvent )
	float GetNavigationPointsSnappingRadius() const;

	float GetNavigationPointsSnappingRadius_Implementation() const;

	UFUNCTION( BlueprintNativeEvent ) 
	float GetAgentSpawnerSpanningRadius();

	float GetAgentSpawnerSpanningRadius_Implementation();

	UPROPERTY( BlueprintAssignable, BlueprintCallable  ) 
	FOnInteractionModeChanged OnInteractionModeSelectedDelegate;

	UPROPERTY( BlueprintAssignable, BlueprintCallable )
	FOnSimulationSignature OnSimulationStartedDelegate;

	UPROPERTY( BlueprintAssignable, BlueprintCallable )
	FOnSimulationSignature OnSimulationStoppedDelegate;

	UPROPERTY( BlueprintAssignable, BlueprintCallable )
	FOnSimulationSignature OnSimulationPausedDelegate;
};
