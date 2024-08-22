#include "pch.h"
#include "../Include/NetworkInterface.h"
#include "../Network/SocketAddress.h"
#include "../Serialization/MemoryStream.h"
#include "../Serialization/MemoryBitStream.h"
#include "../Serialization/LinkingContext.h"
#include "../Serialization/Endian.h"
#include "../Serialization/Reflection.h"
#include <cstddef>

//출력 창 - 테스트에 출력하기
#define gcout std::cout << "[   INFO   ] "

using namespace std::chrono_literals;

constexpr int DEFAULT_BUFLEN = 512;

namespace Network
{
	void ExcuteTCPServer(const std::string& addr)
	{
		std::unique_ptr<TCPServer> tcpServer = CreateTCPServer();
		tcpServer->Bind(addr);

		do
		{
			bool update = false;
			tcpServer->UpdateSocket(&update);
			if (!update) continue;

			bool exist = false;
			int32_t recvBytes{ 0 };
			std::array<void*, DEFAULT_BUFLEN> recvbuf{};
			EXPECT_TRUE(tcpServer->Receive(recvbuf.data(), recvbuf.size(), &recvBytes, &exist));
			if (recvBytes == 0)
			{
				gcout<< "disconnected" << std::endl;
				break;
			}

			if (recvBytes > 0)
			{
				gcout << "Server Bytes received: " << recvBytes << std::endl;
				EXPECT_TRUE(tcpServer->Send(recvbuf.data(), recvBytes, nullptr));
			}
		} while (true);
	}

	void ReceiveData(TCPClient* tcpClient)
	{
		int32_t recvBytes{ 0 };
		do
		{
			std::array<void*, DEFAULT_BUFLEN> recvbuf{};
			auto result = tcpClient->Receive(recvbuf.data(), recvbuf.size(), &recvBytes);
			if (recvBytes == -1)
				continue;

			if (recvBytes == 0)
			{
				gcout << "disconnected" << std::endl;
				break;
			}

			if (recvBytes > 0)
				gcout << "Client Bytes received: " << recvBytes << std::endl;

		} while (true);
	}

	void ExcuteTCPClient(const std::string& addr)
	{
		std::unique_ptr<TCPClient> tcpClient = CreateTCPClient();

		EXPECT_TRUE(tcpClient->Connect(addr));

		std::string sendbuf{ "this is a test" };
		EXPECT_TRUE(tcpClient->Send(sendbuf.c_str(), sendbuf.size(), nullptr));
		EXPECT_TRUE(tcpClient->Shutdown());

		std::future<void> recv = std::async(std::launch::async, ReceiveData, tcpClient.get());
		recv.wait();
	}

	TEST(TCPSocket, Test)
	{
		//blocking 모드일때에는
		//클라이언트에서 접속 하는 함수가 잠깐 기다려 주기 때문에 쓰레드로 돌릴 수 있다.
		//만약 서버 초기화가 길거나 클라이언트에서 접속하는 함수가 기다려 주지 않는다면 
		//클라이언트는 먼저 종료 되고 서버는 클라이언트를 영원히 기다리는 상태가 된다.

		//nonblocking 모드일때에는
		std::thread t1{ ExcuteTCPServer, "192.168.0.125:23015" };
		std::thread t2{ ExcuteTCPClient, "192.168.0.125:23015" };
		
		t1.join();
		t2.join();
	}

	void UDPServer(const std::string& serverAddr)
	{
		std::unique_ptr<UDPProtocol> udpServer = CreateUDPServer(serverAddr);
		EXPECT_NE(udpServer, nullptr);

		SocketAddress clientAddr{};
		int32_t recvBytes{ 0 };
		std::array<void*, DEFAULT_BUFLEN> recvbuf{};
		EXPECT_TRUE(udpServer->ReceiveFrom(recvbuf.data(), DEFAULT_BUFLEN, clientAddr, &recvBytes));

		gcout << "Receive Bytes : " << recvBytes << std::endl;
	}

	void UDPClient(const std::string& serverAddr)
	{
		std::this_thread::sleep_for(100ms); //서버가 먼저 돌게끔 클라이언트 쓰레드는 잠시 멈춘다.
		std::unique_ptr<UDPProtocol> udpClient = CreateUDPClient();
		EXPECT_NE(udpClient, nullptr);

		SocketAddress addr(serverAddr);
		std::string sendbuf{ "this is a test" };
		int32_t sentBytes{ 0 };
		EXPECT_TRUE(udpClient->SendTo(sendbuf.c_str(), sendbuf.size(), addr, &sentBytes));
	}

	TEST(UDPProtocol, Test)
	{
		std::thread t1{ UDPServer, ":28015" };
		std::thread t2{ UDPClient, "192.168.0.125:28015" };
		
		t1.join();
		t2.join();
	}
}
