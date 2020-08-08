#ifdef CT_TEST
#include <catch.hpp>
#include "Login.h"
#include "../common/source/Packet.h"
#include "../common/source/Pipe.h"

using namespace std::literals;

namespace
{
	const ct::Packet clientHello = "hello from client"sv;
	const ct::Packet wrongClientHello = "hi from client"sv;
	uint64_t id = 123;
	const ct::Packet serverHello = "hello client, your id is 123"sv;

	using Login = ct::Login<ct::MockPipe>;
}

TEST_CASE("login")
{
	asio::io_context io;
	auto pipe = std::make_shared<ct::MockPipe>();
	Login login{ pipe, id, io, std::chrono::seconds{10} };
	login.OnError([]() {REQUIRE(false); });
	bool success = false;
	login.OnSuccess([&success] {success = true; });

	pipe->PacketArrive(clientHello);

	REQUIRE(success);
	REQUIRE(pipe->writtenPackets_.size() == 1);
	auto reply = pipe->writtenPackets_[0];
	REQUIRE(reply == serverHello);
}

TEST_CASE("login failed by wrong client hello")
{
	asio::io_context io;
	auto pipe = std::make_shared<ct::MockPipe>();
	Login login{ pipe, id, io, std::chrono::seconds{10} };
	bool failed = false;
	login.OnError([&failed]() {failed = true; });
	login.OnSuccess([] {REQUIRE(false); });

	pipe->PacketArrive(wrongClientHello);

	REQUIRE(failed);
}

TEST_CASE("login failed by timeout")
{
	asio::io_context io;
	auto pipe = std::make_shared<ct::MockPipe>();
	Login login{ pipe, id, io, std::chrono::nanoseconds{1} };
	bool failed = false;
	login.OnError([&failed]() {failed = true; });
	login.OnSuccess([] {REQUIRE(false); });

	asio::steady_timer t{ io,std::chrono::nanoseconds{2} };
	t.wait();

	io.run();

	REQUIRE(failed);
}

TEST_CASE("login failed by pipe broken")
{
	asio::io_context io;
	auto pipe = std::make_shared<ct::MockPipe>();
	Login login{ pipe, id, io, std::chrono::seconds{10} };
	bool failed = false;
	login.OnError([&failed]() {failed = true; });
	login.OnSuccess([] {REQUIRE(false); });

	pipe->SetBroken();

	REQUIRE(failed);
}

TEST_CASE("login cleanup pipe after success")
{
	asio::io_context io;
	auto pipe = std::make_shared<ct::MockPipe>();
	Login login{ pipe, id, io, std::chrono::seconds{10} };
	bool error = false;
	login.OnError([&error] {error = true; });
	login.OnSuccess([]{});

	pipe->PacketArrive(clientHello);
	pipe->PacketArrive(clientHello);
	pipe->PacketArrive(wrongClientHello);

	pipe->SetBroken();

	REQUIRE(pipe->writtenPackets_.size() == 1);
	REQUIRE(!error);
}

TEST_CASE("login cleanup pipe after failed by wrong client hello")
{
	asio::io_context io;
	auto pipe = std::make_shared<ct::MockPipe>();
	Login login{ pipe, id, io, std::chrono::seconds{10} };
	int error = 0;
	login.OnError([&error] {++error; });
	login.OnSuccess([]{});

	pipe->PacketArrive(wrongClientHello);
	pipe->PacketArrive(clientHello);
	pipe->PacketArrive(clientHello);

	pipe->SetBroken();

	REQUIRE(pipe->writtenPackets_.size() == 0);
	REQUIRE(error == 1);
}

TEST_CASE("login cleanup pipe after failed by timeout")
{
	asio::io_context io;
	auto pipe = std::make_shared<ct::MockPipe>();
	Login login{ pipe, id, io, std::chrono::nanoseconds{1} };
	int error = 0;
	login.OnError([&error] {++error; });
	login.OnSuccess([]{});

	asio::steady_timer t{ io,std::chrono::nanoseconds{2} };
	t.wait();
	io.run();

	pipe->PacketArrive(clientHello);
	pipe->SetBroken();

	REQUIRE(pipe->writtenPackets_.size() == 0);
	REQUIRE(error == 1);
}

TEST_CASE("login cleanup pipe after failed by pipe broken")
{
	asio::io_context io;
	auto pipe = std::make_shared<ct::MockPipe>();
	Login login{ pipe, id, io, std::chrono::nanoseconds{1} };
	int error = 0;
	login.OnError([&error] {++error; });
	login.OnSuccess([]{});

	pipe->SetBroken();

	asio::steady_timer t{ io,std::chrono::nanoseconds{2} };
	t.wait();
	io.run();

	REQUIRE(error == 1);
}
#endif
