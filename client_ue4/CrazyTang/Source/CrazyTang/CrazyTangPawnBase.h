// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#if PLATFORM_WINDOWS
#include "Windows/PreWindowsApi.h"
#endif
#include "Eigen/Eigen"
#include "common/NetAgent.h"
#if PLATFORM_WINDOWS
#include "Windows/PostWindowsApi.h"
#endif

#include "CrazyTangPawnBase.generated.h"

UCLASS()
class CRAZYTANG_API ACrazyTangPawnBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACrazyTangPawnBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	void SetupNetAgent(ct::NetAgent<>*);

	void MoveX(float axis);
	void MoveY(float axis);

	void SendInput();

private:
	ct::NetAgent<>* m_NetAgent = nullptr;
	Eigen::Vector3f m_Input;
};
