// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#if PLATFORM_WINDOWS
#include "Windows/PreWindowsApi.h"
#endif
#include <asio.hpp>
#if PLATFORM_WINDOWS
#include "Windows/PostWindowsApi.h"
#endif

#include "CrazyTangGameModeBase.generated.h"

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
	asio::io_context io_;
};
