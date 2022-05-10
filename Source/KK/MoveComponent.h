// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MoveComponent.generated.h"

USTRUCT()
struct FGoKartMove
{
	GENERATED_BODY()

		UPROPERTY()
		float DeltaTime;
	UPROPERTY()
		float Time;
	UPROPERTY()
		float Throttle;
	UPROPERTY()
		float Steering;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KK_API UMoveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMoveComponent();

	UPROPERTY(EditAnywhere)
		float Mass = 1000;
	UPROPERTY(EditAnywhere)
		float DragCoefficient = 1;
	UPROPERTY(EditAnywhere)
		float MaxDrivingForce = 10000;
	UPROPERTY(EditAnywhere)
		float MinTurningRadius = 10;
	UPROPERTY(EditAnywhere)
		float RollingResistanceCoefficient = 0.015;

	FGoKartMove CreateMove(float DeltaTime);
	void SimulateMove(FGoKartMove Move, FVector ServerVelocity);

	void UpdateVelocity(FGoKartMove Move);
	void UpdateRotation(FGoKartMove Move);
	void ManageCollision(FGoKartMove Move, FVector ServerVelocity);

	FVector GetVelocity();
	TArray<FGoKartMove> GetUnaknowledgedMoves();
	void AddUnaknowledgedMove(FGoKartMove Move);
	void ClearAknowledgedMoves(FGoKartMove LastMove);
	void SetVelocity(FVector NewVelocity);

	void Move(float Value);
	void MoveRight(float Value);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	FVector Velocity;
	float Steering;
	float Throttle;

	TArray<FGoKartMove> UnaknowledgedMoves;

	FVector GetAirResistance();
	FVector GetRollingResistance();

		
};
