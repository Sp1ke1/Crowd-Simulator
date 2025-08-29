// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSNavigationGraph.h"
#include "CSNavigationPoint.h"
#include "Components/LineBatchComponent.h"
#include "CSNavigationSystem.generated.h"


class ACSNavigationPoint;

struct FCSNavigationZoneRenderElement
{
	FCSNavigationZoneRenderElement ( int32 p1, int32 p2 )
		: NavPoint1 ( p1 ), NavPoint2( p2 )
	{}; 
	
	int32 NavPoint1;
	int32 NavPoint2;
	FBatchedLine BatchedLine;

	bool operator == ( const FCSNavigationZoneRenderElement & rhs ) const
	{
		return NavPoint1 == rhs . NavPoint1 && NavPoint2 == rhs . NavPoint2
		|| NavPoint1 == rhs . NavPoint2 && NavPoint2 == rhs . NavPoint1; 
	}
};

USTRUCT( BlueprintType )
struct FCSNavigationZoneRenderData
{
	GENERATED_BODY()
	TArray <FCSNavigationZoneRenderElement> RenderElements;

	TArray<FBatchedLine> GetBatchedLines () const
	{
		TArray <FBatchedLine> BatchedLines;
		BatchedLines.Reserve ( RenderElements . Num() ); 
		for ( auto & e : RenderElements )
		{
			BatchedLines.Push ( e . BatchedLine ); 
		}
		return BatchedLines; 
	}
	
};

class UCSSubsystem; 
UCLASS( Blueprintable )
class CROWDSIMULATOR_API ACSNavigationSystem : public AActor
{
	GENERATED_BODY()


	friend UCSSubsystem;
	
public:
	// Sets default values for this actor's properties
	ACSNavigationSystem();

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	bool GetIsValidNavigationPointHandle ( int32 NavPointHandle ) const;
	
	UPROPERTY()
	ULineBatchComponent * LineBatchComponent;  
	
public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION( BlueprintCallable )
	void DrawPath ( int NavPoint1, int NavPoint2, const FColor & Color = FColor::Yellow, float Lifetime = 0.f, float Thickness = 15.f, uint8 DepthPriority = 0 );

	UFUNCTION( BlueprintCallable )
	void UndrawPath ( int NavPoint1, int NavPoint2 );
	
	const FCSNavigationGraph& GetNavigationGraph() const;

	UFUNCTION( BlueprintCallable )
	int32 GetNumberOfNavigationPoints () const; 
	
	UFUNCTION( BlueprintCallable )
	bool AddPath (  int32 NavPoint1, int32 NavPoint2 );

	UFUNCTION( BlueprintCallable )
	bool RemovePath ( int32 NavPoint1, int32 NavPoint2 );
	
	UFUNCTION( BlueprintCallable, BlueprintPure )
	bool GetIsPathExists ( int32 NavPoint1, int32 NavPoint2 );

	UFUNCTION( BlueprintCallable, BlueprintPure )
	bool GetIsNavigationPathExists(int32 NavPoint1, int32 NavPoint2);

	UFUNCTION( BlueprintCallable )
	bool AddNavigationPoint ( int32 NavPointHandle ); 

	UFUNCTION( BlueprintCallable )
	bool RemoveNavigationPoint ( int32 NavPointHandle );

	UFUNCTION( BlueprintCallable )
	bool FindPath ( int32 Start, int32 Finish, TArray<int32> & OutPath ) const;

	UFUNCTION( BlueprintCallable, BlueprintPure )
	int32 GetClosestNavigationPointToLocation ( const FVector & WorldLocation ) const;

	UFUNCTION( BlueprintCallable, BlueprintPure )
	int32 GetClosestGoalToLocation (int32 GroupId, const FVector& WorldLocation) const;

	UFUNCTION( BlueprintCallable )
	int32 GetRandomGoal (int32 GroupId);
	
	UFUNCTION( BlueprintCallable ) 
	int32 GetClosestEvacuationGoalToLocation(int32 GroupId, const FVector& Location);

	UFUNCTION( BlueprintCallable ) 
	int32 GetRandomEvacuationGoal(int32 GroupId);

	UFUNCTION( BlueprintCallable )
	void OnAgentStartedFollowingPath ( int32 NavPoint1, int32 NavPoint2 );

	UFUNCTION( BlueprintCallable )
	void OnAgentFinishedFollowingPath ( int32 NavPoint1, int32 NavPoint2 );

	UFUNCTION( BlueprintCallable )
	const FCSNavigationSystemDrawSettings & GetDrawSettings() const;

	UFUNCTION( BlueprintCallable )
	void SetDrawMode ( ECSNavigationSystemDrawMode DrawMode );

	UFUNCTION( BlueprintCallable )
	ECSNavigationSystemDrawMode GetDrawMode () const;

	UFUNCTION( BlueprintCallable, BlueprintPure )
	TArray<int32> GetNavigationPointPaths(int32 NavigationPointHandle ) const;

protected:
	void RedrawNavigation();

	float GetPathDensityFactor( int32 NavPointHandle1, int32 NavPointHandle2 ) const;

	float GetPathNormalizedDensityFactor( int32 NavPointHandle1, int32 NavPointHandle2 ) const;
	
	FColor GetPathDrawColor ( ACSNavigationPoint * P1, ACSNavigationPoint * P2 ) const; 
	
	FCSNavigationSystemDrawSettings & GetDrawSettings(); 

	FCSNavigationGraph & GetNavigationGraph (); 
	
	int32 GetRandomNavigationPoint ( const TFunction<bool( const ACSNavigationPoint * )> & Filter ) const;
	int32 GetClosestNavigationPointToLocation(const FVector& Location,
	                                          const TFunction<bool(const ACSNavigationPoint*)>& Filter) const;
	
	UPROPERTY( EditDefaultsOnly )
	FCSNavigationSystemDrawSettings DrawSettings; 

	UPROPERTY()
	FCSNavigationGraph NavigationGraph;

	UPROPERTY()
	FCSNavigationZoneRenderData NavigationZoneRenderData; 
};
