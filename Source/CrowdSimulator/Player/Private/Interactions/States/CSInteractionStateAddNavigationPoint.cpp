// Fill out your copyright notice in the Description page of Project Settings.

#include "CrowdSimulator/Player/Public/Interactions/States/CSInteractionStateAddNavigationPoint.h"
#include "CrowdSimulator/AI/Public/CSSubsystem.h"
#include "CrowdSimulator/AI/Public/CSTypes.h"
#include "CrowdSimulator/Core/Public/CSCore.h"
#include "CrowdSimulator/Core/Public/CSUtilities.h"
#include "CrowdSimulator/Player/Public/CSPlayerPawn.h"
#include "CrowdSimulator/Player/Public/Interactions/Previews/CSNavigationPointPreview.h"
#include "CrowdSimulator/UI/Public/CSPlayerUIComponent.h"
#include "CrowdSimulator/AI/Public/CSNavigationPoint.h"

#include "NavigationSystem.h"

void UCSInteractionStateAddNavigationPoint::Update(float dt)
{
	Super::Update(dt);

	const UCSSubsystem * CSSubsystem = GetWorld() -> GetSubsystem<UCSSubsystem>();
	const UCSPlayerUIComponent * UIComponent = UCSUtilities::GetPlayerPawn( this ) -> GetUIComponent();
	
	for ( const auto & NavPoint : PotentialNeighbours )
	{
		const auto NavigationPoint = CSSubsystem -> GetNavigationPoint( NavPoint ); 
		NavigationPoint -> SetDefaultSprite();
	}
	PotentialNeighbours.Empty();
	
	if ( !IsValid ( NavigationPointPreview ) )
	{
		// Spawn preview if needed 
		FActorSpawnParameters PreviewSpawnParameters;
		const FTransform PreviewSpawnTransform ( GetLastHitResultUnderCursor().ImpactPoint ); 
		PreviewSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		NavigationPointPreview = GetWorld() -> SpawnActor <ACSNavigationPointPreview> ( NavigationPointPreviewClass, PreviewSpawnTransform, PreviewSpawnParameters );
	}

	if ( !IsValid ( NavigationPointPreview ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "InteractionStateAddAgent. Can't update. Can't create agent preview" ) );
		return; 
	}

	// Update preview location 
	NavigationPointPreview -> SetActorLocation( GetLastHitResultUnderCursor().ImpactPoint );
	FNavLocation OutLocation;

	
	// Project point onto navigation to determine if it's valid to spawn nav point there 
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld() );
	const bool Available = NavSys -> ProjectPointToNavigation( NavigationPointPreview -> GetActorLocation(), OutLocation );
	NavigationPointPreview -> SetIsAvailable( Available );
	if ( Available )
	{
		
		// Connect all points in radius automatically. 
		CSSubsystem -> GetNavigationPointsInRadius( OutLocation.Location, UIComponent -> GetNavigationPointsSnappingRadius(), PotentialNeighbours);
		NavigationPointPreview -> SetHighlight( ECSNavigationPointSpriteType::SelectSprite );
		for ( const auto & NavPoint : PotentialNeighbours )
		{
			auto NavigationPoint = CSSubsystem -> GetNavigationPoint( NavPoint ); 
			NavigationPoint -> SetSprite( ECSNavigationPointSpriteType::SelectSprite );
		}
	}
	else
	{
		NavigationPointPreview -> SetHighlight( ECSNavigationPointSpriteType::DestroySprite );
	}
}

void UCSInteractionStateAddNavigationPoint::OnInteraction()
{
	Super::OnInteraction();
	if ( !IsValid ( NavigationPointPreview ) || ! NavigationPointPreview -> GetIsAvailable() )
		return; 

	
	// Spawn navigation point on preview position 
	const FVector NavigationPointSpawnLocation = ( GetLastHitResultUnderCursor() . ImpactPoint );
	FActorSpawnParameters NavigationPointSpawnParameters;
	NavigationPointSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; 
	const FTransform NavigationPointSpawnTransform ( NavigationPointSpawnLocation );
	ACSNavigationPoint * SpawnedPoint = GetWorld() -> SpawnActorDeferred <ACSNavigationPoint> ( NavigationPointClass, NavigationPointSpawnTransform );
	if ( !IsValid ( SpawnedPoint ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "InteractionStateAddNavigationPoint::OnInteraction() => Error. Can't spawn navigation point" ) );
		return; 
	}
	const UCSPlayerUIComponent * UIComponent = UCSUtilities::GetPlayerPawn( this ) -> GetUIComponent();
	if ( !IsValid ( UIComponent ) )
	{
		UE_LOG ( LogCrowdSimulator, Warning, TEXT ( "InteractionStateAddNavigationPoint::OnInteraction() => Error. Can't get player UI component" ) );
		return; 
	}
	const auto NavigationPointParameters = UIComponent -> GetNavigationPointParameters(); 
	SpawnedPoint -> SetupPointFromParameters( NavigationPointParameters );
	SpawnedPoint -> FinishSpawning( NavigationPointSpawnTransform );
	UE_LOG ( LogCrowdSimulator, Log, TEXT ( "Spawned new navigation point. GroupId: %d"), UIComponent -> GetNavigationPointParameters().GoalGroupId );
	
	// Connect all points in radius automatically. 
	UCSSubsystem * CSSubsystem = GetWorld() -> GetSubsystem<UCSSubsystem>();
	ensure ( CSSubsystem );
	for ( const auto NavPoint : PotentialNeighbours )
	{
		CSSubsystem -> AddNavigationPath( SpawnedPoint -> GetHandle(), NavPoint); 
	}
}

void UCSInteractionStateAddNavigationPoint::OnDestroy()
{
	Super::OnDestroy();
	const UCSSubsystem * CSSubsystem = GetWorld() -> GetSubsystem<UCSSubsystem>();
	for ( const auto & Handle : PotentialNeighbours )
	{
		CSSubsystem -> GetNavigationPoint( Handle ) -> SetDefaultSprite(); 
	}
	if ( IsValid ( NavigationPointPreview ) )
		NavigationPointPreview -> Destroy(); 
}
