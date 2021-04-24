// Copyright Epic Games, Inc. All Rights Reserved.


#include "CrazyTangGameModeBase.h"

#include "Engine.h"

THIRD_PARTY_INCLUDES_START
#include "Eigen/Eigen"
#include "ZSerializer.hpp"
#include "common/AsyncConnect.h"
#include "common/NetAgent.h"
#include "common/Entity.h"
#include "common/Position.h"
#include "common/Velocity.h"
#include "common/Voxel.h"
#include "common/Math.h"
#include "client_core/Login.h"
THIRD_PARTY_INCLUDES_END

using namespace asio::ip;
using namespace ct;

namespace
{
	tcp::endpoint StringToEndpoint(const std::string& str)
	{
		auto pos = str.find(':');
		if (pos == std::string::npos)
			return {};
		auto ip = str.substr(0, pos);
		int port = std::stoi(str.substr(pos + 1));
		return { asio::ip::make_address_v4(ip), (unsigned short)port };
	}

	tcp::endpoint ServerEndpoint()
	{
		return StringToEndpoint("127.0.0.1:33773");
	}

	struct ConnectionID
	{
		uint64_t id;
	};

	template<typename In>
	bool ReadEntity(In& in, ct::EntityHandle e)
	{
		auto uuid = zs::Read<UUID>(in);
		if (std::holds_alternative<zs::Error>(uuid))
			return false;
		*e.Add<UUID>() = std::get<0>(uuid);

		zs::StringReader components(std::get<0>(zs::Read<std::string>(in)));
		while (true)
		{
			auto _tag = zs::Read<std::string>(components);
			if (std::holds_alternative<zs::Error>(_tag))
				break;

			auto tag = std::get<0>(_tag);
			if (tag == "connection")
			{
				if (!e.Has<ConnectionID>())
					e.Add<ConnectionID>();
				*e.Get<ConnectionID>() = std::get<0>(zs::Read<ConnectionID>(components));
			}
			else if (tag == "position")
			{
				if (!e.Has<Position>())
					e.Add<Position>();
				*e.Get<Position>() = std::get<0>(zs::Read<Position>(components));
			}
			else if (tag == "velocity")
			{
				if (!e.Has<Velocity>())
					e.Add<Velocity>();
				*e.Get<Velocity>() = std::get<0>(zs::Read<Velocity>(components));
			}
			else
			{
				assert(false);
			}
		}
		return true;
	}

	struct ActorInfo
	{
		ACrazyTangPawnBase* pawn = nullptr;
	};
}

ACrazyTangGameModeBase::ACrazyTangGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACrazyTangGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("InitGame"));
	ct::AsyncConnect(m_HighPriorityIO, ServerEndpoint(),
		[&](const std::error_code& ec, std::shared_ptr<ct::Socket> socket)
	{
		if (ec)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Connection Failed"));
			return;
		}
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Connection Succeed"));

		auto pipe = std::make_shared<ct::Pipe<>>(std::move(*socket));
		OnConnected(m_HighPriorityIO, pipe);
	});

	if (m_VoxelActors.Num() != m_Voxels.x * m_Voxels.y * m_Voxels.z)
	{
		m_VoxelActors.SetNum(m_Voxels.x * m_Voxels.y * m_Voxels.z, true);
	}

	for (int i = 0; i < m_VoxelActors.Num(); ++i)
	{
		m_LowPriorityIO.post(
			[this, i]()
		{
			m_VoxelActors[i] = GetWorld()->SpawnActor<AActor>(MyVoxel);
		});
	}
}

void ACrazyTangGameModeBase::Tick(float DeltaSeconds)
{
	m_HighPriorityIO.poll();
	m_LowPriorityIO.run_for(std::chrono::milliseconds{ 1 });
	TickVoxels(DeltaSeconds);
	return Super::Tick(DeltaSeconds);
}

void ACrazyTangGameModeBase::OnConnected(asio::io_context& io, std::shared_ptr<ct::Pipe<>> pipe)
{
	auto login = std::make_shared<ct::Login<>>(pipe, io, std::chrono::seconds{ 3 });
	login->OnSuccess(
		[login, &io, pipe, this](uint64_t id) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Login Succeed"));
		OnLoginSuccess(io, id, pipe);
	});
	login->OnError(
		[login]() {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Login Failed"));
	});
}

void ACrazyTangGameModeBase::OnLoginSuccess(asio::io_context& io, uint64_t clientID, std::shared_ptr<Pipe<>> pipe)
{
	auto agent = std::make_shared<ct::NetAgent<>>(pipe);
	agent->OnError(
		[agent]() {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("NetAgent OnError"));
	});

	{
		zs::StringWriter out;
		zs::Write(out, Eigen::Vector3f{ 1.f,0,0 });
		agent->Send("set position", out.String());
	}

	{
		zs::StringWriter out;
		zs::Write(out, Eigen::Vector3f{ 0,50.f,0 });
		agent->Send("set velocity", out.String());
	}

	agent->Listen("entities",
		[this, clientID, agent](std::string&& rawWorld)
	{
		zs::StringReader worldArchive{ std::move(rawWorld) };

		ct::EntityContainer newEntities;
		std::unordered_map<uint64_t, ct::EntityHandle> newIDToEntities;
		while (true)
		{
			auto e = newEntities.Create();
			if (!ReadEntity(worldArchive, e))
			{
				e.Destroy();
				break;
			}
			newIDToEntities[e.Get<UUID>()->id] = e;
		}

		std::unordered_map<uint64_t, ct::EntityHandle> oldIDToEntities;
		m_Entities.ForEach([&oldIDToEntities](ct::EntityHandle e)
		{
			oldIDToEntities[e.Get<UUID>()->id] = e;
		});

		for (auto [uuid, entity] : oldIDToEntities)
		{
			if (newIDToEntities.find(uuid) == newIDToEntities.end())
			{
				// remove
				entity.Get<ActorInfo>()->pawn->Destroy();
				entity.Destroy();
			}
		}

		for (auto [uuid, entity] : newIDToEntities)
		{
			auto old = oldIDToEntities.find(uuid);
			if (old == oldIDToEntities.end())
			{
				// add
				auto actor = GetWorld()->SpawnActor<ACrazyTangPawnBase>(MyPawn);
				if (entity.Has<ConnectionID>() && entity.Get<ConnectionID>()->id == clientID)
					actor->SetupNetAgent(agent.get());
				actor->SetUUID(uuid);
				auto info = entity.Add<ActorInfo>();
				info->pawn = actor;
			}
			else
			{
				// refresh
				auto info = entity.Add<ActorInfo>();
				*info = *(old->second.Get<ActorInfo>());
			}
		}

		m_Entities = newEntities;
	});

	agent->Listen("voxels",
		[this, agent](std::string&& rawVoxels)
	{
		zs::StringReader in(std::move(rawVoxels));
		auto voxels = zs::Read<ct::voxel::Container>(in);
		m_Voxels = std::get<0>(voxels);
	});
}

ct::EntityHandle ACrazyTangGameModeBase::GetEntity(uint64_t id)
{
	ct::EntityHandle entity;
	m_Entities.ForEach([&entity, id](ct::EntityHandle e)
	{
		if (entity.Valid())
			return;
		if(e.Get<UUID>()->id == id)
			entity = e;
	});
	return entity;
}

void ACrazyTangGameModeBase::TickVoxels(float dt)
{
	int index = 0;
	for (size_t x = 0;x < m_Voxels.x;++x)
	{
		for (size_t y = 0;y < m_Voxels.y;++y)
		{
			for (size_t z = 0;z < m_Voxels.z;++z)
			{
				if (m_VoxelActors[index] == nullptr)
					continue;

				auto actor = m_VoxelActors[index];
				if (m_Voxels.Get(x, y, z).type == 1)
				{
					actor->SetActorLocation(FVector{ x * 100.f,y * 100.f,z * 100.f });
					actor->SetActorHiddenInGame(false);
				}
				else
				{
					actor->SetActorHiddenInGame(true);
				}

				index++;
			}
		}
	}
}
