// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSimulator/AI/Public/CSAgent.h"
#include "CrowdSimulator/AI/Public/CSDetourCrowdAIController.h"
#include "CrowdSimulator/AI/Public/CSSubsystem.h"
#include "CrowdSimulator/Core/Public/CSUtilities.h"
#include "GameFramework/CharacterMovementComponent.h"
// Sets default values
ACSAgent::ACSAgent()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = ACSDetourCrowdAIController::StaticClass();
}

int32 ACSAgent::GetHandle() const
{
	return Handle; 
}

void ACSAgent::SetSpeed(float Speed)
{
	GetCharacterMovement() -> MaxWalkSpeed = Speed; 
}

float ACSAgent::GetSpeed() const
{
	return GetCharacterMovement() -> MaxWalkSpeed;
}

const FCSAgentSpeedParameters& ACSAgent::GetSpeedParameters() const
{
	return SpeedParameters;
}

void ACSAgent::SetSpeedParameters(const FCSAgentSpeedParameters& AgentSpeedParameters)
{
	SpeedParameters = AgentSpeedParameters;
}


// Called when the game starts or when spawned
void ACSAgent::BeginPlay()
{
	Super::BeginPlay();
	GetWorld() -> GetSubsystem<UCSSubsystem>() -> RegisterAgent ( this ); 	
}

void ACSAgent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if ( IsValid ( GetWorld () ) )
	{
		const auto CSSubsystem = GetWorld() -> GetSubsystem<UCSSubsystem>();
		if ( IsValid ( CSSubsystem ))
		{
			CSSubsystem -> UnregisterAgent ( GetHandle() ); 
		}
	}
}

void ACSAgent::SetHandle(int32 NewHandle)
{
	Handle = NewHandle;
}

void ACSAgent::SetGroupId(int Id)
{
	GroupId = Id; 
}

void ACSAgent::SetAge(TEnumAsByte<ECSAgentAge> NewAge)
{
	Age = NewAge;  
}

TEnumAsByte<ECSAgentAge> ACSAgent::GetAge() const
{
	return Age; 
}

void ACSAgent::SetupFromParameters(const FCSAgentParameters& AgentParameters)
{
	SetAge ( AgentParameters.Age ); 
	SetGroupId( AgentParameters.GroupId );
	SetSpeedParameters ( AgentParameters.AgentSpeedParameters );
	UpdateMovementSpeed();
	if ( AgentParameters . HasSpawnTransform )
		SetActorTransform ( AgentParameters . SpawnTransform );
}

FCSAgentParameters ACSAgent::GetAgentParameters() const
{
	FCSAgentParameters AgentParameters;
	AgentParameters.Age = GetAge();
	AgentParameters.GroupId = GetGroupId();
	AgentParameters.SpawnTransform = GetActorTransform();
	AgentParameters.AgentSpeedParameters = GetSpeedParameters();
	AgentParameters.HasSpawnTransform = true;
	return AgentParameters;
}

ECSAgentMovementType ACSAgent::GetMovementType() const
{
	return MovementType; 
}

void ACSAgent::UpdateMovementSpeed()
{
	switch ( MovementType )
	{
		case ECSAgentMovementType::Walk:
		{
			SetSpeed( GetSpeedParameters() . WalkSpeedMap[ GetAge() ] );  
		}
		case ECSAgentMovementType::Run:
		{
			SetSpeed ( GetSpeedParameters() . RunSpeedMap [ GetAge () ] ); 
		}
		default:
		{
			UE_LOG ( LogCrowdSimulator, Warning, TEXT ("ACSAGent::UpdateMovementSpeed() => Unknown movemnet type." ) );
			SetSpeed ( GetSpeedParameters() . WalkSpeedMap [ GetAge () ] ); 
		}
	}
	
}

void ACSAgent::SetMovementType(ECSAgentMovementType NewMovementType)
{
	if ( MovementType == NewMovementType )
		return; 
	MovementType = NewMovementType;
	UpdateMovementSpeed();
}

// Called every frame
void ACSAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACSAgent::SetHighlight(ECSNavigationPointSpriteType HighlightType)
{
	if ( HighlightType == ECSNavigationPointSpriteType::DestroySprite && ! IsValid ( HighlightMaterial ) )
	{
		UE_LOG ( LogCrowdSimulator, Warning, TEXT( "ACSAGent::SetHighlight. Invalid highlight material. Can't highlight" ) );
		return; 
	}

	if ( DefaultMaterials . IsEmpty() )
	{
		for ( auto i = 0; i < GetMesh() -> GetNumMaterials(); i ++ )
		{
			DefaultMaterials . Push ( GetMesh() -> GetMaterial ( i ) ); 
		}
	}
	
	const auto N = GetMesh() -> GetNumMaterials(); 
	for ( auto i = 0; i < N; i ++ )
	{
		UMaterialInterface * Material =  HighlightType == ECSNavigationPointSpriteType::DestroySprite ? HighlightMaterial : DefaultMaterials [ i ];
		GetMesh() -> SetMaterial ( i, Material ); 
	}
}
int ACSAgent::GetGroupId() const
{
	return GroupId; 
}

bool ACSAgent::GetIsRunningLogic() const
{
	ACSDetourCrowdAIController * AIController = Cast <ACSDetourCrowdAIController> ( GetController() );
	if ( !IsValid ( AIController ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "ACSAgent::GetIsRunningLogic (). Can't start logic. Owning controller doesn't derive from ACSDetourCrowdAIControllerCLass" ) );
		return false; 	
	}
	return AIController -> GetIsRunningAgentLogic();
}

void ACSAgent::StartLogic()
{
	ACSDetourCrowdAIController * AIController = Cast <ACSDetourCrowdAIController> ( GetController() );
	if ( !IsValid ( AIController ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "ACSAgent::StartLogic (). Can't start logic. Owning controller doesn't derive from ACSDetourCrowdAIControllerCLass" ) );
		return; 	
	}
	AIController -> StartAgentLogic();
}

void ACSAgent::StopLogic()
{
	ACSDetourCrowdAIController * AIController = Cast <ACSDetourCrowdAIController> ( GetController() );
	if ( !IsValid ( AIController ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "ACSAgent::StartLogic (). Can't start logic. Owning controller doesn't derive from ACSDetourCrowdAIControllerCLass" ) );
		return; 	
	}
	AIController -> StopAgentLogic(); 
}

void ACSAgent::PauseLogic()
{
	ACSDetourCrowdAIController * AIController = Cast <ACSDetourCrowdAIController> ( GetController() );
	if ( !IsValid ( AIController ) )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ( "ACSAgent::StartLogic (). Can't start logic. Owning controller doesn't derive from ACSDetourCrowdAIControllerCLass" ) );
		return; 	
	}
	AIController -> PauseAgentLogic(); 
}

