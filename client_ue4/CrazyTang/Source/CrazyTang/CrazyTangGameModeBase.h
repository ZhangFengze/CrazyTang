// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include <memory>
#include <map>

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CrazyTangPawnBase.h"

#if PLATFORM_WINDOWS
#include "Windows/PreWindowsApi.h"
#endif
#include "asio.hpp"
#include "common/Pipe.h"
#include "common/Entity.h"
#include "common/UUID.h"
#include "common/Voxel.h"
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

public:
	void OnConnected(asio::io_context& io, std::shared_ptr<ct::Pipe<>> pipe);
	void OnLoginSuccess(asio::io_context& io, uint64_t id, std::shared_ptr<ct::Pipe<>> pipe);

	ct::EntityHandle GetEntity(uint64_t);

	void TickVoxels(float dt);

private:
	asio::io_context io_;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACrazyTangPawnBase> MyPawn;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> MyVoxel;

	ct::EntityContainer m_Entities;
	ct::voxel::Container m_Voxels;

	UPROPERTY()
	TArray<AActor*> m_VoxelActors;
};
