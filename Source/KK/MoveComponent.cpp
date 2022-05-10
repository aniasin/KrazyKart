// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveComponent.h"
#include "GameFramework/GameStateBase.h"

// Sets default values for this component's properties
UMoveComponent::UMoveComponent()
{

}

// Called when the game starts
void UMoveComponent::BeginPlay()
{
	Super::BeginPlay();

}

FGoKartMove UMoveComponent::CreateMove(float DeltaTime)
{
	FGoKartMove Move;
	Move.DeltaTime = DeltaTime;
	Move.Steering = Steering;
	Move.Throttle = Throttle;
	Move.Time = GetOwner()->GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

	return Move;
}
void UMoveComponent::ClearAknowledgedMoves(FGoKartMove LastMove)
{
	TArray<FGoKartMove> NewMoves;

	for (const FGoKartMove& Move : UnaknowledgedMoves)
	{
		if (Move.Time > LastMove.Time)
		{
			NewMoves.Add(Move);
		}
	}
	UnaknowledgedMoves = NewMoves;
}

void UMoveComponent::Move(float Value)
{
	Throttle = Value;
}

void UMoveComponent::MoveRight(float Value)
{
	Steering = Value;
}

void UMoveComponent::SimulateMove(FGoKartMove Move, FVector ServerVelocity)
{
	UpdateVelocity(Move);
	UpdateRotation(Move);
	ManageCollision(Move, ServerVelocity);
}

void UMoveComponent::UpdateVelocity(FGoKartMove Move)
{
	FVector Force = GetOwner()->GetActorForwardVector() * MaxDrivingForce * Move.Throttle;
	Force += GetAirResistance();
	Force += GetRollingResistance();
	FVector Acceleration = Force / Mass;

	Velocity += Acceleration * Move.DeltaTime;

	FString SpeedString = FString::SanitizeFloat(Velocity.Size());
	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("SPEED: " + SpeedString));
}

void UMoveComponent::UpdateRotation(FGoKartMove Move)
{
	float DeltaLocation = FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity) * Move.DeltaTime;
	float RotationAngle = DeltaLocation / MinTurningRadius * Move.Steering;
	FQuat DeltaRotation(GetOwner()->GetActorUpVector(), RotationAngle);

	Velocity = DeltaRotation.RotateVector(Velocity);

	GetOwner()->AddActorWorldRotation(DeltaRotation);
}

void UMoveComponent::ManageCollision(FGoKartMove Move, FVector ServerVelocity)
{
	FHitResult HitResult;
	FVector Translation = ServerVelocity * 100 * Move.DeltaTime;
	GetOwner()->AddActorWorldOffset(Translation, true, &HitResult);

	if (HitResult.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}
}

FVector UMoveComponent::GetVelocity()
{
	return Velocity;
}

TArray<FGoKartMove> UMoveComponent::GetUnaknowledgedMoves()
{
	return UnaknowledgedMoves;
}

void UMoveComponent::AddUnaknowledgedMove(FGoKartMove Move)
{
	UnaknowledgedMoves.Add(Move);
}

void UMoveComponent::SetVelocity(FVector NewVelocity)
{
	Velocity = NewVelocity;
}

FVector UMoveComponent::GetAirResistance()
{
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

FVector UMoveComponent::GetRollingResistance()
{
	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100;
	float NormalForce = Mass * AccelerationDueToGravity;
	return -Velocity.GetSafeNormal() * NormalForce * RollingResistanceCoefficient;
}


