#include "gmock/gmock.h"
#include "../Include/NetworkInterface.h"

namespace Client
{
    class MockTest {
    public:
        MOCK_METHOD(void, SomeMethod, ());
    };

    TEST(TestCaseName, TestName) {
        MockTest mock;
        EXPECT_CALL(mock, SomeMethod);
        mock.SomeMethod();
        EXPECT_EQ(1, 1);
        EXPECT_TRUE(true);
    }

    class MockTCPClient : public TCPClient
    {
    public:
        MOCK_METHOD(bool, Connect, (const std::string& serverAddress), (override));
        MOCK_METHOD(bool, Send, (const void* data, size_t len, int32_t* recvBytes), (override));
        MOCK_METHOD(bool, Receive, (void* data, size_t len, int32_t* recvBytes), (override));
        //MOCK_METHOD(bool, Shutdown, (int shutdownFlag), (override));
        MOCK_METHOD1(Shutdown, bool(int shutdownFlag));
    };

    using ::testing::_;
    TEST(MockTCPSocket, Test)
    {
        std::unique_ptr<MockTCPClient> mockTcpClient = std::make_unique<MockTCPClient>();
        EXPECT_CALL(*mockTcpClient, Connect(_)).Times(1);
        //EXPECT_CALL(*mockTcpClient, Shutdown(_)).Times(1);
        mockTcpClient->Connect("");
        //mockTcpClient->Shutdown();
    }
}