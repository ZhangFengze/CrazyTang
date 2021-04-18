// Fill out your copyright notice in the Description page of Project Settings.


#include "CrazyTangPawnBase.h"
THIRD_PARTY_INCLUDES_START
#include "ZSerializer.hpp"
#include "common/Math.h"
THIRD_PARTY_INCLUDES_END
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

// Sets default values
ACrazyTangPawnBase::ACrazyTangPawnBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set this pawn to be controlled by the lowest-numbered player
	//AutoPossessPlayer = EAutoReceiveInput::Player0;

}

// Called when the game starts or when spawned
void ACrazyTangPawnBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACrazyTangPawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SendInput();
}

// Called to bind functionality to input
void ACrazyTangPawnBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveX", this, &ACrazyTangPawnBase::MoveX);
	PlayerInputComponent->BindAxis("MoveY", this, &ACrazyTangPawnBase::MoveY);
}

void ACrazyTangPawnBase::SetupNetAgent(ct::NetAgent<>* agent)
{
	m_NetAgent = agent;
	auto controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	controller->Possess(this);
}

void ACrazyTangPawnBase::MoveX(float axis)
{
	m_Input.x() = axis * 100;
}

void ACrazyTangPawnBase::MoveY(float axis)
{
	m_Input.y() = axis * 100;
}

void ACrazyTangPawnBase::SendInput()
{
	if (m_NetAgent)
	{
		zs::StringWriter out;
		zs::Write(out, m_Input);
		m_NetAgent->Send("set velocity", ar.String());
	}
}
