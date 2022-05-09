// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "DrawDebugHelpers.h"


// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);

	PlayerInputComponent->BindAction("Escape", IE_Pressed, this, &AGoKart::QuitGame);

}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	
}

FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "none";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "ERROR";
	}
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateRotation(DeltaTime);
	UpdateLocationFromVelocity(DeltaTime); 
	

	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(GetLocalRole()), this, FColor::Green, DeltaTime);

}

void AGoKart::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle;
	Force += GetAirResistance();
	Force += GetRollingResistance();
	FVector Acceleration = Force / Mass;

	Velocity += Acceleration * DeltaTime;

	FHitResult HitResult;
	FVector Translation = Velocity * 100 * DeltaTime;
	AddActorWorldOffset(Translation, true, &HitResult);

	if (HitResult.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}

	FString SpeedString = FString::SanitizeFloat(Velocity.Size());
	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("SPEED: " + SpeedString));
}

void AGoKart::UpdateRotation(float DeltaTime)
{
	float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = DeltaLocation / MinTurningRadius * Steering;
	FQuat DeltaRotation(GetActorUpVector(), RotationAngle);

	Velocity = DeltaRotation.RotateVector(Velocity);	

	AddActorWorldRotation(DeltaRotation);
}

FVector AGoKart::GetAirResistance()
{
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

FVector AGoKart::GetRollingResistance()
{
	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100;
	float NormalForce = Mass * AccelerationDueToGravity;
	return -Velocity.GetSafeNormal() * NormalForce * RollingResistanceCoefficient;
}

void AGoKart::MoveForward(float Value)
{
	Throttle = Value;
	Server_MoveForward(Value);
}

void AGoKart::Server_MoveForward_Implementation(float Value)
{
	Throttle = Value;
}

bool AGoKart::Server_MoveForward_Validate(float Value)
{
	
	return FMath::Abs(Value) <= 1 ;
}


void AGoKart::MoveRight(float Value)
{
	Steering = Value;
	Server_MoveRight(Value);
}

void AGoKart::Server_MoveRight_Implementation(float Value)
{
	Steering = Value;
}

bool AGoKart::Server_MoveRight_Validate(float Value)
{
	return FMath::Abs(Value) <= 1;
}

void AGoKart::QuitGame()
{
	GEngine->GetFirstLocalPlayerController(GetWorld())->ConsoleCommand("quit");
}

