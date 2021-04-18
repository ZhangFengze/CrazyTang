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
			else if (tag == "voxel")
			{
				if (!e.Has<Voxel>())
					e.Add<Voxel>();
				auto voxel = e.Get<Voxel>();
				*voxel = std::get<0>(zs::Read<Voxel>(components));
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
	ct::AsyncConnect(io_, ServerEndpoint(),
		[&](const std::error_code& ec, std::shared_ptr<ct::Socket> socket)
	{
		if (ec)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Connection Failed"));
			return;
		}
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Connection Succeed"));

		auto pipe = std::make_shared<ct::Pipe<>>(std::move(*socket));
		OnConnected(io_, pipe);
	});
}

void ACrazyTangGameModeBase::Tick(float DeltaSeconds)
{
	io_.poll();
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

	agent->Listen("world",
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

		for (auto [uuid, entity] : m_IDToEntities)
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
			auto old = m_IDToEntities.find(uuid);
			if (old == m_IDToEntities.end())
			{
				// add
				auto actor = GetWorld()->SpawnActor<ACrazyTangPawnBase>(MyPawn);
				if (entity.Has<ConnectionID>() && entity.Get<ConnectionID>()->id == clientID)
					actor->SetupNetAgent(agent.get());
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
		m_IDToEntities = newIDToEntities;
	});
}
