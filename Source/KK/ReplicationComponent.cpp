// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplicationComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UReplicationComponent::UReplicationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
}

// Called when the game starts
void UReplicationComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UReplicationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		FGoKartMove Move = MoveComponent->CreateMove(DeltaTime);
		MoveComponent->SimulateMove(Move);
		MoveComponent->AddUnaknowledgedMove(Move);
		Server_SendMove(Move);
	}
	if (GetOwnerRole() == ROLE_Authority && GetOwner()->GetInstigatorController()->IsLocalController())
	{
		FGoKartMove Move = MoveComponent->CreateMove(DeltaTime);
		Server_SendMove(Move);
	}
	if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		MoveComponent->SimulateMove(ServerState.LastMove);
	}
}

void UReplicationComponent::Server_SendMove_Implementation(FGoKartMove Move)
{
	if (!MoveComponent) return;

	MoveComponent->SimulateMove(Move);
	ServerState.LastMove = Move;
	ServerState.Transform = GetOwner()->GetActorTransform();
	ServerState.Velocity = MoveComponent->GetVelocity();
}

bool UReplicationComponent::Server_SendMove_Validate(FGoKartMove Move)
{
	return true; //TODO make better validation
}

void UReplicationComponent::OnRep_ServerState()
{
	if (!MoveComponent) return;

	GetOwner()->SetActorTransform(ServerState.Transform);
	MoveComponent->SetVelocity(ServerState.Velocity);
	MoveComponent->ClearAknowledgedMoves();

	for (const FGoKartMove& Move : MoveComponent->GetUnaknowledgedMoves())
	{
		MoveComponent->SimulateMove(Move);
	}
}
void UReplicationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UReplicationComponent, ServerState);
}
