#include "pch.h"
#include "../Include/NetworkInterface.h"
#include "../Network/SocketAddress.h"
#include "../Serialization/MemoryStream.h"
#include "../Serialization/MemoryBitStream.h"
#include "../Serialization/LinkingContext.h"
#include "../Serialization/Endian.h"

//출력 창 - 테스트에 출력하기
#define gcout std::cout << "[   INFO   ] "

using namespace std::chrono_literals;

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

	auto silent_run = false;
	if (silent_run) {
		auto& listeners = ::testing::UnitTest::GetInstance()->listeners();
		delete listeners.Release(listeners.default_result_printer());
	}

	return RUN_ALL_TESTS();
}

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

namespace Serialization
{
	class GameObject
	{
	public:
		GameObject() {};
		virtual ~GameObject() {}
	};

	class CData : public GameObject
	{
	public:
		CData() : m_ID{ 0 } {}
		CData(int id) : m_ID{ id } {}

		bool operator==(const CData& rhs) const
		{
			if (m_ID != rhs.m_ID) return false;

			return true;
		}
		
	private:
		int m_ID;
	};

	class CMSTest
	{
	public:
		CMSTest() : m_value{ 0 }, m_pos{ 0.f }, m_bitTest1{ 0 }, m_bitTest2{ 0 }, m_list{}, m_data{ std::make_unique<CData>() }
		{}
		CMSTest(int value, float pos, int bit1, int bit2, std::vector<double> list, int id) : m_value{ value }, m_pos{ pos }, 
			m_bitTest1{ bit1 }, m_bitTest2{ bit2 }, m_list{ list }, m_data{ std::make_unique<CData>(id) } {}
		void Write(OutputMemoryStream& oms) 
		{ 
			oms.Write(m_value); 
			oms.Write(m_pos);
			oms.Write(m_list);
		}

		void Read(InputMemoryStream& ims)
		{
			ims.Read(m_value);
			ims.Read(m_pos);
			ims.Read(m_list);
		}

		void WriteBit(OutputMemoryBitStream& ombs)
		{
			ombs.Write(m_value);
			ombs.Write(m_bitTest1, 5);
			ombs.Write(m_bitTest2, 6);
			ombs.Write(m_pos);
		}

		void ReadBit(InputMemoryBitStream& imbs)
		{
			imbs.Read(m_value);
			imbs.Read(m_bitTest1, 5);
			imbs.Read(m_bitTest2, 6);
			imbs.Read(m_pos);
		}

		int Get() { return m_value; }
		bool operator==(const CMSTest& rhs) const
		{
			if (m_value != rhs.m_value) return false;
			if (m_pos != rhs.m_pos) return false;
			if (m_bitTest1 != rhs.m_bitTest1) return false;
			if (m_bitTest2 != rhs.m_bitTest2) return false;
			if (!std::ranges::equal(m_list, rhs.m_list)) return false;
			if ((*m_data) != (*rhs.m_data)) return false;

			return true;
		}

	private:
		int m_value;
		float m_pos;
		std::vector<double> m_list;
		int m_bitTest1;
		int m_bitTest2;
		std::unique_ptr<CData> m_data;
	};
	TEST(ExcuteMemoryStream, Test)
	{
		OutputMemoryStream oms;

		CMSTest toMS(12, 3.5f, 0, 0, {3.5, 4.5}, 3);
		toMS.Write(oms);

		//테스트용으로 간단하게 copy했지만 원래대로면 패킷으로 send, receive 해야 한다.
		InputMemoryStream ims(4096);
		std::copy(oms.GetBufferPtr(), oms.GetBufferPtr() + oms.GetBufferSize(), ims.GetBufferPtr());
		
		CMSTest fromMS;
		fromMS.Read(ims);

		EXPECT_EQ(toMS, fromMS);
	}

	TEST(ExcuteMemoryBitStream, Test)
	{
		OutputMemoryBitStream ombs;

		CMSTest toMS(1, 7.532f, 13, 52, {}, 0);
		toMS.WriteBit(ombs);

		InputMemoryBitStream imbs(4096);
		std::copy(ombs.GetBufferPtr(), ombs.GetBufferPtr() + ombs.GetBitLength(), imbs.GetBufferPtr());

		CMSTest fromMS;
		fromMS.ReadBit(imbs);

		EXPECT_EQ(toMS, fromMS);
	}

	TEST(ExcuteEndian, Test)
	{
		std::int32_t test = 0x12345678;
		auto test32 = ByteSwap(test);
		EXPECT_EQ(test32, 0x78563412);

		std::int32_t intTest = 1;
		auto testInt = TypeAliaser<int, std::uint32_t>(ByteSwap(intTest)).Get();
		EXPECT_EQ(testInt, 0x01000000);
	}
}
