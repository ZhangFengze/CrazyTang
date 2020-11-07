// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CrazyTangGameModeBase.generated.h"

namespace asio
{
	class io_context;
}

/**
 * 
 */
UCLASS()
class CRAZYTANG_API ACrazyTangGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ACrazyTangGameModeBase();

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	void Tick(float DeltaSeconds) override;

private:
	asio::io_context* io_ = nullptr;
};
