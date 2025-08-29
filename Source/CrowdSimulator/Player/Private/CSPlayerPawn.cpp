// Fill out your copyright notice in the Description page of Project Settings.



#include "CrowdSimulator/Player/Public/CSPlayerPawn.h"
#include "CrowdSimulator/UI/Public/CSPlayerUIComponent.h"
#include "CrowdSimulator/Player/Public/Interactions/States/CSInteractionState.h"
#include "CrowdSimulator/Core/Public/CSCore.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


// Sets default values
ACSPlayerPawn::ACSPlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UIComponent = CreateDefaultSubobject<UCSPlayerUIComponent> ( "UIComponent" );
}

// Called when the game starts or when spawned
void ACSPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	if (const APlayerController * PlayerController = Cast<APlayerController> ( Controller ) )
	{
		if ( UEnhancedInputLocalPlayerSubsystem * Subsystem = ULocalPlayer::GetSubsystem <UEnhancedInputLocalPlayerSubsystem> ( PlayerController -> GetLocalPlayer() ) )
		{
			Subsystem -> AddMappingContext( MappingContext, 0 );
		}
	}

	UIComponent -> OnInteractionModeSelectedDelegate . AddDynamic ( this, &ACSPlayerPawn::OnInteractionModeChanged );
}


void ACSPlayerPawn::OnInteractionModeChanged( TSubclassOf <UCSInteractionState> InteractionStateClass )
{
	if ( IsValid ( InteractionState )  )
	{
		InteractionState -> OnDestroy();
		InteractionState -> ConditionalBeginDestroy();
		InteractionState = nullptr; 
	}
	InteractionState = NewObject<UCSInteractionState> ( this, InteractionStateClass );
	if ( IsValid ( InteractionState ) )
		InteractionState -> Initialize(); 
	
}

void ACSPlayerPawn::Interact(const FInputActionValue& Value)
{
	if ( IsValid ( InteractionState ) )
	{
		InteractionState -> OnInteraction(); 
	}
}

void ACSPlayerPawn::Move(const FInputActionValue& Value)
{
	if ( !Controller )
		return;

	const FVector MovementVector = Value.Get<FVector>();
	const FRotator ControllerRotation = Controller -> GetControlRotation();

	// To move over world X / Y 
	// const FRotator YawRotation (0, ControllerRotation.Yaw, 0 );

	
	// Get forward vector
	
	// const FVector ForwardDirection = FRotationMatrix ( ControllerRotation ) . GetUnitAxis( EAxis::X );

	const FRotator ControllerRotationNoPitch = { 0.f,ControllerRotation.Yaw ,  ControllerRotation.Roll };
	const FVector ForwardDirection = ControllerRotationNoPitch.RotateVector( FVector::ForwardVector );
	const FVector RightDirection = FRotationMatrix ( ControllerRotation ) . GetUnitAxis( EAxis::Y ); 
	const FVector UpDirection = FVector::UpVector; 
	
	AddMovementInput( ForwardDirection, MovementVector.X );
	AddMovementInput( RightDirection, MovementVector.Y );
	AddMovementInput( UpDirection, MovementVector.Z );
}
void ACSPlayerPawn::Look(const FInputActionValue& Value)
{
	if ( ! Controller )
		return;
	const float LookAxisVector = Value.Get<float>();
	AddControllerYawInput( LookAxisVector );
}


// Called every frame
void ACSPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if ( IsValid ( InteractionState ) )
	{
		InteractionState -> Update ( DeltaTime ); 
	}
}

// Called to bind functionality to input
void ACSPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if ( UEnhancedInputComponent * EnhancedInputComponent = CastChecked <UEnhancedInputComponent> ( PlayerInputComponent ) )
	{
		EnhancedInputComponent -> BindAction( MoveAction, ETriggerEvent::Triggered, this, &ACSPlayerPawn::Move );
		EnhancedInputComponent -> BindAction( LookAction, ETriggerEvent::Triggered, this, &ACSPlayerPawn::Look );
		EnhancedInputComponent -> BindAction( InteractAction, ETriggerEvent::Completed, this, &ACSPlayerPawn::Interact );
		return; 
	}
	UE_LOG ( LogCrowdSimulator, Error, TEXT( "PlayerInputComponent in ACrowdSimulaotorPawn is not derived from UEnhancedInputComponent" ) ); 
}

UCSPlayerUIComponent* ACSPlayerPawn::GetUIComponent() const
{

	return UIComponent;
}
