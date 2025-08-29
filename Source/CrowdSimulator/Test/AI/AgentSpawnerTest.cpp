#include "CrowdSimulator/AI/Public/CSTypes.h"
#include "CrowdSimulator/AI/Public/CSSubsystem.h"
#include "CrowdSimulator/Test/WorldFixture.h"
#include "CrowdSimulator/AI/Public/CSAgentSpawner.h"
#include <thread>
#include <chrono>
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(AgentSpawnerTest, "CrowdSimulator.AI.AgentSpawnerTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)


bool HasSpawnedValidAgent = false; 

void OnAgentSpawned ( ACSAgent * Agent )
{
	if ( !Agent || Agent -> GetGroupId() != 1 )
	{
		HasSpawnedValidAgent = false; 
		return; 
	}
	else
	{
		HasSpawnedValidAgent = true;
		return;
	}
}

bool AgentSpawnerTest::RunTest(const FString& Parameters)
{
	FWorldFixture WorldFixture;
	UWorld * World = WorldFixture.GetWorld();
	UCSSubsystem * Subsystem = World -> GetSubsystem<UCSSubsystem>(); 
	FCSAgentSpawnerParameters AgentSpawnerParameters;
	AgentSpawnerParameters.SpawnerSize = { 50.f, 50.f };
	AgentSpawnerParameters.SpawnRate = 0.5f;
	AgentSpawnerParameters.AgentsGroupId = 1; 

	
	std::chrono::seconds PauseTime ( 1 );
	ACSAgentSpawner * AgentSpawner = World -> SpawnActorDeferred<ACSAgentSpawner>( ACSAgentSpawner::StaticClass(), FTransform::Identity );
	TestTrue ( "Default agent spawner has spawned.", AgentSpawner != nullptr );
	AgentSpawner -> SetupSpawnerFromParameters( AgentSpawnerParameters );
	TestTrue ( "Initialized with correct spawn rate.", AgentSpawner -> GetSpawnRate() == 0.5f );
	TestTrue ("Initialized with correct agents group id.", AgentSpawner -> GetAgentsGroupId() == 1 );
	AgentSpawner -> FinishSpawning( FTransform::Identity );
	TestTrue ( "Registered in the subsystem.", Subsystem -> GetIsNavigationPointRegistered( AgentSpawner -> GetHandle() ) );
	TestTrue ("Can be unregistered from the subsystem.", Subsystem -> UnregisterNavigationPoint( AgentSpawner -> GetHandle() ) );
	TestFalse ("Not reachable after unregistration", Subsystem -> GetIsNavigationPointRegistered( AgentSpawner -> GetHandle ()) );
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
