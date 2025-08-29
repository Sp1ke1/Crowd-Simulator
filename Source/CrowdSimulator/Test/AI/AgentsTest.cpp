#include "CrowdSimulator/AI/Public/CSAgentSpawner.h"
#include "CrowdSimulator/AI/Public/CSAgent.h"
#include "CrowdSimulator/AI/Public/CSSubsystem.h"
#include "CrowdSimulator/AI/Public/CSTypes.h"
#include "Misc/AutomationTest.h"
#include "CrowdSimulator/Test/WorldFixture.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(AgentsTest, "CrowdSimulator.AI.AgentsTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool AgentsTest::RunTest(const FString& Parameters)
{

	FWorldFixture WorldFixture;
	UWorld * World = WorldFixture.GetWorld();
	UCSSubsystem * Subsystem = World -> GetSubsystem<UCSSubsystem>(); 
	FCSAgentParameters AgentParameters;
	AgentParameters.Age = ECSAgentAge::Adult;
	AgentParameters.GroupId = 1;

	ACSAgent * Agent = World -> SpawnActorDeferred<ACSAgent>( ACSAgent::StaticClass(), FTransform::Identity );
	
	TestTrue ( "Default agent has spawned", Agent != nullptr ); 
	Agent -> SetupFromParameters( AgentParameters );
	Agent -> FinishSpawning( FTransform::Identity );

	TestTrue ( "Agent age is correct.", Agent -> GetAge() == ECSAgentAge::Adult );
	TestTrue ( "Agent group id is correct.", Agent -> GetGroupId() == 1 );
	TestTrue ( "Agent is registered in the subsystem", Subsystem -> GetIsAgentRegistered( Agent -> GetHandle() ) ); 
	TestTrue ( "Agent can be get from the subsystem", Subsystem -> GetAgent( Agent -> GetHandle() ) != nullptr );
	TestTrue ( "Agent can be unregistered.", Subsystem -> UnregisterAgent( Agent -> GetHandle() ) ); 
	TestTrue ( "Agent is not registered after unregistration", Subsystem -> GetIsAgentRegistered( Agent -> GetHandle() ) == false );
	
	Agent -> Destroy();
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
