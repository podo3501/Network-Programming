#include "pch.h"
#include "../Include/NetworkInterface.h"

//출력 창 - 테스트에 출력하기
#define COUT std::cout << "[   INFO   ] "

class GlobalEnv : public ::testing::Environment
{
public:
	GlobalEnv() {}
	~GlobalEnv() {}

	void SetUp()
	{
		//메모리 릭을 잡기 위한 옵션
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	}

	void TearDown()
	{}
};

//GlobalEnv 클래스의 SetUp을 해주기 위해서.
int main(int argc, char** argv)
{
	//쓰레드가 돌아갈때 강제로 종료할 경우 프로그램이 살아있는 경우가 있어서 
	//다음 테스트에 오동작 하는 경우가 있다. 그럴때를 대비해서 프로그램이 띄워져 있는지 체크
	HANDLE Mutex;
	std::wstring progMutex{ L"Network Test" }; // 프로젝트명
	if ((Mutex = OpenMutex(MUTEX_ALL_ACCESS, false, progMutex.c_str())) == NULL)
		Mutex = CreateMutex(NULL, true, progMutex.c_str());
	else
	{
		OutputDebugString(L"\nTest 실패! 프로그램이 중복 실행 되었습니다. 자동 종료 합니다.\n\n");
		return 0;
	}
	
	::testing::InitGoogleTest(&argc, argv);
	::testing::AddGlobalTestEnvironment(new GlobalEnv);

	return RUN_ALL_TESTS();
}

constexpr int DEFAULT_BUFLEN = 512;

namespace Network
{
	void TCPServer(const std::string& addr)
	{
		std::unique_ptr<Server> network = CreateServer();

		EXPECT_EQ(network->Setup(addr), true);
		EXPECT_EQ(network->PrepareTCPSocket(), true);

		int32_t recvBytes{ 0 };
		do
		{
			std::array<void*, DEFAULT_BUFLEN> recvbuf{};
			EXPECT_EQ(network->Receive(recvbuf.data(), recvbuf.size(), &recvBytes), true);
			if (recvBytes > 0)
			{
				COUT << "Server Bytes received: " << recvBytes << std::endl;
				EXPECT_EQ(network->Send(recvbuf.data(), recvBytes, nullptr), true);
			}
		} while (recvBytes > 0);

		EXPECT_EQ(network->Shutdown(), true);
	}

	void TCPClient(const std::string& addr)
	{
		std::unique_ptr<Client> network = CreateClient();

		EXPECT_EQ(network->Setup("192.168.0.125:27015"), true);
		EXPECT_EQ(network->Connect(), true);

		std::string sendbuf{ "this is a test" };
		EXPECT_EQ(network->Send(sendbuf.c_str(), sendbuf.size(), nullptr), true);

		EXPECT_EQ(network->Shutdown(), true);

		int32_t recvBytes{ 0 };
		do
		{
			std::array<void*, DEFAULT_BUFLEN> recvbuf{};
			EXPECT_EQ(network->Receive(recvbuf.data(), recvbuf.size(), &recvBytes), true);
			if (recvBytes > 0)
				COUT << "Client Bytes received: " << recvBytes << std::endl;
		} while (recvBytes > 0);
	}

	TEST(TCPSocket, Test)
	{
		//클라이언트에서 접속 하는 함수가 잠깐 기다려 주기 때문에 쓰레드로 돌릴 수 있다.
		//만약 서버 초기화가 길거나 클라이언트에서 접속하는 함수가 기다려 주지 않는다면 
		//클라이언트는 먼저 종료 되고 서버는 클라이언트를 영원히 기다리는 상태가 된다.
		std::thread t1{ TCPServer, ":27015" };
		std::thread t2{ TCPClient, "192.168.0.125:27015" };
		
		t1.join();
		t2.join();
	}

	void UDPServer()
	{

	}

	TEST(UDPSocket, Test)
	{
		//std::thread t1{ UDPServer };

		//t1.join();
	}
}
