// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSimulator/AI/Public/CSAgentSpawner.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "CrowdSimulator/AI/Public/CSAgent.h"
#include "CrowdSimulator/AI/Public/CSSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void ACSAgentSpawner::SetSpawnerExtent(FVector2d Extent)
{
	GetCollisionComponent() -> SetBoxExtent( { Extent.X, Extent.Y, 5.0f } );
}

FCSAgentParameters ACSAgentSpawner::GenerateAgentParameters() const
{
	FCSAgentParameters AgentParameters;
	AgentParameters.Age = AgeDistribution.GetAgentAge();
	AgentParameters.GroupId = AgentsGroupId;
	AgentParameters.HasSpawnTransform = true;
	AgentParameters.SpawnTransform = FTransform ( UKismetMathLibrary::RandomPointInBoundingBox( GetActorLocation(), GetCollisionComponent()->GetScaledBoxExtent() ) );
	return AgentParameters;

}

float ACSAgentSpawner::GetSpawnRate() const
{
	return SpawnRate;
}

FCSAgentSpawnerParameters ACSAgentSpawner::GetAgentSpawnerParameters()
{
	FCSAgentSpawnerParameters AgentSpawnerParameters;
	AgentSpawnerParameters.NavigationPointParameters = Super::GetNavigationPointParameters();
	AgentSpawnerParameters.AgeDistribution = GetAgeDistribution();
	AgentSpawnerParameters.SpawnerSize = { GetCollisionComponent()->GetScaledBoxExtent().X, GetCollisionComponent() -> GetScaledBoxExtent().Y };
	AgentSpawnerParameters.AgentsGroupId = GetAgentsGroupId();
	AgentSpawnerParameters.SpawnRate = GetSpawnRate();
	return AgentSpawnerParameters; 
}

ACSAgentSpawner::ACSAgentSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCollisionComponent() -> SetHiddenInGame( false ); 
}

void ACSAgentSpawner::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ACSAgentSpawner::SetSpawnRate(float spawnRate)
{
	SpawnRate = spawnRate; 
}

void ACSAgentSpawner::SetAgeDistribution(const FAgentSpawnerAgeDistribution& NewDistribution)
{
	AgeDistribution = NewDistribution;
}

const FAgentSpawnerAgeDistribution& ACSAgentSpawner::GetAgeDistribution() const
{
	return AgeDistribution; 
}

void ACSAgentSpawner::StartSpawning()
{
	GetWorldTimerManager().SetTimer ( SpawnTimerHandle, this, &ACSAgentSpawner::SpawnAgent, SpawnRate, true );
}

void ACSAgentSpawner::StopSpawning()
{
	GetWorldTimerManager().ClearTimer( SpawnTimerHandle ); 
}

int32 ACSAgentSpawner::GetAgentsGroupId() const
{
	return AgentsGroupId; 
}

void ACSAgentSpawner::SetAgentsGroupId(int32 GroupId)
{
	AgentsGroupId = GroupId; 
}

void ACSAgentSpawner::SetupSpawnerFromParameters(const FCSAgentSpawnerParameters& Parameters)
{
	Super::SetupPointFromParameters( Parameters.NavigationPointParameters );
	SetAgeDistribution( Parameters.AgeDistribution );
	SetSpawnRate ( Parameters.SpawnRate );
	SetAgentsGroupId( Parameters.AgentsGroupId );
	SetSpawnerExtent( Parameters.SpawnerSize );
}

void ACSAgentSpawner::BeginPlay()
{
	Super::BeginPlay();
	const UCSSubsystem * Subsystem = GetWorld() -> GetSubsystem<UCSSubsystem>();
	if ( Subsystem -> GetIsSimulating() )
	{
		StartSpawning(); 
	}
}

void ACSAgentSpawner::SpawnAgent()
{
	if ( !GetWorld () )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "ACSAgentSpawner => SpawnAgent(). Can't spawn new agent. Invalid world." ) );
		return; 
	}

	const auto Subsystem = GetWorld() -> GetSubsystem<UCSSubsystem>();
	if ( ! IsValid ( Subsystem ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "ACSAgentSpawner => SpawnAgent(). Can't spawn new agent. Invalid subsystem." ) );
		return; 
	}
	APawn * PlayerPawn = UGameplayStatics::GetPlayerPawn( this, 0);
	const auto AgentParameters = GenerateAgentParameters(); 
	ACSAgent * SpawnedAgent = GetWorld() -> SpawnActorDeferred <ACSAgent> ( AgentClassToSpawn, AgentParameters.SpawnTransform, this, PlayerPawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn );
	const FVector SpawnOffset = { 0.f, 0.f, SpawnedAgent -> GetCapsuleComponent() -> GetScaledCapsuleHalfHeight() }; 
	if ( ! IsValid ( SpawnedAgent ) )
	{
		UE_LOG (LogCrowdSimulator, Error, TEXT ("ACSAgentSpawner => SpawnAgent() => Can't spawn new agent" ) );
		return; 
	}
	
	SpawnedAgent -> SetupFromParameters( AgentParameters );
	SpawnedAgent -> AddActorLocalOffset( SpawnOffset ); 
	SpawnedAgent -> SpawnDefaultController();
	SpawnedAgent -> FinishSpawning( AgentParameters.SpawnTransform );
	OnAgentSpawnedDelegate.Broadcast( SpawnedAgent ); 
	UE_LOG ( LogCrowdSimulator, Log, TEXT ( "ACSAgentSpawner:: Spawned new agent" ) );
}

