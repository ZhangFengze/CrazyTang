// Copyright Epic Games, Inc. All Rights Reserved.


#include "CrazyTangGameModeBase.h"

#include "Engine.h"

THIRD_PARTY_INCLUDES_START
#include "Eigen/Eigen"
#include "ZSerializer.hpp"
#include "common/AsyncConnect.h"
#include "common/NetAgent.h"
#include "common/Entity.h"
#include "common/Player.h"
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
		return StringToEndpoint("127.0.0.1:3377");
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
		std::map<uint64_t, ct::EntityHandle> oldEntities;
		oldEntities.swap(m_EntitiesID);

		zs::StringReader worldArchive{ std::move(rawWorld) };
		while (auto id = std::get<0>(zs::Read<uint64_t>(worldArchive)))
		{
			zs::StringReader entityArchive{ std::get<0>(zs::Read<std::string>(worldArchive)) };
			if (auto iter = oldEntities.find(id); iter != oldEntities.end())
			{
				auto e = iter->second;
				LoadPlayer(entityArchive, e);
				m_EntitiesID[id] = e;

				oldEntities.erase(iter);

				FVector pos
				{
					e.Get<Position>()->data.x(),
					e.Get<Position>()->data.y(),
					e.Get<Position>()->data.z(),
				};
				e.Get<ActorInfo>()->pawn->SetActorLocation(pos);
			}
			else
			{
				auto e = m_Entities.Create();
				LoadPlayer(entityArchive, e);
				m_EntitiesID[id] = e;

				auto actor = GetWorld()->SpawnActor<ACrazyTangPawnBase>(MyPawn);
				if (id == clientID)
					actor->SetupNetAgent(agent.get());
				auto info = e.Add<ActorInfo>();
				info->pawn = actor;
			}
		}

		for (auto [_, e] : oldEntities)
		{
			e.Get<ActorInfo>()->pawn->Destroy();
			e.Destroy();
		}
	});
}
