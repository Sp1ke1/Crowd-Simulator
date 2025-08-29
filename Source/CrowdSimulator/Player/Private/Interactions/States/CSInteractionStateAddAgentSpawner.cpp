// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSimulator/Player/Public/Interactions/States/CSInteractionStateAddAgentSpawner.h"
#include "CrowdSimulator/AI/Public/CSAgentSpawner.h"
#include "CrowdSimulator/AI/Public/CSSubsystem.h"
#include "CrowdSimulator/Core/Public/CSCore.h"
#include "CrowdSimulator/Core/Public/CSUtilities.h"
#include "CrowdSimulator/Player/Public/CSPlayerPawn.h"
#include "CrowdSimulator/Player/Public/Interactions/Previews/CSAgentSpawnerPreview.h"
#include "CrowdSimulator/UI/Public/CSPlayerUIComponent.h"
#include "CrowdSimulator/Core/Public/CSUtilities.h"
#include "NavigationSystem.h"
#include "Components/BoxComponent.h"

void UCSInteractionStateAddAgentSpawner::Update(float dt)
{
	Super::Update(dt);

	UCSSubsystem * CSSubsystem = GetWorld() -> GetSubsystem<UCSSubsystem>();
	for ( const auto & Handle : PotentialNeighbours )
	{
		CSSubsystem -> GetNavigationPoint( Handle ) -> SetDefaultSprite(); 
	}

	if ( !IsValid ( AgentSpawnerPreview ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "InteractionStateAddAgentSpawner. Can't update. Agent spawner is invalid" ) );
		return; 
	}

	const UCSPlayerUIComponent * UIComponent = UCSUtilities::GetPlayerPawn( this ) -> GetUIComponent();
	if ( !IsValid ( UIComponent ) )
	{
		UE_LOG ( LogCrowdSimulator, Warning, TEXT ( "InteractionStateAddAgentSpawner::OnInteraction() => Error. Can't get player UI component" ) );
		return; 
	}

	FHitResult PreviewTrace;
	const FVector TraceStart = GetLastHitResultUnderCursor().ImpactPoint + FVector { 0.0f, 0.0f, 15.0f };
	const FVector BoxExtent = { UIComponent -> GetAgentSpawnerParameters().SpawnerSize.X,  UIComponent -> GetAgentSpawnerParameters().SpawnerSize.Y, 5.0f };
	FCollisionObjectQueryParams ObjectQueryParams;
	for ( auto TraceChannel : TraceChannels )
	{
		ObjectQueryParams.AddObjectTypesToQuery( TraceChannel ); 
	}
	const bool Colliding = GetWorld() -> SweepSingleByObjectType( PreviewTrace, TraceStart, TraceStart, FQuat::Identity, ObjectQueryParams, FCollisionShape::MakeBox( BoxExtent ) );


	// Update preview location 
	AgentSpawnerPreview -> SetActorLocation( GetLastHitResultUnderCursor().ImpactPoint );
	AgentSpawnerPreview -> GetBoxComponent() -> SetBoxExtent( BoxExtent ); 
	FNavLocation OutLocation;
	
	// Project point onto navigation to determine if it's valid to spawn nav point there 
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld() );
	const bool Projected = NavSys -> ProjectPointToNavigation( AgentSpawnerPreview -> GetActorLocation(), OutLocation );
	const bool Available = !Colliding && Projected; 
	AgentSpawnerPreview -> SetIsAvailable( Available );
	if ( Available )
	{
		CSSubsystem -> GetNavigationPointsInRadius( OutLocation.Location, UIComponent -> GetAgentSpawnerSnappingRadius(), PotentialNeighbours );
		for ( const auto & Handle : PotentialNeighbours )
		{
			CSSubsystem -> GetNavigationPoint( Handle ) -> SetSprite( ECSNavigationPointSpriteType::SelectSprite );
		}
		AgentSpawnerPreview -> SetHighlight( ECSNavigationPointSpriteType::SelectSprite ); 
	}
	else
	{
		AgentSpawnerPreview -> SetHighlight( ECSNavigationPointSpriteType::DestroySprite ); 
	}
}

void UCSInteractionStateAddAgentSpawner::OnInteraction()
{
	Super::OnInteraction();

	if ( !IsValid ( AgentSpawnerPreview ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "InteractionStateAddAgentSpawner. Can't interact. Agent spawner is invalid" ) );
		return;
	}

	if ( ! AgentSpawnerPreview -> GetIsAvailable() )
		return;

	// Spawn navigation point on preview position 
	const FVector AgentSpawnerSpawnLocation = ( GetLastHitResultUnderCursor() . ImpactPoint + FVector (  0.f, 0.f, 5.f ) );
	FActorSpawnParameters AgentSpawnerSpawnParameters;
	AgentSpawnerSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; 
	const FTransform AgentSpawnerSpawnTransform ( AgentSpawnerSpawnLocation );
	ACSAgentSpawner * AgentSpawner = GetWorld() -> SpawnActorDeferred <ACSAgentSpawner> ( AgentSpawnerClass, AgentSpawnerSpawnTransform );
	if ( !IsValid ( AgentSpawner ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "InteractionStateAddAgentSpawner::OnInteraction() => Error. Can't spawn agent spawner" ) );
		return; 
	}
	const UCSPlayerUIComponent * UIComponent = UCSUtilities::GetPlayerPawn( this ) -> GetUIComponent();
	if ( !IsValid ( UIComponent ) )
	{
		UE_LOG ( LogCrowdSimulator, Warning, TEXT ( "InteractionStateAddNavigationPoint::OnInteraction() => Error. Can't get player UI component" ) );
		return; 
	}
	
	const FCSAgentSpawnerParameters AgentSpawnerParameters = UIComponent -> GetAgentSpawnerParameters();
	AgentSpawner -> SetupSpawnerFromParameters( AgentSpawnerParameters );
	AgentSpawner -> FinishSpawning( AgentSpawnerSpawnTransform );


	UCSSubsystem * CSSubsystem = GetWorld() -> GetSubsystem<UCSSubsystem>();
	for ( const auto NavPoint : PotentialNeighbours )
	{
		CSSubsystem -> AddNavigationPath( AgentSpawner -> GetHandle(), NavPoint ); 
	}
}

void UCSInteractionStateAddAgentSpawner::OnDestroy()
{
	Super::OnDestroy();
	UCSSubsystem * CSSubsystem = GetWorld() -> GetSubsystem<UCSSubsystem>();
	for ( const auto & Handle : PotentialNeighbours )
	{
		CSSubsystem -> GetNavigationPoint( Handle ) -> SetDefaultSprite(); 
	}
	if ( AgentSpawnerPreview )
	{
		AgentSpawnerPreview -> Destroy(); 
	}
}

void UCSInteractionStateAddAgentSpawner::Initialize()
{
	Super::Initialize();
	AgentSpawnerPreview = GetWorld() -> SpawnActor <ACSAgentSpawnerPreview> ( AgentSpawnerPreviewClass );
	if ( !IsValid ( AgentSpawnerPreview ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "InteractionStateAddAgentSpawner::Initialize => Can't spawn agent spawner preview" ) );
		return; 
	}
}
