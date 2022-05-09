// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"


// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

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

	if (HasAuthority())
	{
		NetUpdateFrequency = 1;
	}
	
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
	
	if (IsLocallyControlled())
	{
		FGoKartMove Move;
		Move.DeltaTime = DeltaTime;
		Move.Steering = Steering;
		Move.Throttle = Throttle;
		//TODO Set time

		Server_SendMove(Move);
	}

	UpdateRotation(DeltaTime);
	UpdateLocationFromVelocity(DeltaTime);

	if (HasAuthority())
	{
		ServerState.Transform = GetActorTransform();
		ServerState.Velocity = Velocity;
		//TODO Update LastMove
	}
	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(GetLocalRole()), this, FColor::Green, DeltaTime);
}

void AGoKart::OnRep_ServerState()
{
	SetActorTransform(ServerState.Transform);
	Velocity = ServerState.Velocity;
}

void AGoKart::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle;
	Force += GetAirResistance();
	Force += GetRollingResistance();
	FVector Acceleration = Force / Mass;

	Velocity += Acceleration * DeltaTime;

	FHitResult HitResult;
	FVector Translation = ServerState.Velocity * 100 * DeltaTime;
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

void AGoKart::Server_SendMove_Implementation(FGoKartMove Move)
{
	Throttle = Move.Throttle;
	Steering = Move.Steering;
}

bool AGoKart::Server_SendMove_Validate(FGoKartMove Move)
{
	return true; //TODO make better validation
}

void AGoKart::MoveForward(float Value)
{
	Throttle = Value;
}

void AGoKart::MoveRight(float Value)
{
	Steering = Value;
}

void AGoKart::QuitGame()
{
	GEngine->GetFirstLocalPlayerController(GetWorld())->ConsoleCommand("quit");
}

void AGoKart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGoKart, ServerState);
	DOREPLIFETIME(AGoKart, Throttle);
	DOREPLIFETIME(AGoKart, Steering);
}
