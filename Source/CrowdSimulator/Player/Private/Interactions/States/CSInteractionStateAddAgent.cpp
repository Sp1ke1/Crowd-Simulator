
#include "CrowdSimulator/Player/Public/Interactions/States/CSInteractionStateAddAgent.h"
#include "CrowdSimulator/Player/Public/Interactions/Previews/CSAgentPreview.h"
#include "CrowdSimulator/Core/Public/CSCore.h"
#include "CrowdSimulator/AI/Public/CSAgent.h"
#include "CrowdSimulator/Core/Public/CSUtilities.h"
#include "CrowdSimulator/Player/Public/CSPlayerPawn.h"
#include "CrowdSimulator/UI/Public/CSPlayerUIComponent.h"

#include "Components/CapsuleComponent.h"
#include "NavigationSystem.h"

void UCSInteractionStateAddAgent::Initialize()
{
	
	
}

void UCSInteractionStateAddAgent::Update(float dt)
{
	// Fetch data 
	Super::Update(dt);
	
	if ( !IsValid ( AgentPreview ) )
	{
		// Spawn preview if needed 
		FActorSpawnParameters PreviewSpawnParameters;
		FTransform PreviewSpawnTransform ( GetLastHitResultUnderCursor().ImpactPoint ); 
		PreviewSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AgentPreview = GetWorld() -> SpawnActor <ACSAgentPreview> ( AgentPreviewClass,  PreviewSpawnTransform, PreviewSpawnParameters );
	}

	if ( !IsValid ( AgentPreview ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "InteractionStateAddAgent. Can't update. Can't create agent preview" ) );
		return; 
	}

	// Update preview location 
	const float CapsuleHalfHeight = AgentPreview -> GetCapsuleComponent() -> GetScaledCapsuleHalfHeight();
	const float CapsuleRadius = AgentPreview -> GetCapsuleComponent() -> GetScaledCapsuleRadius(); 
	const FVector TraceStart = AgentPreview -> GetActorLocation() + FVector { 0.f, 0.f, 5.f };  

	// Update preview availability state 
	FHitResult PreviewTrace;

	FNavLocation LocationProjectedOnNavigation;
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld() );
	const bool IsProjectedOnNavigation = NavSys -> ProjectPointToNavigation( GetLastHitResultUnderCursor().ImpactPoint, LocationProjectedOnNavigation );
	
	AgentPreview -> SetActorLocation( IsProjectedOnNavigation ? LocationProjectedOnNavigation.Location : GetLastHitResultUnderCursor().ImpactPoint ); 
	AgentPreview -> AddActorWorldOffset( { 0.f, 0.f, CapsuleHalfHeight } );
	FCollisionObjectQueryParams ObjectQueryParams;
	for ( auto TraceChannel : TraceChannels )
	{
		ObjectQueryParams.AddObjectTypesToQuery( TraceChannel ); 
	}
	bool Colliding = GetWorld() -> SweepSingleByObjectType( PreviewTrace, TraceStart, TraceStart, FQuat::Identity, ObjectQueryParams, FCollisionShape::MakeCapsule( CapsuleRadius, CapsuleHalfHeight ) );
	AgentPreview -> SetIsAvailable( !Colliding && IsProjectedOnNavigation );
}

void UCSInteractionStateAddAgent::OnInteraction()
{
	if ( !IsValid ( AgentPreview ) || ! AgentPreview -> GetIsAvailable() )
		return; 

	// Spawn agent on preview position 
	FVector AgentSpawnLocation = ( GetLastHitResultUnderCursor() . ImpactPoint );
	FActorSpawnParameters AgentSpawnParameters;
	AgentSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; 
	AgentSpawnLocation.Z += AgentPreview -> GetCapsuleComponent() -> GetScaledCapsuleHalfHeight();
	const FTransform AgentSpawnTransform ( AgentSpawnLocation );
	ACSAgent * SpawnedAgent = GetWorld() -> SpawnActorDeferred <ACSAgent> ( AgentClass, AgentSpawnTransform );
	if ( !IsValid ( SpawnedAgent ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "InteractionStateAddAgent::OnInteraction() => Error spawning agent" ) );
		return; 
	}
	SpawnedAgent -> SpawnDefaultController();
	const auto UIComponent = UCSUtilities::GetPlayerPawn( this ) -> GetUIComponent();
	if ( !IsValid ( UIComponent ) )
	{
		UE_LOG ( LogCrowdSimulator, Warning, TEXT ( "InteractionStateAddAgent::OnInteraction => Can't get UI component from player" ) );
		return; 
	}
	
	SpawnedAgent -> SetupFromParameters ( UIComponent -> GetAgentParameters() );
	SpawnedAgent -> FinishSpawning( AgentSpawnTransform, true ); 
}

void UCSInteractionStateAddAgent::OnDestroy()
{
	if ( IsValid ( AgentPreview ) )
		AgentPreview -> Destroy(); 
}
