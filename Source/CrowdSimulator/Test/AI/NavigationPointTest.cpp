#include "CrowdSimulator/AI/Public/CSNavigationPoint.h"
#include "CrowdSimulator/AI/Public/CSSubsystem.h"
#include "CrowdSimulator/Test/WorldFixture.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(NavigationPointTest, "CrowdSimulator.AI.NavigationPointTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool NavigationPointTest::RunTest(const FString& Parameters)
{
	FWorldFixture WorldFixture;
	UWorld * World = WorldFixture.GetWorld();
	UCSSubsystem * Subsystem = World -> GetSubsystem<UCSSubsystem> ();
	ACSNavigationPoint * P1 = World -> SpawnActor<ACSNavigationPoint> ();
	ACSNavigationPoint * P2 = World -> SpawnActor<ACSNavigationPoint> ();
	const int32 Handle1 = P1 -> GetHandle();
	const int32 Handle2 = P2 -> GetHandle();
	TestTrue ( "Navigation points were spawned.", P1 != nullptr && P2 != nullptr );

	TestTrue ( "Navigation points handles are valid.", Subsystem -> GetIsNavigationPointRegistered( Handle1 )
																&& Subsystem -> GetIsNavigationPointRegistered( Handle2 ) );
	TestFalse ( "Path doesn't exist before adding", Subsystem -> GetIsNavigationPathExists( Handle1, Handle2 ) );

	TestTrue ( "Added valid navigation path", Subsystem -> AddNavigationPath( Handle1 , Handle2 ) );
	TestTrue ( "Path exists after adding.", Subsystem -> GetIsNavigationPathExists( Handle1, Handle2 ) );
	Subsystem -> RemoveNavigationPath( Handle1, Handle2 );
	TestFalse ( "Path doesn't exists after removing", Subsystem -> GetIsNavigationPathExists( Handle1, Handle2 ) );
	TestFalse ( "Registration fail of already registered point", Subsystem -> RegisterNavigationPoint( P1 ) );

	P1 -> Destroy();
	P2 -> Destroy(); 
	return true; 
}
