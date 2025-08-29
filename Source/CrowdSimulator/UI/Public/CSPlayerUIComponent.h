// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CrowdSimulator/AI/Public/CSTypes.h"
#include "Components/ActorComponent.h"
#include "CSPlayerInteractionsWidget.h"
#include "CSPlayerUIComponent.generated.h"



UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CROWDSIMULATOR_API UCSPlayerUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCSPlayerUIComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

	/* Passthrough all events from widgets further to player */
	UFUNCTION () 
	void OnInteractionModeChanged( TSubclassOf<UCSInteractionState> InteractionStateClass );

	UFUNCTION()
	void OnSimulationStarted ();

	UFUNCTION()
	void OnSimulationStopped ();

	UFUNCTION()
	void OnSimulationPaused();

	UFUNCTION( BlueprintCallable )
	void SetVisibility( bool Visibility ); 

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION( BlueprintCallable )
	UCSPlayerInteractionsWidget * GetInteractionsWidget() const;
	
	UFUNCTION( BlueprintCallable )
	float GetNavigationPointsSnappingRadius () const;

	UFUNCTION( BlueprintCallable )
	float GetAgentSpawnerSnappingRadius() const; 
	
	UFUNCTION( BlueprintCallable )
	FCSAgentParameters GetAgentParameters () const;

	UFUNCTION( BlueprintCallable )
	FCSNavigationPointParameters GetNavigationPointParameters () const;


	UFUNCTION( BlueprintCallable )
	FCSAgentSpawnerParameters GetAgentSpawnerParameters () const; 
	
	
	UPROPERTY( BlueprintReadOnly, EditAnywhere, Category = "UI" ) 
	TSubclassOf<UCSPlayerInteractionsWidget> InteractionsUIWidgetClass;
	
	UPROPERTY()
	UCSPlayerInteractionsWidget * InteractionsWidget = nullptr;
	
	UPROPERTY( BlueprintAssignable )
	FOnInteractionModeChanged OnInteractionModeSelectedDelegate; 

	UPROPERTY( BlueprintAssignable )
	FOnSimulationSignature OnSimulationStartedDelegate;
	
	UPROPERTY( BlueprintAssignable )
	FOnSimulationSignature OnSimulationStoppedDelegate;
	
	UPROPERTY( BlueprintAssignable )
	FOnSimulationSignature OnSimulationPausedDelegate;
};
