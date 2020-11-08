// Copyright Epic Games, Inc. All Rights Reserved.


#include "CrazyTangGameModeBase.h"

#include "Engine.h"

THIRD_PARTY_INCLUDES_START
#include "Eigen/Eigen"
#include "common/AsyncConnect.h"
#include "common/NetAgent.h"
#include "common/Entity.h"
#include "common/Player.h"
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
		OnLoginSuccess(io, pipe);
	});
	login->OnError(
		[login]() {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Login Failed"));
	});
}

void ACrazyTangGameModeBase::OnLoginSuccess(asio::io_context& io, std::shared_ptr<Pipe<>> pipe)
{
	auto agent = std::make_shared<ct::NetAgent<>>(pipe);
	agent->OnError(
		[agent]() {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("NetAgent OnError"));
	});

	agent->Listen("echo",
		[agent](std::string&& data) {
		printf("net agent on echo:%s\n", data.c_str());
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("NetAgent OnEcho"));
	});
	agent->Send("echo", "hello?");

	agent->Listen("broadcast",
		[agent](std::string&& data) {
		InputStringArchive in{ std::move(data) };
		auto from = in.Read<uint64_t>();
		auto content = in.Read<std::string>();
		//printf("net agent on broadcast, from %llu, content:%s\n", *from, content->c_str());
	});
	agent->Send("broadcast", "hello everyone?");

	agent->Listen("list online",
		[agent](std::string&& data) {
		InputStringArchive in{ std::move(data) };
		auto size = in.Read<size_t>();
		//printf("net agent on list online: %llu online\n", *size);
		for (size_t i = 0; i < size; ++i)
		{
			auto id = in.Read<uint64_t>();
			//printf("%llu\n", *id);
		}
	});
	agent->Send("list online", "");

	{
		OutputStringArchive ar;
		ar.Write(Eigen::Vector3f{ 1.f,0,0 });
		agent->Send("set position", ar.String());
	}

	{
		OutputStringArchive ar;
		ar.Write(Eigen::Vector3f{ 0,1.f,0 });
		agent->Send("set velocity", ar.String());
	}

	agent->Listen("world",
		[](std::string&& rawWorld)
	{
		printf("net agent on world,");
		InputStringArchive worldArchive{ std::move(rawWorld) };
		while (true)
		{
			auto id = worldArchive.Read<uint64_t>();
			if (!id) break;
			InputStringArchive entityArchive{ worldArchive.Read<std::string>().value() };
			EntityContainer entities;
			auto e = entities.Create();
			LoadPlayer(entityArchive, e);
			//printf(" [id:%llu, position:%f %f %f, velocity:%f %f %f],", id.value(),
			//    e.Get<Position>()->data.x(),
			//    e.Get<Position>()->data.y(),
			//    e.Get<Position>()->data.z(),
			//    e.Get<Velocity>()->data.x(),
			//    e.Get<Velocity>()->data.y(),
			//    e.Get<Velocity>()->data.z());
		}
		printf("\n");
	});
}
