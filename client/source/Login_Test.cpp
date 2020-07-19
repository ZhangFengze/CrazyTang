#ifdef CT_TEST
#include <catch.hpp>
#include "Login.h"
#include "../../common/source/Pipe.h"

namespace
{
	const std::string clientHello = "hello from client";
	const std::string wrongClientHello = "hi from client";
	const std::string serverHello = "hello client, your id is ";
	const std::string wrongServerHello = "hi client, your id is ";

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
	login.OnSuccess([&success, &id](uint64_t id_) {success = true; id = id_; });

	REQUIRE(pipe->writtenPackets_.size() == 1);
	REQUIRE(pipe->writtenPackets_[0] == clientHello);

	auto reply = serverHello + "123";
	pipe->PacketArrive(reply.data(), reply.size());

	REQUIRE(success);
	REQUIRE(id == 123);
}

TEST_CASE("login failed by wrong server hello")
{
	asio::io_context io;
	auto pipe = std::make_shared<ct::MockPipe>();
	Login login{ pipe, io, std::chrono::seconds{10} };

	bool error = false;
	login.OnError([&error]() {error = true; });
	login.OnSuccess([](uint64_t) {REQUIRE(false); });

	auto reply = wrongServerHello + "123";
	pipe->PacketArrive(reply.data(), reply.size());

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

	auto reply = serverHello + "123";
	pipe->PacketArrive(reply.data(), reply.size());

	pipe->PacketArrive(reply.data(), reply.size());

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

	auto reply = wrongServerHello + "123";
	pipe->PacketArrive(reply.data(), reply.size());

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

	auto reply = wrongServerHello + "123";
	pipe->PacketArrive(reply.data(), reply.size());
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
#endif
