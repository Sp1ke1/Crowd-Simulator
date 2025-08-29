// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSAgent.h"
#include "CSNavigationGraph.generated.h"


struct FCSNavigationGraphEdge  
{
public:
	explicit FCSNavigationGraphEdge ( int32 Target ); 
	
	bool operator < ( const FCSNavigationGraphEdge & Other ) const;

	bool operator == ( const FCSNavigationGraphEdge & Other ) const; 
	
	void AddFollowingAgent ();

	void RemoveFollowingAgent ();

	int32 GetFollowingAgentsCount () const; 

	int32 GetTargetId () const; 

private:
	int32 FollowingAgentsCount = 0; 
	int32 TargetId;
};

uint32 GetTypeHash(const FCSNavigationGraphEdge& Edge);


struct FCSNavigationGraphVertex		
{
public: 

	FCSNavigationGraphVertex (int32 Vertex, const UObject * WorldContext );

	explicit FCSNavigationGraphVertex ( int32 Vertex );
	
	bool operator < ( const FCSNavigationGraphVertex & Other ) const; 

	void AddEdge ( int32 Target );

	void RemoveEdge ( int32 Target );

	void AddFollowingAgent ( int32 Target );

	void RemoveFollowingAgent ( int32 Target );

	const FCSNavigationGraphEdge & GetEdge ( int32 Target ) const; 
	
	int32 GetVertexId () const;

	bool GetHasEdge ( int32 Target ) const;

	int32 GetNumberOfFollowingAgents ( int32 Target ) const; 
	
	const TSet<FCSNavigationGraphEdge> & GetEdges() const;

private:

	FCSNavigationGraphEdge & GetEdge ( int32 Target ); 
	
	TSet<FCSNavigationGraphEdge> & GetEdges();
	
	int32 VertexId;

	TSet<FCSNavigationGraphEdge> Edges; 

	const UObject * WorldContextObject = nullptr;
};

typedef TMap<int32, FCSNavigationGraphVertex> FGraphType;

USTRUCT()
struct CROWDSIMULATOR_API FCSNavigationGraph
{

public: 
	GENERATED_BODY()
	FCSNavigationGraph ( const UObject * WorldContextObject )
		: WorldContextObject( WorldContextObject )
	{};

	// Don't use it
	FCSNavigationGraph ()
	{}; 
	
	bool FindPath ( int32 StartNode, int32 EndNode, TArray <int32> & OutPath ) const;
	
	bool AddVertex ( int32 NavPointHandle );
	
	bool AddEdge ( int32 v, int32 u );

	bool RemoveEdge ( int32 v, int32 u );
	
	bool RemoveVertex ( int32 v );

	bool GetIsEmpty() const;
	
	bool GetIsConnective () const;

	void AddFollowingAgent ( int32 u, int32 v );

	void RemoveFollowingAgent ( int32 u, int32 v ); 

	TSet<int32> GetVertices () const;

	const FGraphType & GetGraph () const;

	TArray<int32> GetEdges(int32 u) const;;

	bool GetIsValidEdge ( int32 u, int32 v ) const;

	bool GetIsValidVertex ( int32 v ) const;

	int32 GetNumberOfVertices() const;

	bool GetIsPathExists(int32 NavPoint1, int32 NavPoint2) const;

	const FCSNavigationGraphVertex & GetVertex ( int32 VertexId ) const; 

	float GetDensityFactor ( int32 u, int32 v ) const;

	float GetRandomFactor(int32 u, int32 v) const;

	float GetNormalizedDensityFactor ( int32 u, int32 v ) const; 
		
private:

	FGraphType & GetGraph (); 
	
	FCSNavigationGraphVertex & GetVertex ( int32 VertexId );
	
	void BFS ( int32 Start, TMap<int32, int32> & OutParents, TMap<int32, int32> & OutDistances ) const;

	struct AStarNode
	{
		int32 NodeRef;
		float Price;
		bool operator < ( const AStarNode & rhs ) const;
	};


	bool GetShouldApplyRandomFactor ( int32 u, const TSet<int32> & ProcessedNodes ) const; 

	float GetTraversalCost ( int32 u, int32 v, bool bUseRandomFactor ) const;

	float GetHeuristicCost ( int32 u, int32 v ) const;

	UCSSubsystem* GetCSSubsystem() const;

	static bool ReconstructPath ( int32 Start, int32 Finish, const TMap <int32, int32> & PathMap, TArray <int32> & Path ); 
	
	FGraphType Graph;
	
	UPROPERTY()
	const UObject * WorldContextObject = nullptr; 
};

