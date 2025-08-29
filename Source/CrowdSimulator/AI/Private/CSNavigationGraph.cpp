// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSimulator/AI/Public/CSNavigationGraph.h"
#include "CrowdSimulator/AI/Public/CSNavigationPoint.h"
#include "CrowdSimulator/AI/Public/CSSubsystem.h"

void FCSNavigationGraph::BFS ( int32 Start, TMap<int32, int32> & OutParents, TMap<int32, int32> & OutDistances ) const
{
	TQueue<int32> Q;
	// Assume that navigation graph is connective 
	OutParents . Reserve ( GetNumberOfVertices() );
	OutDistances . Reserve ( GetNumberOfVertices() ); 
	// -1 = NotFound; 0 = Open; 1 = Closed 
	TMap<int32, int32> States;
	States.Reserve ( GetNumberOfVertices() );
	// Mark all vertices as not found 
	for ( const auto & v : GetGraph() )
	{
		States . Add ( v.Key, -1 ); 
	}

	Q . Enqueue( Start );
	States [ Start ] = 0;
	OutDistances . Add ( Start, 0 );
	OutParents . Add ( Start,  CS_INVALID_HANDLE );
	while ( !Q . IsEmpty() )
	{
		int32 Current;
		Q.Dequeue( Current );
		for ( const auto & Neighbour : GetVertex ( Current ).GetEdges() )
		{
			const int32 NeighbourId = Neighbour.GetTargetId(); 
			if ( States [ NeighbourId ] == -1 )
			{
				States [ NeighbourId ] = 0;
				OutParents . Add ( NeighbourId, Current );
				OutDistances . Add (  NeighbourId, OutDistances [ Current ] + 1 );
				Q . Enqueue( NeighbourId ); 
			}
			States [ Current ] = 1; 
		}
	}
}

FCSNavigationGraphEdge::FCSNavigationGraphEdge(int32 V): TargetId ( V )
{}

bool FCSNavigationGraphEdge::operator<(const FCSNavigationGraphEdge& Other) const
{
	return GetTargetId() < Other.GetTargetId(); 
}

bool FCSNavigationGraphEdge::operator==(const FCSNavigationGraphEdge& Other) const
{
	return GetTargetId() == Other.GetTargetId(); 
}

void FCSNavigationGraphEdge::AddFollowingAgent()
{
	FollowingAgentsCount++; 
}

int32 FCSNavigationGraphEdge::GetTargetId() const
{
	return TargetId; 
}

void FCSNavigationGraphEdge::RemoveFollowingAgent()
{
	--FollowingAgentsCount;
	if ( FollowingAgentsCount < 0 )
		FollowingAgentsCount = 0; 
}

int32 FCSNavigationGraphEdge::GetFollowingAgentsCount() const
{
	return FollowingAgentsCount; 
}

bool FCSNavigationGraphVertex::operator<(const FCSNavigationGraphVertex& Other) const
{
	return VertexId < Other.VertexId;
}

void FCSNavigationGraphVertex::AddEdge(int32 Target)
{
	GetEdges().Add ( FCSNavigationGraphEdge ( Target ) ); 
}

void FCSNavigationGraphVertex::RemoveEdge(int32 Target)
{
	GetEdges().Remove ( FCSNavigationGraphEdge ( Target ) ); 
}

void FCSNavigationGraphVertex::AddFollowingAgent(int32 Target)
{
	if ( !GetHasEdge( Target ) )
		return; 
	GetEdge ( Target ) . AddFollowingAgent();
}

void FCSNavigationGraphVertex::RemoveFollowingAgent(int32 Target)
{
	if ( !GetHasEdge( Target ) )
		return;
	GetEdge( Target ) . RemoveFollowingAgent();
}

const FCSNavigationGraphEdge& FCSNavigationGraphVertex::GetEdge(int32 Target) const
{
	return *GetEdges().Find( FCSNavigationGraphEdge ( Target ) );
}

int32 FCSNavigationGraphVertex::GetVertexId() const
{
	return VertexId; 
}

bool FCSNavigationGraphVertex::GetHasEdge(int32 Target) const
{
	return GetEdges().Contains( FCSNavigationGraphEdge ( Target ) ); 
}

int32 FCSNavigationGraphVertex::GetNumberOfFollowingAgents(int32 Target) const
{
	if ( !GetHasEdge ( Target ) )
		return 0; 
	return GetEdge ( Target ) . GetFollowingAgentsCount();
}

const TSet<FCSNavigationGraphEdge>& FCSNavigationGraphVertex::GetEdges() const
{
	return Edges;
}

FCSNavigationGraphEdge& FCSNavigationGraphVertex::GetEdge(int32 Target)
{
	return *GetEdges() . Find ( FCSNavigationGraphEdge ( Target ) ); 
}

TSet<FCSNavigationGraphEdge>& FCSNavigationGraphVertex::GetEdges()
{
	return Edges;
}

uint32 GetTypeHash(const FCSNavigationGraphEdge& Edge)
{
	return GetTypeHash ( Edge . GetTargetId() ); 
}

FCSNavigationGraphVertex::FCSNavigationGraphVertex(int32 Vertex, const UObject* WorldContext)
: VertexId ( Vertex ), WorldContextObject( WorldContext )
{}

FCSNavigationGraphVertex::FCSNavigationGraphVertex(int32 Vertex)
: VertexId ( Vertex ) {
}

bool FCSNavigationGraph::FindPath ( int32 StartNode, int32 EndNode, TArray <int32> & OutPath ) const
{
	if ( !GetIsValidVertex( StartNode ) || !GetIsValidVertex( EndNode ) )
		return false;
		
	TArray <AStarNode> OpenHeap;
	TMap <int32, int32> PathMap;
	TMap <int32, float> CostMap;
	TSet<int32> ClosedSet; 

	OpenHeap . HeapPush ( { StartNode, 0.f }  );
	PathMap . Add ( StartNode, StartNode );
	CostMap . Add ( StartNode, 0.f ); 
		
	while ( ! OpenHeap . IsEmpty() )
	{
		AStarNode CurrentNode; 
		OpenHeap . HeapPop ( CurrentNode );
		const int32 Current = CurrentNode . NodeRef;
			
		if ( Current == EndNode )
			break;
		
		const bool ShouldApplyRandom = GetShouldApplyRandomFactor( Current, ClosedSet );
		for ( const auto & Neighbour : GetVertex ( Current ) . GetEdges() )
		{
			const int32 NeighbourId = Neighbour.GetTargetId();
			const float Cost = CostMap [ Current ] + GetTraversalCost( Current, NeighbourId, ShouldApplyRandom );
			if ( !CostMap . Contains( NeighbourId ) || CostMap [ NeighbourId ] > Cost )
			{
				CostMap . Add ( NeighbourId, Cost ); 
				const float Priority = Cost + GetHeuristicCost( NeighbourId, EndNode );
				OpenHeap . HeapPush ( { NeighbourId, Priority } );
				PathMap . Add ( NeighbourId, Current ); 
			}
		}
		ClosedSet.Add ( Current ); 
	}
	return ReconstructPath ( StartNode, EndNode, PathMap, OutPath );
}

bool FCSNavigationGraph::AddVertex(int32 NavPointHandle)
{
	if ( GetIsValidVertex( NavPointHandle ) )
		return false;
	FCSNavigationGraphVertex v ( NavPointHandle, WorldContextObject ); 
	GetGraph().Add( {NavPointHandle, v } ); 
	return true; 
}

bool FCSNavigationGraph::AddEdge(int32 v, int32 u)
{
	if ( v == u )
		return false; 
	
	if ( !GetIsValidVertex( v ) || !GetIsValidVertex( u ) )
		return false;
	
	GetVertex( v ).AddEdge( u );
	GetVertex( u ).AddEdge( v ); 
	return true; 
}

bool FCSNavigationGraph::RemoveEdge(int32 v, int32 u)
{
	if ( !GetIsValidEdge ( v, u ) )
		return false;

	GetVertex( v ).RemoveEdge( u ); 
	GetVertex( u ).RemoveEdge( v );
	return true; 
}

bool FCSNavigationGraph::RemoveVertex(int32 v)
{
	if ( !GetIsValidVertex( v ) )
		return false;
	GetGraph().Remove ( v );
	for ( auto & u : Graph )
	{
		u.Value.RemoveEdge( v );  
	}
	return true; 
}

bool FCSNavigationGraph::GetIsEmpty() const
{
	return GetNumberOfVertices() <= 0; 
}

bool FCSNavigationGraph::GetIsConnective() const
{
	if ( GetIsEmpty() )
		return false; 
	TMap <int32, int32> OutParents;
	TMap <int32, int32> OutDistances;
	const auto Start = GetGraph().begin().Key(); 
	BFS ( Start, OutParents, OutDistances );
	return OutDistances.Num() == GetNumberOfVertices();
}

void FCSNavigationGraph::AddFollowingAgent(int32 u, int32 v)
{
	if ( !GetIsValidEdge( u, v ) )
		return;
	GetVertex ( u ).AddFollowingAgent( v );
	GetVertex ( v ).AddFollowingAgent( u ); 
}

void FCSNavigationGraph::RemoveFollowingAgent(int32 u, int32 v)
{
	if ( !GetIsValidEdge( u, v ) )
		return;
	GetVertex( u ).RemoveFollowingAgent( v );
	GetVertex( v ).RemoveFollowingAgent( u );
}

TSet<int32> FCSNavigationGraph::GetVertices() const
{
	// fucking garbage peace of shit. Why there is no "const TSet<T> & TMap<T, whatever>::GetKeys() const" ??? 
	TSet<int32> OutKeys;
	GetGraph().GetKeys ( OutKeys );
	return OutKeys; 
}

const FGraphType& FCSNavigationGraph::GetGraph() const
{
	return Graph; 
}

TArray<int32> FCSNavigationGraph::GetEdges(int32 u) const
{
	if ( !GetIsValidVertex( u ) )
		return {};
	TArray<int32> OutEdges;
	OutEdges.Reserve ( GetVertex( u ).GetEdges().Num() );
	for ( const auto Edge : GetVertex ( u ).GetEdges() )
	{
		OutEdges.Push ( Edge.GetTargetId() );
	}
	return OutEdges;
}

bool FCSNavigationGraph::GetIsValidEdge(int32 u, int32 v) const
{
	bool r = GetIsValidVertex ( u ) && GetIsValidVertex( v )
		&& GetVertex ( u ) . GetHasEdge( v )
		&& GetVertex ( v ) . GetHasEdge( u );
	return r;  
}

bool FCSNavigationGraph::GetIsValidVertex(int32 v) const
{
	return GetGraph().Contains( v ); 
}

int32 FCSNavigationGraph::GetNumberOfVertices() const
{
	return GetGraph().Num();
}

bool FCSNavigationGraph::GetIsPathExists(int32 NavPoint1, int32 NavPoint2) const
{	
	TArray <int32> OutPath; 	
	return FindPath ( NavPoint1, NavPoint2, OutPath ); 
}

const FCSNavigationGraphVertex& FCSNavigationGraph::GetVertex(int32 VertexId) const
{
	return GetGraph()[VertexId];
}

	

float FCSNavigationGraph::GetDensityFactor(int32 u, int32 v) const
{
	if ( !GetIsValidEdge ( u , v ) )
		return 0.f;
	const auto NumberOfFollowingAgents  = GetVertex( u ) . GetNumberOfFollowingAgents( v );

	const auto CSSubsystem = GetCSSubsystem();
	if ( !GetCSSubsystem() )
		return 0.f;

	return CSSubsystem -> GetNavigationDensityFactor() * NumberOfFollowingAgents; 
}

float FCSNavigationGraph::GetRandomFactor(int32 u, int32 v) const
{
	if ( !GetIsValidEdge( u, v ) )
		return 0.f;
	if ( ! GetCSSubsystem() )
		return 0.f;
	return FMath::RandRange ( 0.f, GetCSSubsystem() -> GetNavigationRandomFactor() ); 
}

float FCSNavigationGraph::GetNormalizedDensityFactor(int32 u, int32 v) const
{
	if ( !GetIsValidEdge ( u , v ) )
		return 0.f;
	const auto CSSubsystem = GetCSSubsystem();
	if ( !GetCSSubsystem() )
		return 0.f;

	return GetDensityFactor( u , v ) / GetCSSubsystem() -> GetNavigationMaxDensityFactor(); 
}

FGraphType& FCSNavigationGraph::GetGraph()
{
	return Graph; 
}

FCSNavigationGraphVertex& FCSNavigationGraph::GetVertex(int32 VertexId)
{
	return GetGraph()[VertexId];
}

bool FCSNavigationGraph::AStarNode::operator<(const AStarNode& rhs) const
{ return Price < rhs . Price; }

bool FCSNavigationGraph::GetShouldApplyRandomFactor(int32 u, const TSet<int32>& ProcessedNodes) const
{
	int32 FoundOpenedNeighbours = 0;
	constexpr int32 OpenedNeighboursForRandom = 2; 
	for ( const auto & Neighbour : GetVertex( u ) . GetEdges() )
	{
		if ( ! ProcessedNodes . Contains( Neighbour.GetTargetId() ) )
		{
			FoundOpenedNeighbours++; 
			if ( FoundOpenedNeighbours >= OpenedNeighboursForRandom )
			{
				return true; 
			}
		}
	}
	return false; 
}

float FCSNavigationGraph::GetTraversalCost(int32 u, int32 v, bool bUseRandomFactor = false) const
{
	const UCSSubsystem * Subsystem = GetCSSubsystem();
	if ( !Subsystem )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT("FCSNavigationGraph::GetTraversalCost => Can't get CSSubsystem" ) );
		return UE_MAX_FLT; 
	}

	const ACSNavigationPoint * A = Subsystem -> GetNavigationPoint( u );
	const ACSNavigationPoint * B = Subsystem -> GetNavigationPoint( v );
	const float DensityFactor = GetDensityFactor( u, v ); 
	const float RandomFactor = bUseRandomFactor ?  GetRandomFactor ( u, v ) : 0.f; 
	
	const float Distance = FVector::Distance ( A -> GetActorLocation(), B -> GetActorLocation() );
	const float Price = Distance - DensityFactor + RandomFactor;
	return FMath::Clamp ( Price, 0.f, Price ); 
}

float FCSNavigationGraph::GetHeuristicCost(int32 u, int32 v) const
{
	const UCSSubsystem * Subsystem = GetCSSubsystem();
	if ( !Subsystem )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT("FCSNavigationGraph::GetTraversalCost => Can't get CSSubsystem" ) );
		return UE_MAX_FLT; 
	}

	const ACSNavigationPoint * A = Subsystem -> GetNavigationPoint( u );
	const ACSNavigationPoint * B = Subsystem -> GetNavigationPoint( v );
	const float Distance = FVector::Distance ( A -> GetActorLocation(), B -> GetActorLocation() ); 
	
	return Distance;
}

bool FCSNavigationGraph::ReconstructPath ( int32 Start, int32 Finish, const TMap <int32, int32> & PathMap, TArray <int32> & Path )
{
	int32 Current = Finish;
	if ( !PathMap . Contains( Finish ) )
		return false;

	Path . Empty(); 
		
	do 
	{
		Path . Push ( Current );
		Current = PathMap [ Current ];
	} while ( Current != Start );
	if ( Start != Finish  ) 
		Path . Push ( Start ); 
	Algo::Reverse ( Path );
	return true; 
}
UCSSubsystem* FCSNavigationGraph::GetCSSubsystem() const
{
	if ( !WorldContextObject )
		return nullptr; 
	if ( WorldContextObject -> GetWorld() )
		return WorldContextObject -> GetWorld() -> GetSubsystem<UCSSubsystem> ();
	return nullptr; 
}	