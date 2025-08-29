// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdSimulator/Player/Public/Interactions/Previews/CSNavigationPathPreview.h"
#include "Components/LineBatchComponent.h"


// Sets default values
ACSNavigationPathPreview::ACSNavigationPathPreview()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LineBatchComponent = CreateDefaultSubobject<ULineBatchComponent>( "LineBatchComponent" );
}

// Called when the game starts or when spawned
void ACSNavigationPathPreview::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACSNavigationPathPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACSNavigationPathPreview::SetPathPoints(const FVector& P1, const FVector& P2, const FColor & Color )
{
	Clear(); 
	GetLineBatchComponent() -> DrawLine( P1, P2, Color.ReinterpretAsLinear(), DepthPriority, Thickness, Lifetime );
}

void ACSNavigationPathPreview::Clear()
{
	GetLineBatchComponent() -> Flush(); 
}

ULineBatchComponent* ACSNavigationPathPreview::GetLineBatchComponent() const
{
	return LineBatchComponent; 
}

