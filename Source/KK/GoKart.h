// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"


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

USTRUCT()
struct FGoKartState
{
	GENERATED_BODY()

	UPROPERTY()
	float DeltaTime;
	UPROPERTY()
	FGoKartMove LastMove;
	UPROPERTY()
	FVector Velocity;
	UPROPERTY()
	FTransform Transform;
};

UCLASS()
class KK_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

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

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FGoKartMove Move);

	void QuitGame();

private:	
	
	FVector Velocity;
	float Throttle;
	float Steering;

	TArray<FGoKartMove> UnaknowledgedMoves;

	UPROPERTY(ReplicatedUsing=OnRep_ServerState)
	FGoKartState ServerState;

	UFUNCTION()
	void OnRep_ServerState();

	void UpdateLocation(FGoKartMove Move);
	void UpdateRotation(FGoKartMove Move);
	FGoKartMove CreateMove(float DeltaTime);
	void ClearAknowledgedMoves(FGoKartMove LastMove);
	void SimulateMove(const FGoKartMove& Move);

	FVector GetAirResistance();
	FVector GetRollingResistance();


};
