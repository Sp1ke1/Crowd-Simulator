#include "CrowdSimulator/AI/Public/CSNavigationGraph.h"
#include "CrowdSimulator/Test/WorldFixture.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(NavigationGraphTest, "CrowdSimulator.AI.NavigationGraphTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool NavigationGraphTest::RunTest(const FString& Parameters)
{
	FWorldFixture WorldFixture;
	UWorld * World = WorldFixture . GetWorld();
	FCSNavigationGraph g ( World );
	g.AddVertex( 1 );
	g.AddVertex( 2 );

	TestTrue( "Has vertices after adding.", g.GetIsValidVertex( 1 ) && g.GetIsValidVertex( 2 ) );
	TestFalse ("Doesn't have path if doesn't have path", g.GetIsValidEdge( 1, 2 ) );
	g.AddEdge( 1, 2 );
	TestTrue ( "Has path when added.", g.GetIsValidEdge( 1, 2 ) );
	TestTrue ( "Is connective when connective.", g.GetIsConnective() );
	g.RemoveEdge( 1, 2  );
	TestFalse ("Is not connective when connectivity removed", g.GetIsConnective() );
	TestFalse ("Doesn't have edge after removal", g.GetIsValidEdge( 1, 2 ) );
	TestTrue ( "Has correct number of vertices." , g.GetNumberOfVertices() == 2 ); 
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
