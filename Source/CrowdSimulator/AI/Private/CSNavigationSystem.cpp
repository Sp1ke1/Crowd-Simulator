// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSimulator/AI/Public/CSNavigationSystem.h"
#include "CrowdSimulator/AI/Public/CSNavigationPoint.h"
#include "CrowdSimulator/AI/Public/CSSubsystem.h"
#include "CrowdSimulator/Core/Public/CSUtilities.h"

// Sets default values
ACSNavigationSystem::ACSNavigationSystem()
	: NavigationGraph( this )
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LineBatchComponent = CreateDefaultSubobject<ULineBatchComponent>( "LineBatchComponent" );
}

// Called when the game starts or when spawned
void ACSNavigationSystem::BeginPlay()
{
	Super::BeginPlay();
}

bool ACSNavigationSystem::GetIsValidNavigationPointHandle(int32 NavPointHandle) const
{
	if ( IsValid ( GetWorld() ) )
	{
		if (const auto Subsystem = GetWorld() -> GetSubsystem<UCSSubsystem>() )
		{
			return Subsystem -> GetIsNavigationPointRegistered( NavPointHandle ); 
		}
	}
	return false;
}

void ACSNavigationSystem::DrawPath (const int NavPoint1, const int NavPoint2, const FColor & Color, float Lifetime, float Thickness, uint8 DepthPriority )
{
	if ( !GetIsPathExists( NavPoint1, NavPoint2 ) )
		return;
	
	FCSNavigationZoneRenderElement NewElement ( NavPoint1, NavPoint2 );
	const auto OldElementIndex = NavigationZoneRenderData . RenderElements . IndexOfByKey( NewElement ); 
	if ( OldElementIndex != INDEX_NONE ) 
	{
		const FCSNavigationZoneRenderElement PrevElement = NavigationZoneRenderData. RenderElements [ OldElementIndex ]; 
		NavigationZoneRenderData.RenderElements.RemoveAtSwap( OldElementIndex );
		NewElement.BatchedLine = PrevElement.BatchedLine;
		NewElement.BatchedLine.Color = Color;
		LineBatchComponent->Flush();
		LineBatchComponent->DrawLines ( NavigationZoneRenderData.GetBatchedLines() );
		return; 
	}

	const UCSSubsystem * CSSubsystem = GetWorld() -> GetSubsystem<UCSSubsystem>();
	const FVector L1 = CSSubsystem -> GetNavigationPoint( NavPoint1 ) -> GetActorLocation();
	const FVector L2 = CSSubsystem -> GetNavigationPoint( NavPoint2 ) -> GetActorLocation(); 
	NewElement . BatchedLine = FBatchedLine ( L1, L2, Color, Lifetime, Thickness, DepthPriority );

	NavigationZoneRenderData . RenderElements . Push ( NewElement );
	LineBatchComponent -> Flush();
	LineBatchComponent -> DrawLines ( NavigationZoneRenderData.GetBatchedLines() ); 
}

void ACSNavigationSystem::UndrawPath(int NavPoint1, int NavPoint2)
{
	const FCSNavigationZoneRenderElement NewElement ( NavPoint1, NavPoint2 );
	NavigationZoneRenderData.RenderElements.Remove ( NewElement );
	LineBatchComponent -> Flush ();
	LineBatchComponent -> DrawLines ( NavigationZoneRenderData . GetBatchedLines() );
}

const FCSNavigationGraph & ACSNavigationSystem::GetNavigationGraph() const
{
	return NavigationGraph; 
}


int32 ACSNavigationSystem::GetNumberOfNavigationPoints() const
{
	return GetNavigationGraph().GetNumberOfVertices(); 
}

bool ACSNavigationSystem::AddPath(int32 NavPoint1, int32 NavPoint2)
{
	if ( !GetIsValidNavigationPointHandle( NavPoint1 ) || !GetIsValidNavigationPointHandle( NavPoint2 ) )
		return false;

	
	const bool Success =  GetNavigationGraph().AddEdge( NavPoint1, NavPoint2 );
	if ( !Success )
		return false;
	const UCSSubsystem * CSSubsystem = GetWorld() -> GetSubsystem<UCSSubsystem>();
	if ( !IsValid ( CSSubsystem ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ("ACSNavigationSysten::AddPath() => Can't draw navigation path. UCSSubsystem is invalid." ) );
		return false; 
	}
	const auto P1 = CSSubsystem -> GetNavigationPoint( NavPoint1 );
	const auto P2 = CSSubsystem -> GetNavigationPoint( NavPoint2 );
	const FColor DrawColor = GetPathDrawColor( P1, P2 );  
	DrawPath ( NavPoint1, NavPoint2, DrawColor );
	return true;
}

bool ACSNavigationSystem::RemovePath(int32 NavPoint1, int32 NavPoint2)
{
	if ( !GetIsValidNavigationPointHandle ( NavPoint1 ) || ! GetIsValidNavigationPointHandle( NavPoint2 ) )
		return false;
	const bool Success = GetNavigationGraph() . RemoveEdge( NavPoint1, NavPoint2 );
	if ( !Success )
		return false;
	UndrawPath ( NavPoint1, NavPoint2 );
	return true;
}

bool ACSNavigationSystem::GetIsPathExists(int32 NavPoint1, int32 NavPoint2)
{
	return GetNavigationGraph().GetIsValidEdge ( NavPoint1, NavPoint2 ); 
}

bool ACSNavigationSystem::GetIsNavigationPathExists ( int32 NavPoint1, int32 NavPoint2 )
{
	return GetNavigationGraph() . GetIsPathExists ( NavPoint1, NavPoint2 ); 
}

bool ACSNavigationSystem::AddNavigationPoint(int32 NavPointHandle)
{
	if ( ! GetIsValidNavigationPointHandle( NavPointHandle ) )
		return false;
	
	return GetNavigationGraph() . AddVertex( NavPointHandle );
}


bool ACSNavigationSystem::RemoveNavigationPoint(int32 NavPointHandle)
{
	if ( ! GetIsValidNavigationPointHandle( NavPointHandle ) )
		return false;
	
	const auto & Graph = GetNavigationGraph(); // Needed to force to use const GetNavigatioNGraph variant.  
	const auto & Vertex = Graph . GetVertex ( NavPointHandle );
	for ( const auto & Neighbour : Vertex.GetEdges() )
		UndrawPath ( NavPointHandle, Neighbour.GetTargetId() ); 
	return GetNavigationGraph() . RemoveVertex ( NavPointHandle );
}

bool ACSNavigationSystem::FindPath(int32 Start, int32 Finish,
                                 TArray<int32> & OutPath) const
{
	if ( !GetIsValidNavigationPointHandle( Start ) || !GetIsValidNavigationPointHandle( Finish ) )
		return false;
	return GetNavigationGraph().FindPath ( Start, Finish, OutPath ); 
}

int32 ACSNavigationSystem::GetClosestNavigationPointToLocation(const FVector& WorldLocation) const
{
	auto Filter = [] ( const ACSNavigationPoint * )
	{
		return true; 
	};
	return GetClosestNavigationPointToLocation( WorldLocation, Filter ); 
}

int32 ACSNavigationSystem::GetClosestGoalToLocation( int32 GroupId, const FVector& WorldLocation) const
{
	auto Filter = [GroupId] ( const ACSNavigationPoint * NavigationPoint )
	{
		return NavigationPoint -> GetIsGoalPoint()
			&& NavigationPoint -> GetGoalGroupId() == GroupId
			&& !NavigationPoint -> GetIsEvacuationPoint(); 
	}; 
	return GetClosestNavigationPointToLocation(  WorldLocation, Filter ); 
}

int32 ACSNavigationSystem::GetClosestEvacuationGoalToLocation(int32 GroupId, const FVector& Location)
{ 
	auto Filter = [GroupId] ( const ACSNavigationPoint * NavigationPoint )
	{ return NavigationPoint -> GetGoalGroupId() == GroupId
		  && NavigationPoint -> GetIsGoalPoint()
		  && !NavigationPoint -> GetIsEvacuationPoint(); };
	return GetClosestNavigationPointToLocation ( Location, Filter ); 
}

int32 ACSNavigationSystem::GetRandomGoal( int32 GroupId )
{
	auto Filter = [GroupId] ( const ACSNavigationPoint * NavigationPoint )
				  { return NavigationPoint -> GetGoalGroupId() == GroupId
						&& NavigationPoint -> GetIsGoalPoint()
						&& !NavigationPoint -> GetIsEvacuationPoint(); };
	return GetRandomNavigationPoint( Filter ); 
}


int32 ACSNavigationSystem::GetRandomEvacuationGoal(int32 GroupId)
{
	auto Filter = [GroupId] ( const ACSNavigationPoint * NavPoint )
	{
		return NavPoint -> GetIsEvacuationPoint()
			&& NavPoint -> GetIsGoalPoint()
			&& NavPoint -> GetGoalGroupId() == GroupId; 
	};
	return GetRandomNavigationPoint ( Filter ); 
}

void ACSNavigationSystem::OnAgentStartedFollowingPath(int32 NavPoint1, int32 NavPoint2)
{
	if ( !GetIsPathExists( NavPoint1, NavPoint2 ) )
		return;
	GetNavigationGraph().AddFollowingAgent( NavPoint1, NavPoint2 );
}

void ACSNavigationSystem::OnAgentFinishedFollowingPath(int32 NavPoint1, int32 NavPoint2)
{
	if ( !GetIsPathExists( NavPoint1, NavPoint2 ) )
		return;
	GetNavigationGraph().RemoveFollowingAgent( NavPoint1, NavPoint2 );
}

const FCSNavigationSystemDrawSettings& ACSNavigationSystem::GetDrawSettings() const
{
	return DrawSettings; 
}

void ACSNavigationSystem::SetDrawMode(ECSNavigationSystemDrawMode DrawMode)
{
	if ( GetDrawMode() == DrawMode )
		return;
	GetDrawSettings().DrawMode = DrawMode;
	RedrawNavigation(); 
}

ECSNavigationSystemDrawMode ACSNavigationSystem::GetDrawMode() const
{
	return GetDrawSettings().DrawMode; 
}

TArray<int32> ACSNavigationSystem::GetNavigationPointPaths(int32 NavigationPointHandle) const
{
	if ( !GetNavigationGraph().GetIsValidVertex( NavigationPointHandle ) )
		return {};
	return GetNavigationGraph().GetEdges ( NavigationPointHandle );
}

void ACSNavigationSystem::RedrawNavigation()
{
	const auto CSSubsystem = GetWorld() -> GetSubsystem<UCSSubsystem>();
	if ( !CSSubsystem )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT( "ACSNavigationSystem::RedrawNavigation => Invalid CSSubsystem reference." ) );
		return; 
	}
	TArray <FCSNavigationZoneRenderElement> UpdatedRenderElements;
	UpdatedRenderElements.Reserve ( NavigationZoneRenderData.RenderElements.Num() ); 
	for (const auto & RenderElement : NavigationZoneRenderData.RenderElements )
	{
		FCSNavigationZoneRenderElement UpdatedRenderElement = RenderElement;
		const auto P1 = CSSubsystem -> GetNavigationPoint( RenderElement.NavPoint1 );
		const auto P2 = CSSubsystem -> GetNavigationPoint( RenderElement.NavPoint2 ); 
		UpdatedRenderElement.BatchedLine.Color = GetPathDrawColor( P1, P2 );
		UpdatedRenderElements.Push ( UpdatedRenderElement ); 
	}
	NavigationZoneRenderData.RenderElements = UpdatedRenderElements;
	LineBatchComponent -> Flush();
	LineBatchComponent -> DrawLines ( NavigationZoneRenderData.GetBatchedLines() );
}

float ACSNavigationSystem::GetPathDensityFactor(int32 NavPointHandle1, int32 NavPointHandle2) const
{
	return GetNavigationGraph().GetDensityFactor( NavPointHandle1, NavPointHandle2 ); 
}

float ACSNavigationSystem::GetPathNormalizedDensityFactor(int32 NavPointHandle1, int32 NavPointHandle2) const
{
	return GetNavigationGraph().GetNormalizedDensityFactor( NavPointHandle1, NavPointHandle2 ); 
}

FColor ACSNavigationSystem::GetPathDrawColor(ACSNavigationPoint* P1, ACSNavigationPoint* P2) const
{
	switch ( GetDrawMode() )
	{
		case ( ECSNavigationSystemDrawMode::Default ):
		{
			return P1 -> GetIsEvacuationPoint() || P2 -> GetIsEvacuationPoint() ? GetDrawSettings().EvacuationColor : GetDrawSettings().DefaultColor;					
		}
		case ( ECSNavigationSystemDrawMode::HeatMap ):
		{
			return FLinearColor::LerpUsingHSV( GetDrawSettings().DefaultColor,
											GetDrawSettings().DestroyColor,
											GetPathNormalizedDensityFactor( P1 -> GetHandle(), P2 -> GetHandle() ) )
								. ToFColor( true ); 
		}
		default:
		{
			UE_LOG ( LogCrowdSimulator, Warning, TEXT( "ACSNavigationSystem::GetPathDrawColor. Unrecognized draw mode: %d"), GetDrawMode() );
			return P1 -> GetIsEvacuationPoint() || P2 -> GetIsEvacuationPoint() ? GetDrawSettings().EvacuationColor : GetDrawSettings().DefaultColor; 
		}
	}
}

FCSNavigationSystemDrawSettings& ACSNavigationSystem::GetDrawSettings()
{
	return DrawSettings; 
}

FCSNavigationGraph& ACSNavigationSystem::GetNavigationGraph()
{
	return NavigationGraph; 
}

int32 ACSNavigationSystem::GetRandomNavigationPoint( const TFunction<bool(const ACSNavigationPoint*)>& Filter) const
{
	TArray<int32> FilteredPoints;
	const UCSSubsystem * Subsystem = GetWorld() -> GetSubsystem<UCSSubsystem>();
	if ( !Subsystem )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ("ACSNavigationZone::GetRandomGoal. Invalid subsystem ref. " ) );
		return CS_INVALID_HANDLE; 
	}
	for ( auto NavPointHandle : GetNavigationGraph().GetVertices() )
	{
		const ACSNavigationPoint * NavPoint = Subsystem -> GetNavigationPoint( NavPointHandle );
		if ( Filter ( NavPoint ) )
			FilteredPoints . Push ( NavPointHandle );  
	}
	if ( FilteredPoints . IsEmpty() )
		return CS_INVALID_HANDLE; 
	return FilteredPoints [ FMath::RandRange ( 0, FilteredPoints.Num() - 1 ) ]; 
}

int32 ACSNavigationSystem::GetClosestNavigationPointToLocation( const FVector& Location, const TFunction<bool(const ACSNavigationPoint*)>& Filter) const
{
	float MinDistanceSquared = UE_BIG_NUMBER;
	int32 ClosestNavPointHandle = CS_INVALID_HANDLE;
	const UCSSubsystem * Subsystem = GetWorld() -> GetSubsystem<UCSSubsystem> ();
	if ( !Subsystem )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ("ACSNavigationZone::GetClosestNavigationPointToLocation. Invalid subsystem ref. " ) );
		return CS_INVALID_HANDLE; 
	}
	
	for ( const auto & NavPointHandle : GetNavigationGraph().GetVertices() )
	{
		const ACSNavigationPoint * NavPoint = Subsystem -> GetNavigationPoint ( NavPointHandle );
		if ( !NavPoint )
		{
			UE_LOG ( LogCrowdSimulator, Error, TEXT ("ACSNavigationZone::GetClosestNavigationPointToLocation. Got invalid navigation point ref from subsystem." ) );
			return CS_INVALID_HANDLE; 
		}
		if ( ! Filter ( NavPoint ) )
			continue;
		
		const float DistanceSquared = FVector::DistSquared( NavPoint -> GetActorLocation(), Location );
		if ( DistanceSquared > MinDistanceSquared )
			continue;
		MinDistanceSquared = DistanceSquared;
		ClosestNavPointHandle = NavPointHandle; 
	}
	return ClosestNavPointHandle; 
}


// Called every frame
void ACSNavigationSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if ( GetDrawMode() == ECSNavigationSystemDrawMode::HeatMap )
		RedrawNavigation();

	/*const auto & Graph = GetNavigationGraph();
	
	for ( const auto & V : Graph.GetGraph() )
	{
		for ( const auto & E : V . Value . GetEdges() )
		{
			UCSUtilities::PrintString ( FString::FromInt ( V.Key ) +
										" - " +
										FString::FromInt ( E.GetTargetId() ) +
										" : " + 
										FString::SanitizeFloat ( GetPathDensityFactor( V.Key, E.GetTargetId() )));
		}
	}*/
}

