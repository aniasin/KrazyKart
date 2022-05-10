// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "KK/MoveComponent.h"
#include "KK/ReplicationComponent.h"
#include "GoKart.generated.h"

UCLASS()
class KK_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UMoveComponent* MoveComponent;
	UPROPERTY()
	class UReplicationComponent* ReplicationComponent;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void QuitGame();

};
