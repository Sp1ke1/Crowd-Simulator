// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSNavigationSystem.h"
#include "CrowdSimulator/Core/Public/CSCore.h"
#include "CrowdSimulator/Core/Public/CSSaveGame.h"
#include "Subsystems/WorldSubsystem.h"
#include "CSSubsystem.generated.h"
class ACSAgentSpawner;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSimulationStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnSimulationPaused );
DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnSimulationStopped );
DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnEvacuationStarted );
DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnEvacuationEnded ); 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam ( FOnSimulationErrorMessage, FString, Message ); 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam ( FOnSimulationMessage, FString, Message ); 


UENUM ( BlueprintType )
enum ESimulationMode
{
	Normal,
	Evacuation
};

USTRUCT ()
struct  FCSObjectsContainer
{

	GENERATED_BODY()
	
	TSparseArray<ACSAgent*> Agents;
	TSparseArray<ACSNavigationPoint*> NavigationPoints;
	ACSNavigationSystem * NavigationSystem; 
};

UENUM( BlueprintType )
enum EGoalSearchType
{
	Random,
	Closest
};

/**
 * 
 */

class ACSAgent;
class ACSNavigationPoint;
class ACSNavigationSystem;
struct FCSNavigationPath; 
UCLASS( Blueprintable )
class CROWDSIMULATOR_API UCSSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION( BlueprintCallable, BlueprintPure )
	void GetNavigationPointsInRadius (const FVector& Location, float Radius, TArray<int32>& OutNavigationPointHandles) const; 
	
	UFUNCTION( BlueprintCallable, BlueprintPure ) 
	ACSNavigationPoint * GetNavigationPoint ( int32 NavPointHandle ) const;

	UFUNCTION( BlueprintCallable, BlueprintPure )
	ACSAgent * GetAgent ( int32 AgentHandle ) const;

	UFUNCTION( BlueprintCallable, BlueprintPure )
	ACSNavigationSystem * GetNavigationSystem () const; 
	
	UFUNCTION( BlueprintCallable ) 
	bool RegisterNavigationPoint ( ACSNavigationPoint * NavigationPoint );

	UFUNCTION ( BlueprintCallable )
	bool RegisterNavigationPointWithHandle ( ACSNavigationPoint * NavigationPoint, int32 Handle );

	UFUNCTION( BlueprintCallable )
	bool UnregisterNavigationPoint ( int32 NavPointHandle );

	UFUNCTION( BlueprintCallable )
	bool RegisterAgent ( ACSAgent * Agent );

	UFUNCTION( BlueprintCallable )
	bool RegisterAgentWithHandle ( ACSAgent * Agent, int32 Handle ); 

	UFUNCTION( BlueprintCallable )
	bool UnregisterAgent ( int32 AgentHandle );
	
	UFUNCTION( BlueprintCallable, BlueprintPure )
	bool GetIsAgentRegistered ( int32 AgentHandle ) const; 
	
	UFUNCTION( BlueprintCallable, BlueprintPure )
	bool GetIsNavigationPointRegistered ( int32 NavPointHandle ) const;
	
	UFUNCTION( BlueprintCallable )
	UCSNavigationPath * FindPath ( int32 AgentHandle, EGoalSearchType GoalSearchType );

	UFUNCTION( BlueprintCallable )
	void UpdatePath ( UCSNavigationPath * Path ); 
	
	UFUNCTION( BlueprintCallable, BlueprintPure )
	ACSNavigationSystem* GetOrCreateNavigationSystem();

	UFUNCTION( BlueprintCallable )
	bool RemoveNavigationPath ( int32 NavPointHandle1, int32 NavPointHandle2 ); 

	UFUNCTION( BlueprintCallable )
	bool AddNavigationPath ( int32 NavPointHandle1, int32 NavPointHandle2 ); 

	UFUNCTION( BlueprintCallable, BlueprintPure )
	bool GetIsPathExists(int32 NavPointHandle1, int32 NavPointHandle2) const;
	
	UFUNCTION( BlueprintCallable, BlueprintPure )
	bool GetIsNavigationPathExists(int32 NavPointHandle1, int NavPointHandle2) const;

	UFUNCTION( BlueprintCallable, BlueprintPure )
	bool GetIsSimulating() const ;

	UFUNCTION( BlueprintCallable )
	void SetNavigationGraphVisibility ( bool Visibility ); 
	
	UFUNCTION( BlueprintCallable )
	void SetNavigationPointsVisibility ( bool Visibility ); 
	
	UFUNCTION( BlueprintCallable, BlueprintPure )
	bool GetNavigationGraphVisibility () const; 
	
	UFUNCTION( BlueprintCallable, BlueprintPure )
	bool GetNavigationPointsVisibility () const; 
	
	UFUNCTION( BlueprintCallable, BlueprintPure )
	FCSSaveGameContainer GetLevelSaveData () const; 

	UFUNCTION( BlueprintCallable )
	void LoadLevelFromSaveData(const FCSSaveGameContainer& LevelData);

	UFUNCTION( BlueprintCallable, BlueprintPure )
	TArray<int32> GetNavigationPointPaths(int32 NavigationPointHandle ) const;

	const TSparseArray <ACSAgent*> & GetAgents () const;
	
	const TSparseArray<ACSNavigationPoint*> & GetNavigationPoints () const;

	UFUNCTION( BlueprintCallable, BlueprintPure ) 
	float GetNavigationRandomFactor() const;

	UFUNCTION( BlueprintCallable, BlueprintPure )
	float GetNavigationMaxRandomFactor () const; 
	
	UFUNCTION( BlueprintCallable )
	void SetNavigationRandomFactor ( float RandomFactor ); 

	UPROPERTY( BlueprintAssignable )
	FOnSimulationStarted OnSimulationStartedDelegate; 

	UPROPERTY( BlueprintAssignable )
	FOnSimulationPaused OnSimulationStoppedDelegate;
	
	UPROPERTY( BlueprintAssignable )
	FOnSimulationPaused OnSimulationPausedDelegate;

	UPROPERTY( BlueprintAssignable )
	FOnEvacuationStarted OnEvacuationStartedDelegate;
	
	UPROPERTY( BlueprintAssignable )
	FOnEvacuationEnded OnEvacuationEndedDelegate;
	
	UPROPERTY( BlueprintAssignable )
	FOnSimulationErrorMessage OnSimulationErrorMessageDelegate; 
	
	UPROPERTY( BlueprintAssignable )
	FOnSimulationMessage OnSimulationMessageDelegate;

	UFUNCTION( BlueprintCallable, BlueprintPure )
	float GetNavigationDensityFactor () const;

	UFUNCTION( BlueprintCallable, BlueprintPure )
	float GetNavigationMaxDensityFactor () const; 
	
	UFUNCTION( BlueprintCallable )
	void SetNavigationDensityFactor ( float DensityFactor );

	UFUNCTION( BlueprintCallable, BlueprintPure )
	bool GetAgentsReconsiderPaths () const;

	UFUNCTION( BlueprintCallable )
	void SetAgentsReconsiderPaths ( bool AgentsReconsiderPaths );

	UFUNCTION( BlueprintCallable )
	bool ValidateEvacuationNavigation() const ;

	UFUNCTION( BlueprintCallable )
	void SetSimulationMode ( ESimulationMode NewSimulationMode );

	UFUNCTION( BlueprintCallable )
	void SetEvacuationGroupId ( int32 GroupId ); 

	UFUNCTION( BlueprintCallable )
	ESimulationMode GetSimulationMode () const;
	
	UFUNCTION( BlueprintCallable )
	void GetGroupEvacuationGoals( int32 GroupId, TArray<int32>& OutGoals) const;

	UFUNCTION( BlueprintCallable )
	void GetGroupNonEvacuationGoals ( int32 GroupId, TArray<int32>& OutGoals ) const; 

	UFUNCTION( BlueprintCallable )
	void SetNavigationDrawMode ( ECSNavigationSystemDrawMode DrawMode );

	UFUNCTION( BlueprintCallable )
	ECSNavigationSystemDrawMode GetNavigationDrawMode() const;
	
protected:
	UFUNCTION()
	void OnAgentStartedFollowingPath (int32 NavPointHandle1, int32 NavPointHandle2);

	UFUNCTION()
	void OnAgentFinishedFollowingPath (int32 NavPointHandle1, int32 NavPointHandle2);

	void UpdateNavigationVisibility();
	
	void GetNavigationPoints ( TArray<int32> & OutPoints, const TFunction<bool( ACSNavigationPoint * )> & Filter ) const; 
	
	bool ValidateNavigation () const;

	void StartEvacuation();

	void EndEvacuation(); 


	UPROPERTY()
	TSubclassOf<ACSNavigationSystem> NavigationSystemClass; 

	UPROPERTY()
	TSubclassOf<ACSNavigationPoint> NavigationPointClass;

	UPROPERTY()
	TSubclassOf<ACSAgentSpawner> AgentSpawnerClass; 

	UPROPERTY()
	TSubclassOf<ACSAgent> AgentClass; 
	
	UPROPERTY()
	float NavigationDensityFactor = 25.f;

	UPROPERTY()
	float NavigationMaxDensityFactor = 50.f; 

	UPROPERTY()
	float NavigationRandomFactor = 50.f;

	UPROPERTY()
	float NavigationMaxRandomFactor = 200.f;
	
	UPROPERTY()
	float EvacuationSpeedMultiplier = 2.f; 

	UPROPERTY()
	TEnumAsByte<ESimulationMode> SimulationMode = ESimulationMode::Normal;

	UPROPERTY()
	int32 EvacuationGroupId = CS_INVALID_HANDLE; 
	
	UPROPERTY()
	bool bAgentsReconsiderPaths = false; 
	
	TSparseArray <ACSAgent*> & GetAgents ();

	TSparseArray<ACSNavigationPoint*> & GetNavigationPoints ();
	
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	virtual TStatId GetStatId() const override;

	bool bIsSimulating = false;

	bool bIsNavigationGraphVisible = true; 
	
	bool bIsNavigationPointsVisible = true; 

	UFUNCTION( BlueprintCallable ) 
	void StartSimulation ();

	UFUNCTION( BlueprintCallable )
	void StopSimulation ();

	UFUNCTION( BlueprintCallable )
	void PauseSimulation ();

	FCSObjectsContainer Objects; 
};
