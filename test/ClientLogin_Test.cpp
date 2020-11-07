#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../client_core/Login.h"
#include "../common/Packet.h"
#include "../common/Pipe.h"

using namespace std::literals;

namespace
{
	const ct::Packet clientHello = "hello from client"sv;
	const ct::Packet wrongClientHello = "hi from client"sv;
	uint64_t expectedID = 123;
	const ct::Packet serverHello = "hello client, your id is 123"sv;
	const ct::Packet wrongServerHello = "hi client, your id is 123"sv;
	const ct::Packet wrongIDServerHello = "hello client, your id is not a number"sv;

	using Login = ct::Login<ct::MockPipe>;
}

TEST_CASE("login")
{
	asio::io_context io;
	auto pipe = std::make_shared<ct::MockPipe>();
	Login login{ pipe, io, std::chrono::seconds{10} };

	login.OnError([]() {REQUIRE(false); });

	bool success = false;
	uint64_t id = -1;
	login.OnSuccess([&success, &id](uint64_t id_) {REQUIRE(!success); success = true; id = id_; });

	REQUIRE(pipe->writtenPackets_.size() == 1);
	REQUIRE(pipe->writtenPackets_[0] == clientHello);

	pipe->PacketArrive(serverHello);

	REQUIRE(success);
	REQUIRE(id == expectedID);
}

TEST_CASE("login failed by wrong server hello")
{
	asio::io_context io;
	auto pipe = std::make_shared<ct::MockPipe>();
	Login login{ pipe, io, std::chrono::seconds{10} };

	bool error = false;
	login.OnError([&error]() {error = true; });
	login.OnSuccess([](uint64_t) {REQUIRE(false); });

	pipe->PacketArrive(wrongServerHello);

	REQUIRE(error);
}

TEST_CASE("login failed by wrong id")
{
	asio::io_context io;
	auto pipe = std::make_shared<ct::MockPipe>();
	Login login{ pipe, io, std::chrono::seconds{10} };

	bool error = false;
	login.OnError([&error]() {error = true; });
	login.OnSuccess([](uint64_t) {REQUIRE(false); });

	pipe->PacketArrive(wrongIDServerHello);

	REQUIRE(error);
}

TEST_CASE("login failed by timeout")
{
	asio::io_context io;
	auto pipe = std::make_shared<ct::MockPipe>();
	Login login{ pipe, io, std::chrono::nanoseconds{1} };

	bool error = false;
	login.OnError([&error]() {error = true; });
	login.OnSuccess([](uint64_t) {REQUIRE(false); });

	asio::steady_timer timer{ io,std::chrono::nanoseconds{2} };
	timer.wait();

	io.run();

	REQUIRE(error);
}

TEST_CASE("login failed by pipe broken")
{
	asio::io_context io;
	auto pipe = std::make_shared<ct::MockPipe>();
	Login login{ pipe, io, std::chrono::seconds{10} };

	bool error = false;
	login.OnError([&error]() {error = true; });
	login.OnSuccess([](uint64_t) {REQUIRE(false); });

	pipe->SetBroken();

	REQUIRE(error);
}

TEST_CASE("login cleanup pipe after success")
{
	asio::io_context io;
	auto pipe = std::make_shared<ct::MockPipe>();
	Login login{ pipe, io, std::chrono::seconds{10} };

	bool error = false;
	login.OnError([&error] {error = true; });
	login.OnSuccess([](uint64_t){});

	pipe->PacketArrive(serverHello);

	pipe->PacketArrive(serverHello);

	pipe->SetBroken();

	REQUIRE(!error);
	REQUIRE(pipe->receivedPackets_.size() == 1);
}

TEST_CASE("login cleanup pipe after failed by wrong server hello")
{
	asio::io_context io;
	auto pipe = std::make_shared<ct::MockPipe>();
	Login login{ pipe, io, std::chrono::seconds{10} };
	int error = 0;
	login.OnError([&error] {++error; });
	login.OnSuccess([](uint64_t) {});

	pipe->PacketArrive(wrongServerHello);

	pipe->SetBroken();

	REQUIRE(pipe->receivedPackets_.size() == 0);
	REQUIRE(error == 1);
}

TEST_CASE("login cleanup pipe after failed by timeout")
{
	asio::io_context io;
	auto pipe = std::make_shared<ct::MockPipe>();
	Login login{ pipe, io, std::chrono::nanoseconds{1} };
	int error = 0;
	login.OnError([&error] {++error; });
	login.OnSuccess([](uint64_t) {});

	asio::steady_timer t{ io,std::chrono::nanoseconds{2} };
	t.wait();
	io.run();

	pipe->PacketArrive(wrongServerHello);
	pipe->SetBroken();

	REQUIRE(pipe->receivedPackets_.size() == 1);
	REQUIRE(error == 1);
}

TEST_CASE("login cleanup pipe after failed by pipe broken")
{
	asio::io_context io;
	auto pipe = std::make_shared<ct::MockPipe>();
	Login login{ pipe, io, std::chrono::nanoseconds{1} };
	int error = 0;
	login.OnError([&error] {++error; });
	login.OnSuccess([](uint64_t) {});

	pipe->SetBroken();

	asio::steady_timer t{ io,std::chrono::nanoseconds{2} };
	t.wait();
	io.run();

	REQUIRE(error == 1);
}