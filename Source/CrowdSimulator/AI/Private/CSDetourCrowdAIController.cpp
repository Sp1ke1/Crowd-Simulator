// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSimulator/AI/Public/CSDetourCrowdAIController.h"
#include "BrainComponent.h"
#include "Navigation/CrowdFollowingComponent.h"


ACSDetourCrowdAIController::ACSDetourCrowdAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	PrimaryActorTick.bCanEverTick = true; 
}

// Called when the game starts or when spawned
void ACSDetourCrowdAIController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACSDetourCrowdAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACSDetourCrowdAIController::StartAgentLogic_Implementation()
{
	RunBehaviorTree( BTAsset );
}

void ACSDetourCrowdAIController::PauseAgentLogic()
{
	GetBrainComponent() -> PauseLogic( "Stop logic called" ); 
}

void ACSDetourCrowdAIController::StopAgentLogic_Implementation()
{
	GetBrainComponent() -> StopLogic( "Pause logic called" );
}

bool ACSDetourCrowdAIController::GetIsRunningAgentLogic() const
{
	const UBrainComponent * BrainComp = GetBrainComponent();
	if ( IsValid ( BrainComp ) )
	{
		return BrainComp -> IsRunning(); 
	}
	return false;
}

