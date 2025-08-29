// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSimulator/UI/Public/CSPlayerUIComponent.h"

#include "CrowdSimulator/Core/Public/CSCore.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UCSPlayerUIComponent::UCSPlayerUIComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true; 

	// ...
}


// Called when the game starts
void UCSPlayerUIComponent::BeginPlay()
{
	Super::BeginPlay();

	APlayerController * PlayerController = UGameplayStatics::GetPlayerController( this, 0 );
	if ( !IsValid ( PlayerController ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT( "PlayerUIComponent can't initialize. Invalid player controller" ) );
		return; 
	}
	// Create and initialize widget 
	InteractionsWidget = CreateWidget<UCSPlayerInteractionsWidget> ( PlayerController, InteractionsUIWidgetClass, "InteractionsUIWidget" );
	if ( !IsValid ( InteractionsWidget ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "PlayerUIComponent can't initialize. Can't create intractions widget " ) );
		return; 
	}
	// Bind all events to passthrough to player 
	InteractionsWidget -> AddToViewport();
	InteractionsWidget -> OnInteractionModeSelectedDelegate . AddDynamic ( this, &UCSPlayerUIComponent::OnInteractionModeChanged );
	InteractionsWidget -> OnSimulationStartedDelegate.AddDynamic( this, &UCSPlayerUIComponent::OnSimulationStarted );
	InteractionsWidget -> OnSimulationStoppedDelegate.AddDynamic( this, &UCSPlayerUIComponent::OnSimulationStopped);
	InteractionsWidget -> OnSimulationPausedDelegate.AddDynamic (this, &UCSPlayerUIComponent::OnSimulationPaused); 
}

void UCSPlayerUIComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	
}

void UCSPlayerUIComponent::OnInteractionModeChanged( TSubclassOf<UCSInteractionState> InteractionStateClass ) 
{
	OnInteractionModeSelectedDelegate . Broadcast( InteractionStateClass ); 
}

void UCSPlayerUIComponent::OnSimulationStarted()
{
	OnSimulationStartedDelegate.Broadcast(); 
}


void UCSPlayerUIComponent::OnSimulationStopped()
{
	OnSimulationStoppedDelegate.Broadcast(); 
}

void UCSPlayerUIComponent::OnSimulationPaused()
{
	OnSimulationPausedDelegate.Broadcast(); 
}

void UCSPlayerUIComponent::SetVisibility(bool Visibility)
{
	if ( IsValid ( InteractionsWidget ) )
		InteractionsWidget -> SetVisibility( Visibility ? ESlateVisibility::Visible : ESlateVisibility::Hidden );

}

// Called every frame
void UCSPlayerUIComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UCSPlayerInteractionsWidget* UCSPlayerUIComponent::GetInteractionsWidget() const
{
	return InteractionsWidget; 
}

float UCSPlayerUIComponent::GetNavigationPointsSnappingRadius() const
{
	return InteractionsWidget -> GetNavigationPointsSnappingRadius(); 
}

float UCSPlayerUIComponent::GetAgentSpawnerSnappingRadius() const
{
	return InteractionsWidget -> GetAgentSpawnerSpanningRadius();
}

FCSAgentParameters UCSPlayerUIComponent::GetAgentParameters() const
{
	return InteractionsWidget -> GetAgentParameters(); 
}

FCSNavigationPointParameters UCSPlayerUIComponent::GetNavigationPointParameters() const
{
	return InteractionsWidget -> GetNavigationPointParameters(); 
}

FCSAgentSpawnerParameters UCSPlayerUIComponent::GetAgentSpawnerParameters() const
{
	return InteractionsWidget -> GetAgentSpawnerParameters(); 
}

