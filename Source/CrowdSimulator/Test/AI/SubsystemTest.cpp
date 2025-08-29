#include "CrowdSimulator/AI/Public/CSNavigationPath.h"
#include "CrowdSimulator/Test/WorldFixture.h"
#include "CrowdSimulator/AI/Public/CSSubsystem.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST( SubsystemTest, "CrowdSimulator.AI.SubsystemTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)


bool SubsystemTest::RunTest(const FString& Parameters)
{
	const FWorldFixture WorldFixture;
	UWorld * World = WorldFixture.GetWorld();

	UCSSubsystem * Subsystem = World -> GetSubsystem<UCSSubsystem> ();
	ACSAgent * P1 = World -> SpawnActorDeferred<ACSAgent> ( ACSAgent::StaticClass(), FTransform::Identity);
	ACSAgent * P2 = World -> SpawnActorDeferred<ACSAgent> ( ACSAgent::StaticClass(), FTransform::Identity );
	FCSAgentParameters AgentParameters;
	AgentParameters.GroupId = 0;
	P1 -> SetupFromParameters( AgentParameters );
	P2 -> SetupFromParameters( AgentParameters );
	P1 -> FinishSpawning( FTransform::Identity );
	P2 -> FinishSpawning( FTransform::Identity ); 

	TestTrue ( "Agents are registered after spawn", Subsystem -> GetAgent( P1 -> GetHandle() ) != nullptr
																&& Subsystem -> GetAgent ( P2 -> GetHandle () ) != nullptr );
	FCSNavigationPointParameters Parameters1;
	Parameters1.GoalGroupId = 0;
	Parameters1.bIsGoal = true;
	ACSNavigationPoint * Point1 = World -> SpawnActorDeferred<ACSNavigationPoint> ( ACSNavigationPoint::StaticClass(), FTransform::Identity );
	ACSNavigationPoint * Point2 = World -> SpawnActorDeferred<ACSNavigationPoint> ( ACSNavigationPoint::StaticClass(), FTransform::Identity );
	Point1 -> SetupPointFromParameters( Parameters1 ); 
	Point1 -> FinishSpawning( FTransform::Identity );
	Point2 -> FinishSpawning( FTransform::Identity );

	int32 Handle1 = Point1 -> GetHandle();
	int32 Handle2 = Point2 -> GetHandle(); 
	
	TestTrue ("Points are registered after spawning", Subsystem -> GetNavigationPoint( Point1 -> GetHandle () )
																&& Subsystem -> GetNavigationPoint( Point2 -> GetHandle() ) );
	Subsystem -> AddNavigationPath( Point1 -> GetHandle(), Point2 -> GetHandle() );
	UCSNavigationPath * Path = Subsystem -> FindPath ( P1 -> GetHandle(), EGoalSearchType::Random );
	TestTrue ("Found valid navigation path.", Path != nullptr );
	TestTrue ("Deeper test of navigation path validity. ", Path -> GetIsValid() );
	UCSNavigationPath * InvalidPath = Subsystem -> FindPath ( 228, EGoalSearchType::Closest );
	TestFalse ( "Invalid path is invalid", InvalidPath != nullptr );
	TestTrue ( "Path exists.", Subsystem -> GetIsPathExists( Handle1, Handle2 ) );
	TestFalse ( "Invalid path doesn't exist", Subsystem -> GetIsPathExists( Handle1, CS_INVALID_HANDLE ) ); 
	return true; 
}
