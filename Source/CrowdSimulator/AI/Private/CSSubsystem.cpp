// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSimulator/AI/Public/CSSubsystem.h"
#include "CrowdSimulator/AI/Public/CSAgent.h"
#include "CrowdSimulator/AI/Public/CSAgentSpawner.h"
#include "CrowdSimulator/AI/Public/CSNavigationPath.h"
#include "CrowdSimulator/AI/Public/CSNavigationPoint.h"
#include "CrowdSimulator/AI/Public/CSNavigationSystem.h"
#include "CrowdSimulator/AI/Public/CSSettings.h"
#include "CrowdSimulator/UI/Public/CSPlayerUIComponent.h"
#include "Kismet/GameplayStatics.h"


void UCSSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	const auto Settings = GetDefault<UCSSettings>();
	if ( !Settings )
	{
		UE_LOG ( LogCrowdSimulator, Warning, TEXT( "UCSSubystem::Initialize => Can't get CSSettings." ));
		NavigationSystemClass = ACSNavigationSystem::StaticClass(); 
		return; 
	}
	NavigationSystemClass = Settings -> GetNavigationSystemClass();
	AgentSpawnerClass = Settings -> GetAgentSpawnerClass();
	AgentClass = Settings -> GetAgentClass();
	NavigationPointClass = Settings -> GetNavigationPointClass();
	NavigationMaxDensityFactor = Settings -> GetMaxDensityFactor();
	NavigationMaxRandomFactor = Settings -> GetMaxRandomFactor(); 
}

void UCSSubsystem::GetNavigationPointsInRadius(const FVector& Location, float Radius,
                                               TArray<int32>& OutNavigationPointHandles) const
{
	OutNavigationPointHandles.Empty(); 
	for ( const auto NavPoint : Objects.NavigationPoints )
	{
		if ( FVector::Distance ( NavPoint -> GetActorLocation(), Location ) < Radius )
			OutNavigationPointHandles.Push ( NavPoint -> GetHandle() ); 
	} 
}	

ACSNavigationPoint* UCSSubsystem::GetNavigationPoint(int32 NavPointHandle) const 
{
	if ( ! GetIsNavigationPointRegistered( NavPointHandle ) )
		return nullptr; 
	return Objects.NavigationPoints [ NavPointHandle ];
}

ACSAgent* UCSSubsystem::GetAgent(int32 AgentHandle) const
{
	if ( !GetIsAgentRegistered( AgentHandle ) )
		return nullptr;
	return Objects . Agents [ AgentHandle ]; 
}

void UCSSubsystem::StartSimulation()
{
	if ( bIsSimulating )
		return; 
	
	UE_LOG ( LogCrowdSimulator, Log, TEXT ( "CSSubsystem: StartSimulation requested. " ) )

	if ( ! ValidateNavigation() )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "CSSubsystem: Simulation can't be started. Navigation is invalid" ) );
		return; 
	}
	bIsSimulating = true;
	for ( const auto & Agent : GetAgents() )
		Agent -> StartLogic ();

	for ( const auto & NavPoint : GetNavigationPoints() )
	{
		ACSAgentSpawner * Spawner = Cast<ACSAgentSpawner> ( NavPoint );
		if ( Spawner )
		{
			Spawner -> StartSpawning(); 
		}
	}
	OnSimulationStartedDelegate . Broadcast();
	OnSimulationMessageDelegate.Broadcast ("Started simulation." ); 
	UE_LOG ( LogCrowdSimulator, Log, TEXT ( "CSSubsystem: Started simulation.") )
}

void UCSSubsystem::StopSimulation()
{
	if ( !bIsSimulating )
		return;
	bIsSimulating = false;
	UE_LOG ( LogCrowdSimulator, Log, TEXT ( "CSSubsystem: StopSimulation requested. " ) )
	for ( const auto & Agent : GetAgents() )
		Agent -> StopLogic ();

	for ( const auto & NavPoint : GetNavigationPoints() )
	{
		ACSAgentSpawner * Spawner = Cast<ACSAgentSpawner> ( NavPoint );
		if ( Spawner )
		{
			Spawner -> StopSpawning(); 
		}
	}
	OnSimulationStoppedDelegate . Broadcast();
	OnSimulationMessageDelegate. Broadcast ("Stopped simulation."); 
	UE_LOG ( LogCrowdSimulator, Log, TEXT ( "CSSubsystem: Stopped simulation. " ) )
}

void UCSSubsystem::PauseSimulation()
{
	if ( !bIsSimulating )
		return;
	bIsSimulating = false;
	UE_LOG ( LogCrowdSimulator, Log, TEXT ( "CSSubsystem: PauseSimulation requested. " ) );
	for ( const auto & Agent : GetAgents() )
		Agent -> PauseLogic ();

	for ( const auto & NavPoint : GetNavigationPoints() )
	{
		ACSAgentSpawner * Spawner = Cast<ACSAgentSpawner> ( NavPoint ); 
		if ( Spawner )
		{
			Spawner -> StopSpawning(); 
		}
	}
	OnSimulationPausedDelegate . Broadcast();
	UE_LOG ( LogCrowdSimulator, Log, TEXT ( "CSSubsystem: Paused simulation." ) )
}


void UCSSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	const APlayerController * PlayerController = GetWorld() -> GetFirstPlayerController();
	if ( !PlayerController )
	{
		UE_LOG ( LogCrowdSimulator, Warning, TEXT ( "CSSubsystem::BeginPlay() => Invalid player controller. " ) );
		return;
	}
	const APawn * ControlledPawn = PlayerController -> GetPawn();
	if ( !ControlledPawn )
	{
		UE_LOG ( LogCrowdSimulator, Warning, TEXT ( "CSSubsystem::BeginPlay() => Invalid player pawn. " ) );
		return;
	}
	UCSPlayerUIComponent * PlayerUIComponent = Cast <UCSPlayerUIComponent> ( ControlledPawn -> GetComponentByClass( UCSPlayerUIComponent::StaticClass() ) );
	if ( !IsValid ( PlayerUIComponent ) )
	{
		UE_LOG ( LogCrowdSimulator, Warning, TEXT ( "CSSubsystem::BeginPlay() => Invalid player UI component. " ) );
		return; 
	}
	PlayerUIComponent -> OnSimulationStartedDelegate.AddDynamic( this, &UCSSubsystem::StartSimulation );
	PlayerUIComponent -> OnSimulationStoppedDelegate.AddDynamic( this, &UCSSubsystem::StopSimulation );
	PlayerUIComponent -> OnSimulationPausedDelegate.AddDynamic (this, &UCSSubsystem::PauseSimulation); 
}

TStatId UCSSubsystem::GetStatId() const
{
	return TStatId(); 
}

bool UCSSubsystem::RegisterNavigationPoint ( ACSNavigationPoint* NavigationPoint)
{
	if ( !IsValid ( NavigationPoint ) )
		return false;
	
	if ( GetIsNavigationPointRegistered( NavigationPoint -> GetHandle() ) )
		return false;
	
	ACSNavigationSystem * NavZone = GetOrCreateNavigationSystem();
	if ( !NavZone )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "Can't registern navigation point: %s . Can't access navigation zone"), *NavigationPoint -> GetName() );
		return false;
	}
	const int32 Handle = GetNavigationPoints () . Add ( NavigationPoint );
	NavigationPoint -> SetHandle ( Handle );
	NavZone -> AddNavigationPoint( Handle ); 
	UE_LOG ( LogCrowdSimulator, Log, TEXT ( "Registered new navigation point: %s"), *NavigationPoint -> GetName() );
	return true; 
}

bool UCSSubsystem::RegisterNavigationPointWithHandle(ACSNavigationPoint* NavigationPoint, int32 Handle)
{
	if ( !IsValid ( NavigationPoint ) )
		return false;
	if ( GetIsNavigationPointRegistered( NavigationPoint -> GetHandle() )
		|| GetIsNavigationPointRegistered( Handle ) )
		return false;
	ACSNavigationSystem * NavSystem = GetOrCreateNavigationSystem();
	if ( !IsValid ( NavSystem ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "Can't registern navigation point: %s . Can't access navigation zone"), *NavigationPoint -> GetName() );
		return false;
	}
	if ( GetNavigationPoints() . Num() <= Handle )
		GetNavigationPoints() . Reserve ( Handle );
	GetNavigationPoints() . Insert ( Handle, NavigationPoint );
	NavigationPoint -> SetHandle ( Handle );
	NavSystem -> AddNavigationPoint ( Handle ); 
	UE_LOG ( LogCrowdSimulator, Log, TEXT ( "Registered new navigation point: %s"), *NavigationPoint -> GetName() );
	return true;
}

bool UCSSubsystem::UnregisterNavigationPoint(int32 NavPointHandle)
{
	if ( !GetIsNavigationPointRegistered( NavPointHandle ) )
		return false ;
	ACSNavigationPoint * NavPoint = GetNavigationPoint( NavPointHandle );
	ACSNavigationSystem * NavZone = GetNavigationSystem();
	if ( !NavZone )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT( "UCSSubsystem::UnregisterNavigationPoint => Navzone doesn't exitsts for group: %d"), NavPoint -> GetGoalGroupId() );
		return false; 
	}
	if ( ! NavZone -> RemoveNavigationPoint( NavPointHandle ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT( "UCSSubsystem::UnregisterNavigationPoint => Navzone doesn't contain navpoint: %s"), *NavPoint -> GetName() );
		return false;
	}
	NavPoint -> SetHandle ( CS_INVALID_HANDLE );
	UE_LOG ( LogCrowdSimulator, Log, TEXT ( "Unregistered navigation point: %s"), *GetNavigationPoint ( NavPointHandle ) -> GetName () );
	Objects.NavigationPoints.RemoveAt ( NavPointHandle );
	return true; 
}


bool UCSSubsystem::RegisterAgent(ACSAgent* Agent)
{
	if ( !IsValid ( Agent ) )
		return false ;

	if ( GetIsAgentRegistered( Agent -> GetHandle () ) )
		return false;

	Agent -> OnStartedFollowingPath . AddDynamic ( this, &UCSSubsystem::OnAgentStartedFollowingPath );
	Agent -> OnFinishedFollowingPath . AddDynamic ( this, &UCSSubsystem::OnAgentFinishedFollowingPath ); 
	
	const int32 Handle = Objects.Agents.Add ( Agent );
	Agent -> SetHandle ( Handle ); 
	if ( bIsSimulating )
	{
		if ( ! Agent -> GetIsRunningLogic() )
			Agent -> StartLogic(); 
	}
	UE_LOG ( LogCrowdSimulator, Log, TEXT ( "Registered new agent: %s"), *Agent -> GetName()); 
	return true; 
}

bool UCSSubsystem::RegisterAgentWithHandle(ACSAgent* Agent, int32 Handle)
{
	if ( !IsValid ( Agent ) )
		return false ;

	if ( GetIsAgentRegistered( Agent -> GetHandle () )
		|| GetIsAgentRegistered( Handle ) ) 
		return false;

	Agent -> OnStartedFollowingPath . AddDynamic ( this, &UCSSubsystem::OnAgentStartedFollowingPath );
	Agent -> OnFinishedFollowingPath . AddDynamic ( this, &UCSSubsystem::OnAgentFinishedFollowingPath ); 

	if ( GetAgents() . Num() <= Handle )
		GetAgents() . Reserve ( Handle );
	
	GetAgents() . Insert ( Handle, Agent ); 
	Agent -> SetHandle ( Handle ); 
	if ( bIsSimulating )
	{
		if ( ! Agent -> GetIsRunningLogic() )
			Agent -> StartLogic(); 
	}
	UE_LOG ( LogCrowdSimulator, Log, TEXT ( "Registered new agent: %s"), *Agent -> GetName()); 
	return true; 
}

bool UCSSubsystem::UnregisterAgent(int32 AgentHandle)
{
	if ( !GetIsAgentRegistered( AgentHandle ) )
		return false;

	ACSAgent * Agent = GetAgent ( AgentHandle );
	Agent -> SetHandle( CS_INVALID_HANDLE );
	GetAgents().RemoveAt( AgentHandle );
	UE_LOG ( LogCrowdSimulator, Log, TEXT ( "Unregistered agent: %s" ), * Agent -> GetName() ); 
	return true; 
}

UCSNavigationPath * UCSSubsystem::FindPath(int32 AgentHandle, EGoalSearchType GoalSearchType )
{
	if ( !GetIsAgentRegistered( AgentHandle ) )
		return {};
	const ACSAgent * Agent = GetAgent ( AgentHandle );
	const int32 AgentGroupId = Agent -> GetGroupId(); 
	ACSNavigationSystem * NavigationSystem = GetNavigationSystem ();
	if ( !NavigationSystem )
	{
		UE_LOG ( LogCrowdSimulator, Warning, TEXT ("UCSSubsystem::FindPath() => Can't find path for agent: %s . Reason: no navigation zone"), * Agent -> GetName() );
		return {};
	}
	TArray <int32> NavigationPoints;
	int Goal;
	
	switch ( GoalSearchType )
	{
		case EGoalSearchType::Closest:
		{
			switch ( GetSimulationMode() )
			{
				case Normal:
				{
					Goal = NavigationSystem -> GetClosestGoalToLocation( AgentGroupId, Agent -> GetActorLocation());
					break;
				}
				case Evacuation:
				{
					Goal = NavigationSystem -> GetClosestEvacuationGoalToLocation( AgentGroupId, Agent -> GetActorLocation());
					break; 		
				}
				default:
				{
					UE_LOG ( LogCrowdSimulator,	Error, TEXT ("UCSSubsystem::FindPath() => Unrecognized SimulationMode: %d"), GetSimulationMode() );
					return nullptr; 
				}
				
			}
			break;
		}
		case EGoalSearchType::Random:
		{
			switch ( GetSimulationMode() )
			{
				case Normal:
				{
					Goal = NavigationSystem -> GetRandomGoal ( AgentGroupId );
					break;  
				}
				case Evacuation:
				{
					Goal = NavigationSystem -> GetRandomEvacuationGoal ( AgentGroupId );
					break; 
				}
				default:
				{
					UE_LOG ( LogCrowdSimulator,	Error, TEXT ("UCSSubsystem::FindPath() => Unrecognized SimulationMode: %d"), GetSimulationMode() );
					return nullptr; 
				}
				
			}
			break; 
		}
		default:
		{
			UE_LOG ( LogCrowdSimulator,	Error, TEXT ("UCSSubsystem::FindPath() => Unrecognized EGoalSearchType: %d"), GoalSearchType );
			Goal = NavigationSystem -> GetRandomGoal( AgentGroupId );
			break; 
		}
	}
	const int32 ClosestPoint = NavigationSystem -> GetClosestNavigationPointToLocation( Agent -> GetActorLocation() );
	NavigationSystem -> FindPath ( ClosestPoint, Goal, NavigationPoints );
	UCSNavigationPath * Path = NewObject<UCSNavigationPath> ( this );
	if ( !IsValid ( Path ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ("UCSSubsystem::FindPath() => Can't create UCSNavigationPath object" ) )
		return nullptr;
	}
	Path -> SetNavigationPointIndex( 0 );
	Path -> SetNavigationPoints ( NavigationPoints );
	Path -> SetGoalHandle( Goal ); 
	return Path;  
}

void UCSSubsystem::UpdatePath(UCSNavigationPath* Path)
{
	if ( !IsValid ( Path ) )
	{
		UE_LOG ( LogCrowdSimulator, Warning, TEXT ( "UCSSubsystem::UpdatePath() => Can't updatep path. Invalid path is provided.") );
		return;
	}
	TArray<int32> UpdatedNavigationPoints = Path -> GetNavigationPoints(); 
	TArray<int32> PartialPath;
	for ( int i = UpdatedNavigationPoints.Num() - 1; i >= Path -> GetCurrentIndex(); i -- )
	{
		UpdatedNavigationPoints.RemoveAt( i ); 
	}
	// Find new path from current to end
	GetOrCreateNavigationSystem() -> FindPath ( Path -> GetCurrentNavigationPoint(), Path -> GetGoalHandle(), PartialPath );
	// Append new path to the cut old
	UpdatedNavigationPoints.Append ( PartialPath );
	Path -> SetNavigationPoints ( UpdatedNavigationPoints );
}

ACSNavigationSystem* UCSSubsystem::GetOrCreateNavigationSystem ()
{
	ACSNavigationSystem * NavigationSystem = GetNavigationSystem();
	if ( NavigationSystem )
		return NavigationSystem;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	NavigationSystem = GetWorld() -> SpawnActor <ACSNavigationSystem> ( NavigationSystemClass, FTransform::Identity, SpawnParameters);
	if ( !IsValid ( NavigationSystem ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "UCSSubsystem::GetOrCreateNavigationSystem() :: can't create navigation system." ) );
		return nullptr; 
	}
	Objects.NavigationSystem = NavigationSystem;  
	return NavigationSystem; 
}

bool UCSSubsystem::RemoveNavigationPath(int32 NavPointHandle1, int32 NavPointHandle2)
{
	if ( ! GetIsNavigationPointRegistered( NavPointHandle1 ) || ! GetIsNavigationPointRegistered( NavPointHandle2 ) )
		return false;
	if ( !GetIsPathExists( NavPointHandle1, NavPointHandle2 ) )
		return false;
	;
	ACSNavigationSystem * NavigationSystem = GetNavigationSystem();
	if ( ! NavigationSystem )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "UCSSubsystem::RemoveNavigationPath. Navigation points are registered but navigation zone is invalid " ) );
		return false; 
	}
	return NavigationSystem -> RemovePath ( NavPointHandle1, NavPointHandle2 ); 
}

bool UCSSubsystem::AddNavigationPath(int32 NavPointHandle1, int32 NavPointHandle2)
{
	if ( ! GetIsNavigationPointRegistered( NavPointHandle1 ) || ! GetIsNavigationPointRegistered( NavPointHandle2 ) )
		return false;
	
	if ( GetIsPathExists( NavPointHandle1, NavPointHandle2 ) )
		return false;
	
	ACSNavigationSystem * NavigationSystem = GetNavigationSystem();
	return NavigationSystem -> AddPath( NavPointHandle1, NavPointHandle2 ); 
}

bool UCSSubsystem::GetIsPathExists(int32 NavPointHandle1, int32 NavPointHandle2) const
{
	if ( !GetIsNavigationPointRegistered( NavPointHandle1 ) || ! GetIsNavigationPointRegistered( NavPointHandle2 ) )
		return false;
	ACSNavigationSystem * NavigationSystem = GetNavigationSystem ();
	return NavigationSystem -> GetIsPathExists( NavPointHandle1, NavPointHandle2 );
}

bool UCSSubsystem::GetIsNavigationPathExists(int32 NavPointHandle1, int NavPointHandle2) const
{
	if ( !GetNavigationSystem() )
		return false;
	return GetNavigationSystem()->GetIsNavigationPathExists( NavPointHandle1, NavPointHandle2 ); 
}

bool UCSSubsystem::GetIsSimulating() const 
{
	return bIsSimulating; 
}

void UCSSubsystem::SetNavigationGraphVisibility(bool Visibility)
{
	bIsNavigationGraphVisible = Visibility;
	UpdateNavigationVisibility();
}

void UCSSubsystem::SetNavigationPointsVisibility(bool Visibility)
{
	bIsNavigationPointsVisible = Visibility;
	UpdateNavigationVisibility(); 
}

bool UCSSubsystem::GetNavigationGraphVisibility() const
{
	return bIsNavigationGraphVisible; 
}

bool UCSSubsystem::GetNavigationPointsVisibility() const
{
	return bIsNavigationPointsVisible; 
}

const TSparseArray<ACSAgent*>& UCSSubsystem::GetAgents() const
{
	return Objects.Agents; 
}

float UCSSubsystem::GetNavigationDensityFactor() const
{
	return NavigationDensityFactor; 
}

float UCSSubsystem::GetNavigationMaxDensityFactor() const
{
	return NavigationMaxDensityFactor; 
}

void UCSSubsystem::SetNavigationDensityFactor(float DensityFactor)
{
	NavigationDensityFactor = DensityFactor; 
}

bool UCSSubsystem::GetAgentsReconsiderPaths() const
{
	return bAgentsReconsiderPaths; 
}

void UCSSubsystem::SetAgentsReconsiderPaths(bool AgentsReconsiderPaths)
{
	bAgentsReconsiderPaths = AgentsReconsiderPaths;
	const FString Suffix = bAgentsReconsiderPaths ? "On" : "Off"; 
	OnSimulationMessageDelegate.Broadcast ("Switched agents reconsider paths to: " + Suffix ); 
}

bool UCSSubsystem::ValidateEvacuationNavigation() const
{
	if ( !IsValid ( GetNavigationSystem() ) )
	{
		OnSimulationErrorMessageDelegate.Broadcast( "Invalid navigation. Evacuation can't be started. No navigation points was found."); 
		return false;
	}

	// Each agent should be able to navigate to all of its goals 
	for ( const auto Agent : GetAgents() )
	{
		TArray<int32> Goals; 
		GetGroupEvacuationGoals ( Agent -> GetGroupId(), Goals );
		if ( Goals.IsEmpty() )
		{
			OnSimulationErrorMessageDelegate.Broadcast("Invalid navigation. Evacuation can't be started. Agent from group: " + FString::FromInt ( Agent -> GetGroupId() ) + " doesn't have any evacuation goal."); 
			return false; 
		}
		for ( const auto Goal : Goals )
		{
			const auto ClosestNavPointHandle = GetNavigationSystem() -> GetClosestNavigationPointToLocation( Agent -> GetActorLocation() );
			const auto ClosestNavPoint = GetNavigationPoint( ClosestNavPointHandle ); 
			if ( ! ClosestNavPoint-> GetIsEvacuationPoint() &&  !GetIsNavigationPathExists( ClosestNavPointHandle, Goal ) )
			{
				OnSimulationErrorMessageDelegate.Broadcast( "Invalid navigation. Evacuation can't be started. Agent from group: " + FString::FromInt ( Agent -> GetGroupId() ) + FString (" can't reach all evacuation goals.") ); 
				return false;
			}
		} 
	}
	// Each spawner should lead to each of the agent group goals
	for ( const auto NavigationPoint : Objects.NavigationPoints )
	{
		const ACSAgentSpawner * AgentSpawner = Cast <ACSAgentSpawner> ( NavigationPoint );
		if ( !AgentSpawner )
			continue;
		TArray<int32> Goals;
		GetGroupEvacuationGoals ( AgentSpawner -> GetAgentsGroupId(), Goals );
		if ( Goals . IsEmpty() )
		{
			OnSimulationErrorMessageDelegate.Broadcast( "Invalid navigation. Evacuation can't be started. Agent spawner with group: " + FString::FromInt ( AgentSpawner -> GetAgentsGroupId() ) + FString (" doesn't have any goals") );
			return false; 
		}
		for ( const auto Goal : Goals )
		{
			const auto GoalNavPoint = GetNavigationPoint ( Goal ); 
			if  ( ! GoalNavPoint -> GetIsEvacuationPoint() && !GetIsNavigationPathExists ( AgentSpawner -> GetHandle(), Goal ) )
			{
				OnSimulationErrorMessageDelegate.Broadcast( "Invalid navigation. Evacuation can't be started. Agent spawner with group: " + FString::FromInt ( AgentSpawner -> GetAgentsGroupId() ) + FString (" can't reach all evacuation goals.") ); 
				return false;; 
			}
		}
	}
	return true ;
}

void UCSSubsystem::SetSimulationMode(ESimulationMode NewSimulationMode)
{
	if ( GetSimulationMode() == NewSimulationMode )
		return;
	if ( !GetIsSimulating() )
	{
		OnSimulationErrorMessageDelegate.Broadcast ("Can't switch simulation mode. Simulation hasn't started.");
		return; 
	}
	switch ( NewSimulationMode )
	{
		case ESimulationMode::Normal:
		{
			EndEvacuation();
			break; 
		}
		case ESimulationMode::Evacuation:
		{
			if ( ValidateEvacuationNavigation() ) 
				StartEvacuation();
			break; 
		}
		default:
		{
			UE_LOG ( LogCrowdSimulator, Warning, TEXT ("UCSSubsystem::SetSimulationMode => unrecogznized simulation mode: %d" ), NewSimulationMode );
			break; 
		}
	}
	FString SimulationModeString = SimulationMode == ESimulationMode::Evacuation ? "Evacuation" : "Default"; 
	OnSimulationMessageDelegate.Broadcast ("Switched simulation mode to: " + SimulationModeString ); 
}

void UCSSubsystem::SetEvacuationGroupId(int32 GroupId)
{
	EvacuationGroupId = GroupId; 
}

ESimulationMode UCSSubsystem::GetSimulationMode() const
{
	return SimulationMode; 
}

void UCSSubsystem::GetNavigationPoints(TArray<int32>& OutPoints, const TFunction<bool(ACSNavigationPoint*)>& Filter) const
{
	OutPoints.Empty();
	for ( const auto NavPoint : GetNavigationPoints() )
	{
		if ( Filter ( NavPoint ) )
			OutPoints.Push ( NavPoint -> GetHandle() ); 
	}
}

void UCSSubsystem::GetGroupEvacuationGoals( int32 GroupId, TArray<int32>& OutGoals) const
{
	auto Filter = [ GroupId ] ( ACSNavigationPoint * NavPoint )
	{
		return NavPoint -> GetIsEvacuationPoint()
			&& NavPoint -> GetIsGoalPoint()
			&& NavPoint -> GetGoalGroupId() == GroupId; 
	};
	return GetNavigationPoints ( OutGoals, Filter ); 
}

void UCSSubsystem::GetGroupNonEvacuationGoals( int32 GroupId, TArray<int32>& OutGoals) const
{
	auto Filter = [ GroupId ] ( const ACSNavigationPoint * NavPoint )
	{
		return !NavPoint -> GetIsEvacuationPoint()
			&& NavPoint -> GetIsGoalPoint()
			&& NavPoint -> GetGoalGroupId() == GroupId; 
	};
	return GetNavigationPoints ( OutGoals, Filter ); 
}

void UCSSubsystem::SetNavigationDrawMode(ECSNavigationSystemDrawMode DrawMode)
{
	if ( !GetNavigationSystem() )
	{
		OnSimulationErrorMessageDelegate.Broadcast ("Can't switch navigation draw mode. Navigation system is invalid." ); 
		return;
	}
	GetNavigationSystem() -> SetDrawMode( DrawMode );
	const FString DrawModeString = DrawMode == ECSNavigationSystemDrawMode::Default ? "Default" : "Heat Map"; 
	OnSimulationMessageDelegate.Broadcast( "Switched draw mode to: " + DrawModeString + "." ); 
}

ECSNavigationSystemDrawMode UCSSubsystem::GetNavigationDrawMode() const
{
	if ( !GetNavigationSystem() )
		return ECSNavigationSystemDrawMode::Default;
	return GetNavigationSystem() -> GetDrawMode(); 
}

void UCSSubsystem::UpdateNavigationVisibility()
{
	if ( !GetNavigationSystem() )
	{
		OnSimulationErrorMessageDelegate.Broadcast ("Can't switch navigation visibility. Navigation system is invalid." ); 
		return;
	}
	GetNavigationSystem() -> SetActorHiddenInGame ( !bIsNavigationGraphVisible );
	for ( const auto NavigationPoint : GetNavigationPoints() )
	{
		NavigationPoint -> SetActorHiddenInGame ( !bIsNavigationPointsVisible ); 
	}
}

const TSparseArray<ACSNavigationPoint*>& UCSSubsystem::GetNavigationPoints() const
{
	return Objects.NavigationPoints;
}

float UCSSubsystem::GetNavigationRandomFactor() const
{
	return NavigationRandomFactor; 
}

float UCSSubsystem::GetNavigationMaxRandomFactor() const
{
	return NavigationMaxRandomFactor; 
}

void UCSSubsystem::SetNavigationRandomFactor(float RandomFactor)
{
	NavigationRandomFactor = RandomFactor; 
}

FCSSaveGameContainer UCSSubsystem::GetLevelSaveData() const
{
	FCSSaveGameContainer SaveGameContainer;
	// Fetch all navigation points parameters 
	for ( const auto NavigationPoint : GetNavigationPoints() )
	{
		FCSNavigationPointParameters NavigationPointParameters = NavigationPoint -> GetNavigationPointParameters ();
		if ( ACSAgentSpawner * AgentSpawner = Cast <ACSAgentSpawner> ( NavigationPoint ) )
		{
			FCSAgentSpawnerParameters AgentSpawnerParameters = AgentSpawner -> GetAgentSpawnerParameters ();
			SaveGameContainer.AgentSpawnerParameters . Push ( AgentSpawnerParameters );
			continue; 
		}
		SaveGameContainer.NavigationPointsParameters . Push ( NavigationPointParameters ); 
	}
	// Fetch all agents parameters
	for ( const auto Agent : GetAgents() )
	{
		FCSAgentParameters AgentParameters = Agent -> GetAgentParameters();
		SaveGameContainer . AgentParameters . Push ( AgentParameters ); 
	}
	return SaveGameContainer; 
}

void UCSSubsystem::LoadLevelFromSaveData(const FCSSaveGameContainer& LevelData)
{
	const auto PlayerPawn = UGameplayStatics::GetPlayerPawn( this, 0 );
	// Spawn and register all navigation points 
	for ( const auto NavigationPointParameters : LevelData.NavigationPointsParameters )
	{
		const auto NavigationPoint = GetWorld() -> SpawnActorDeferred<ACSNavigationPoint>( NavigationPointClass,
		                                                                                   NavigationPointParameters.SpawnTransform,
		                                                                                   PlayerPawn,
		                                                                                   PlayerPawn,
		                                                                                   ESpawnActorCollisionHandlingMethod::AlwaysSpawn );
		if ( !NavigationPoint )
		{
			UE_LOG ( LogCrowdSimulator, Warning, TEXT ( "UCSSubsystem::LoadLevelFromSaveData() => Can't spawn navigation point from save data" ) );
		}
		RegisterNavigationPointWithHandle ( NavigationPoint, NavigationPointParameters.Handle );
		NavigationPoint -> SetupPointFromParameters( NavigationPointParameters );
		NavigationPoint -> FinishSpawning( NavigationPointParameters . SpawnTransform ); 
	}
	// Spawn and register all agent spawners 
	for ( const auto & AgentSpawnerParameters : LevelData.AgentSpawnerParameters )
	{
		const auto AgentSpawner = GetWorld() -> SpawnActorDeferred<ACSAgentSpawner>( AgentSpawnerClass,
		                                                                             AgentSpawnerParameters.NavigationPointParameters.SpawnTransform,
		                                                                             PlayerPawn,
		                                                                             PlayerPawn,
		                                                                             ESpawnActorCollisionHandlingMethod::AlwaysSpawn );
		if ( !AgentSpawner )
		{
			UE_LOG ( LogCrowdSimulator, Warning, TEXT ( "UCSSubsystem::LoadLevelFromSaveData() => Can't spawn agent spawner from save data" ) );
		}
		RegisterNavigationPointWithHandle ( AgentSpawner, AgentSpawnerParameters.NavigationPointParameters.Handle );
		AgentSpawner -> SetupSpawnerFromParameters ( AgentSpawnerParameters );
		AgentSpawner -> FinishSpawning( AgentSpawnerParameters . NavigationPointParameters . SpawnTransform ); 
	}
	// Spawn and register all agents
	for ( const auto & AgentParameters : LevelData.AgentParameters )
	{
		const auto Agent = GetWorld() -> SpawnActorDeferred<ACSAgent>( AgentClass,
		                                                               AgentParameters.SpawnTransform,
		                                                               PlayerPawn,
		                                                               PlayerPawn,
		                                                               ESpawnActorCollisionHandlingMethod::AlwaysSpawn );
		if ( !Agent )
		{
			UE_LOG ( LogCrowdSimulator, Warning, TEXT ( "UCSSubsystem::LoadLevelFromSaveData() => Can't spawn agent from save data" ) );
		}
		RegisterAgent ( Agent );
		Agent -> SetupFromParameters ( AgentParameters );
		Agent -> SpawnDefaultController();
		Agent -> FinishSpawning( AgentParameters.SpawnTransform ); 
	}

	for ( const auto & NavigationPointParameters : LevelData.NavigationPointsParameters )
	{
		for ( const auto & Path : NavigationPointParameters . Paths )
		{
			AddNavigationPath( NavigationPointParameters.Handle, Path );
		}
	}
	for ( const auto & AgentSpawnerParameters : LevelData.AgentSpawnerParameters )
	{
		for ( const auto & Path : AgentSpawnerParameters . NavigationPointParameters . Paths )
		{
			AddNavigationPath( AgentSpawnerParameters . NavigationPointParameters. Handle, Path );
		}
	}
}

TArray<int32> UCSSubsystem::GetNavigationPointPaths(int32 NavigationPointHandle) const
{
	const ACSNavigationSystem * NavSystem = GetNavigationSystem();
	if ( !NavSystem || !GetIsNavigationPointRegistered( NavigationPointHandle ) )
	{
		return {};
	}
	return NavSystem -> GetNavigationPointPaths( NavigationPointHandle ); 
}

bool UCSSubsystem::ValidateNavigation() const
{
	if ( !IsValid ( GetNavigationSystem() ) )
	{
		OnSimulationErrorMessageDelegate.Broadcast ("Invalid navigation. No navigation points was found."); 
		return false;
	}

	// Each agent should be able to navigate to all of its goals 
	for ( const auto Agent : GetAgents() )
	{
		TArray<int32> Goals; 
		GetGroupNonEvacuationGoals ( Agent -> GetGroupId(), Goals );
		if ( Goals.IsEmpty() )
		{
			OnSimulationErrorMessageDelegate.Broadcast( "Invalid navigation. Agent from group: " + FString::FromInt ( Agent -> GetGroupId() ) + FString (" doesn't have any goals.") ); 
			return false; 
		}
		for ( const auto Goal : Goals )
		{
			const auto ClosestNavPointHandle = GetNavigationSystem() -> GetClosestNavigationPointToLocation( Agent -> GetActorLocation() );
			const auto ClosestNavPoint = GetNavigationPoint( ClosestNavPointHandle ); 
			if ( ! ClosestNavPoint-> GetIsEvacuationPoint() &&  !GetIsNavigationPathExists( ClosestNavPointHandle, Goal ) )
			{
				OnSimulationErrorMessageDelegate.Broadcast( "Invalid navigation. Agent from group: " + FString::FromInt ( Agent -> GetGroupId() ) + FString (" can't reach all goals.") ); 
				return false;
			}
		} 
	}
	// Each spawner should lead to each of the agent group goals
	for ( const auto NavigationPoint : Objects.NavigationPoints )
	{
		const ACSAgentSpawner * AgentSpawner = Cast <ACSAgentSpawner> ( NavigationPoint );
		if ( !AgentSpawner )
			continue;
		TArray<int32> Goals;
		GetGroupNonEvacuationGoals ( AgentSpawner -> GetAgentsGroupId(), Goals );
		if ( Goals . IsEmpty() )
		{
			OnSimulationErrorMessageDelegate.Broadcast( "Invalid navigation. Agent spawner from group: " + FString::FromInt ( AgentSpawner -> GetAgentsGroupId() ) + FString (" doesn't have any goals.") ); 
			return false; 
		}
		for ( const auto Goal : Goals )
		{
			const auto GoalNavPoint = GetNavigationPoint ( Goal ); 
			if  ( ! GoalNavPoint -> GetIsEvacuationPoint() && !GetIsNavigationPathExists ( AgentSpawner -> GetHandle(), Goal ) )
			{
				OnSimulationErrorMessageDelegate.Broadcast( "Invalid navigation. Agent spawner with group: " + FString::FromInt ( AgentSpawner -> GetAgentsGroupId() ) + FString (" can't reach all goals.") ); 
				return false;; 
			}
		}
	}
	return true;
}

void UCSSubsystem::StartEvacuation()
{
	for ( const auto NavPoint : GetNavigationPoints() )
	{
		ACSAgentSpawner * Spawner = Cast <ACSAgentSpawner> ( NavPoint );
		if ( !Spawner )
			continue;
		Spawner -> StopSpawning(); 
	}

	for ( const auto Agent : GetAgents() )
	{
		Agent -> StopLogic();
		Agent -> SetSpeed ( Agent -> GetSpeed() * EvacuationSpeedMultiplier ); 
		Agent -> StartLogic(); 
	}
	SimulationMode = ESimulationMode::Evacuation;
	OnEvacuationStartedDelegate.Broadcast();
}

void UCSSubsystem::EndEvacuation()
{
	SimulationMode = ESimulationMode::Normal;
	for ( const auto NavPoint : Objects . NavigationPoints )
	{
		ACSAgentSpawner * Spawner = Cast <ACSAgentSpawner> ( NavPoint );
		if ( !Spawner )
			continue;
		Spawner -> StartSpawning(); 
	}
	
	for ( const auto Agent : Objects . Agents )
	{
		Agent -> StopLogic ();
		Agent -> SetSpeed ( Agent -> GetSpeed() / EvacuationSpeedMultiplier ); 
		Agent -> StartLogic(); 
	}
	
	OnEvacuationEndedDelegate.Broadcast(); 
}

void UCSSubsystem::OnAgentStartedFollowingPath( int32 NavPointHandle1, int32 NavPointHandle2)
{
	if ( !GetNavigationSystem() )
		return; 
	GetNavigationSystem() -> OnAgentStartedFollowingPath( NavPointHandle1, NavPointHandle2 );
}

void UCSSubsystem::OnAgentFinishedFollowingPath( int32 NavPointHandle1, int32 NavPointHandle2 )
{
	if ( !GetNavigationSystem() )
		return; 
	GetNavigationSystem() -> OnAgentFinishedFollowingPath(  NavPointHandle1, NavPointHandle2 );

}

ACSNavigationSystem* UCSSubsystem::GetNavigationSystem() const
{
	return Objects.NavigationSystem; 
}

TSparseArray<ACSAgent*>& UCSSubsystem::GetAgents()
{
	return Objects.Agents; 
}

TSparseArray<ACSNavigationPoint*>& UCSSubsystem::GetNavigationPoints()
{
	return Objects.NavigationPoints; 
}

bool UCSSubsystem::GetIsAgentRegistered(int32 AgentHandle) const
{
	return IsValidCSHandle( AgentHandle ) && GetAgents().IsValidIndex ( AgentHandle ); 	
}


bool UCSSubsystem::GetIsNavigationPointRegistered( int32 NavPointHandle ) const
{
	return IsValidCSHandle( NavPointHandle ) && GetNavigationPoints().IsValidIndex( NavPointHandle ); 
}

