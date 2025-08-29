// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSimulator/AI/Public/CSNavigationPoint.h"
#include "CrowdSimulator/AI/Public/CSSubsystem.h"
#include "CrowdSimulator/AI/Public/CSTypes.h"
#include "Components/BoxComponent.h"

// Sets default values
ACSNavigationPoint::ACSNavigationPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoxCollision = CreateDefaultSubobject<UBoxComponent> ( "BoxCollision ");
	SetRootComponent( BoxCollision );
}

// Called when the game starts or when spawned
void ACSNavigationPoint::BeginPlay()
{
	Super::BeginPlay();
	if ( IsValid ( GetWorld() ) )
	{
		UCSSubsystem * CSSubsystem = GetWorld() -> GetSubsystem<UCSSubsystem> ();
		if ( IsValid ( CSSubsystem ) )
		{
			SetActorHiddenInGame( !CSSubsystem -> GetNavigationGraphVisibility() ); 
			CSSubsystem -> RegisterNavigationPoint( this ); 
		}
	}
	SetDefaultSprite(); 
}


int ACSNavigationPoint::GetGoalGroupId() const
{
	return GoalGroupId; 
}

void ACSNavigationPoint::SetSprite_Implementation(ECSNavigationPointSpriteType SpriteType)
{
	
}

bool ACSNavigationPoint::GetIsEvacuationPoint() const
{
	return bIsEvacuationPoint;
}

void ACSNavigationPoint::SetIsEvacuationPoint(bool IsEvacuationPoint)
{
	bIsEvacuationPoint = IsEvacuationPoint;
}

void ACSNavigationPoint::SetupPointFromParameters(const FCSNavigationPointParameters& Parameters)
{
	SetIsEvacuationPoint( Parameters.bIsEvacuation );
	SetIsGoalPoint( Parameters.bIsGoal );
	SetGoalGroupId( Parameters.GoalGroupId );
	if ( Parameters.HasSpawnTransform ) 
		SetActorTransform ( Parameters.SpawnTransform ); 
}

void ACSNavigationPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if ( IsValid ( GetWorld() ) )
	{
		UCSSubsystem *  CSSubsystem = GetWorld() -> GetSubsystem<UCSSubsystem>();
		if ( IsValid ( CSSubsystem )  )
		{
			CSSubsystem -> UnregisterNavigationPoint ( GetHandle() );
		}
	}
}

bool ACSNavigationPoint::GetIsGoalPoint() const
{
	return IsGoal; 
}

UBoxComponent* ACSNavigationPoint::GetCollisionComponent() const
{
	return BoxCollision; 
}

void ACSNavigationPoint::SetIsGoalPoint(bool isGoal)
{
	IsGoal = isGoal;
}

int32 ACSNavigationPoint::GetHandle() const
{
	return NavPointHandle; 
}

void ACSNavigationPoint::SetDefaultSprite()
{
	ECSNavigationPointSpriteType SpriteType;
	if ( GetIsGoalPoint() && GetIsEvacuationPoint() )
		SpriteType = ECSNavigationPointSpriteType::EvacuationGoalSprite;
	else if ( GetIsGoalPoint() )
		SpriteType = ECSNavigationPointSpriteType::GoalSprite;
	else if ( GetIsEvacuationPoint() )
		SpriteType = ECSNavigationPointSpriteType::EvacuationSprite;
	else
		SpriteType = ECSNavigationPointSpriteType::DefaultSprite;
	SetSprite( SpriteType ); 
}

void ACSNavigationPoint::SetHandle(int32 Handle)
{
	NavPointHandle = Handle; 
}
void ACSNavigationPoint::SetGoalGroupId(int Id)
{
	GoalGroupId = Id; 
}

const FCSNavigationPointSpriteData & ACSNavigationPoint::GetSpritesData() const
{
	return SpritesData; 
}

FCSNavigationPointParameters ACSNavigationPoint::GetNavigationPointParameters() const
{
	FCSNavigationPointParameters NavigationPointParameters;
	NavigationPointParameters.Handle = GetHandle();
	NavigationPointParameters.bIsEvacuation = GetIsEvacuationPoint();
	NavigationPointParameters.bIsGoal = GetIsGoalPoint();
	NavigationPointParameters.SpawnTransform = GetActorTransform();
	NavigationPointParameters.GoalGroupId = GetGoalGroupId();
	NavigationPointParameters.HasSpawnTransform = true;
	const UCSSubsystem * CSSubsystem = GetWorld() -> GetSubsystem<UCSSubsystem>();
	if ( !CSSubsystem )
	{
		UE_LOG ( LogCrowdSimulator, Error, TEXT ("ACSNavigationPoint::GetNavigationPointParameters() => Error. Invalid CSSubsystem. " ) );
	}
	else
	{
		NavigationPointParameters.Paths = CSSubsystem -> GetNavigationPointPaths ( GetHandle() ); 	
	}
	return NavigationPointParameters; 
}

// Called every frame
void ACSNavigationPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

