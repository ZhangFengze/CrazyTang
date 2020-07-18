#ifdef CT_TEST
#include <catch.hpp>
#include "Login.h"
#include "../../common/source/Pipe.h"

namespace
{
	const std::string clientHello = "hello from client";
	const std::string wrongClientHello = "hi from client";
	const std::string serverHello = "hello client, your id is ";

	using Login = ct::Login<ct::MockPipe>;
}

TEST_CASE("login")
{
	asio::io_context io;
	Login login{ std::make_shared<ct::MockPipe>(), 123, io, std::chrono::seconds{10} };
	login.OnError([]() {REQUIRE(false); });
	bool success = false;
	login.OnSuccess([&success] {success = true; });

	login.pipe_->PacketArrive(clientHello.data(), clientHello.size());

	REQUIRE(success);
	REQUIRE(login.pipe_->writtenPackets_.size() == 1);
	auto reply = login.pipe_->writtenPackets_[0];
	REQUIRE(reply == serverHello + "123");
}

TEST_CASE("login failed by wrong client hello")
{
	asio::io_context io;
	Login login{ std::make_shared<ct::MockPipe>(), 123, io, std::chrono::seconds{10} };
	bool failed = false;
	login.OnError([&failed]() {failed = true; });
	login.OnSuccess([] {REQUIRE(false); });

	login.pipe_->PacketArrive(wrongClientHello.data(), wrongClientHello.size());

	REQUIRE(failed);
}

TEST_CASE("login failed by timeout")
{
	asio::io_context io;
	Login login{ std::make_shared<ct::MockPipe>(), 123, io, std::chrono::nanoseconds{1} };
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
	Login login{ std::make_shared<ct::MockPipe>(), 123, io, std::chrono::seconds{10} };
	bool failed = false;
	login.OnError([&failed]() {failed = true; });
	login.OnSuccess([] {REQUIRE(false); });

	login.pipe_->SetBroken();

	REQUIRE(failed);
}

TEST_CASE("login cleanup pipe after success")
{
	asio::io_context io;
	Login login{ std::make_shared<ct::MockPipe>(), 123, io, std::chrono::seconds{10} };
	bool error = false;
	login.OnError([&error] {error = true; });
	login.OnSuccess([]{});

	login.pipe_->PacketArrive(clientHello.data(), clientHello.size());
	login.pipe_->PacketArrive(clientHello.data(), clientHello.size());
	login.pipe_->PacketArrive(wrongClientHello.data(), wrongClientHello.size());

	login.pipe_->SetBroken();

	REQUIRE(login.pipe_->writtenPackets_.size() == 1);
	REQUIRE(!error);
}

TEST_CASE("login cleanup pipe after failed by wrong client hello")
{
	asio::io_context io;
	Login login{ std::make_shared<ct::MockPipe>(), 123, io, std::chrono::seconds{10} };
	int error = 0;
	login.OnError([&error] {++error; });
	login.OnSuccess([]{});

	login.pipe_->PacketArrive(wrongClientHello.data(), wrongClientHello.size());
	login.pipe_->PacketArrive(clientHello.data(), clientHello.size());
	login.pipe_->PacketArrive(clientHello.data(), clientHello.size());

	login.pipe_->SetBroken();

	REQUIRE(login.pipe_->writtenPackets_.size() == 0);
	REQUIRE(error == 1);
}

TEST_CASE("login cleanup pipe after failed by timeout")
{
	asio::io_context io;
	Login login{ std::make_shared<ct::MockPipe>(), 123, io, std::chrono::nanoseconds{1} };
	int error = 0;
	login.OnError([&error] {++error; });
	login.OnSuccess([]{});

	asio::steady_timer t{ io,std::chrono::nanoseconds{2} };
	t.wait();
	io.run();

	login.pipe_->PacketArrive(clientHello.data(), clientHello.size());
	login.pipe_->SetBroken();

	REQUIRE(login.pipe_->writtenPackets_.size() == 0);
	REQUIRE(error == 1);
}

TEST_CASE("login cleanup pipe after failed by pipe broken")
{
	asio::io_context io;
	Login login{ std::make_shared<ct::MockPipe>(), 123, io, std::chrono::nanoseconds{1} };
	int error = 0;
	login.OnError([&error] {++error; });
	login.OnSuccess([]{});

	login.pipe_->SetBroken();

	asio::steady_timer t{ io,std::chrono::nanoseconds{2} };
	t.wait();
	io.run();

	REQUIRE(error == 1);
}
#endif
