// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle;
	FVector Acceleration = Force / Mass;
	Velocity += Acceleration * DeltaTime;

	FVector Translation = Velocity * 100 * DeltaTime;
	AddActorWorldOffset(Translation);

	FString Speed = FString::SanitizeFloat(Velocity.X);
	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("SPEED: " + Speed));

}

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);

	PlayerInputComponent->BindAction("Escape", IE_Pressed, this, &AGoKart::QuitGame);

}

void AGoKart::MoveForward(float Value)
{
	Throttle = Value;
}

void AGoKart::QuitGame()
{
	GEngine->GetFirstLocalPlayerController(GetWorld())->ConsoleCommand("quit");
}


