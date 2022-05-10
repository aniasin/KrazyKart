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

	MoveComponent = CreateDefaultSubobject<UMoveComponent>(TEXT("MoveComp"));
}

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", MoveComponent, &UMoveComponent::Move);
	PlayerInputComponent->BindAxis("MoveRight", MoveComponent, &UMoveComponent::MoveRight);

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

	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		FGoKartMove Move = MoveComponent->CreateMove(DeltaTime);
		MoveComponent->SimulateMove(Move, ServerState.Velocity);
		MoveComponent->AddUnaknowledgedMove(Move);
		Server_SendMove(Move);
	}
	if (GetLocalRole() == ROLE_Authority && IsLocallyControlled())
	{
		FGoKartMove Move = MoveComponent->CreateMove(DeltaTime);
		Server_SendMove(Move);
	}
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		MoveComponent->SimulateMove(ServerState.LastMove, ServerState.Velocity);
	}
	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(GetLocalRole()), this, FColor::Green, DeltaTime);
}

void AGoKart::Server_SendMove_Implementation(FGoKartMove Move)
{
	MoveComponent->SimulateMove(Move, ServerState.Velocity);
	ServerState.LastMove = Move;
	ServerState.Transform = GetActorTransform();
	ServerState.Velocity = MoveComponent->GetVelocity();
}

bool AGoKart::Server_SendMove_Validate(FGoKartMove Move)
{
	return true; //TODO make better validation
}

void AGoKart::OnRep_ServerState()
{
	SetActorTransform(ServerState.Transform);
	MoveComponent->SetVelocity(ServerState.Velocity);
	MoveComponent->ClearAknowledgedMoves(ServerState.LastMove);

	for (const FGoKartMove& Move : MoveComponent->GetUnaknowledgedMoves())
	{
		MoveComponent->SimulateMove(Move, ServerState.Velocity);
	}
}

void AGoKart::QuitGame()
{
	GEngine->GetFirstLocalPlayerController(GetWorld())->ConsoleCommand("quit");
}

void AGoKart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGoKart, ServerState);
}
