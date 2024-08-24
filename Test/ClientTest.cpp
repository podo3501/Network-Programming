#include "gmock/gmock.h"
#include "../Include/NetworkInterface.h"
#include "SerializationTestClass.h"

namespace Client
{
    class FakeTCPClient : public TCPClient
    {
    public:
        virtual bool Connect(const std::string& serverAddress) { return true; }
        virtual bool Send(const void* data, size_t len, int32_t* recvBytes) { return true; }
        virtual bool Receive(void* data, size_t len, int32_t* recvBytes) override;
        virtual bool Shutdown(int shutdownFlag = SD_SEND) { return true; }
    };

    bool FakeTCPClient::Receive(void* data, size_t len, int32_t* recvBytes)
    {
        CMSTest toMS(12, 3.5f, 0, 0, { 3.5, 4.5 }, 3);
        OutputMemoryStream writeStream;
        toMS.Write(writeStream);

        size_t realLength = min(len, writeStream.GetBufferSize());
        std::copy(writeStream.GetBufferPtr(), writeStream.GetBufferPtr() + realLength, reinterpret_cast<std::uint8_t*>(data));
        if (recvBytes != nullptr) (*recvBytes) = static_cast<int32_t>(writeStream.GetBufferSize());

        return true; 
    }

    class MockTCPClient : public TCPClient
    {
    public:
        FakeTCPClient fake;

        MockTCPClient() { DelegateToFake(); }

        MOCK_METHOD(bool, Connect, (const std::string& serverAddress), (override));
        MOCK_METHOD(bool, Send, (const void* data, size_t len, int32_t* recvBytes), (override));
        MOCK_METHOD(bool, Receive, (void* data, size_t len, int32_t* recvBytes), (override));
        MOCK_METHOD(bool, Shutdown, (int shutdownFlag), (override));

    private:
        void DelegateToFake()
        {
            using ::testing::_;
            ON_CALL(*this, Receive(_, _, _)).WillByDefault(::testing::Invoke(&fake, &FakeTCPClient::Receive));
        }
    };

    TEST(MockTCPSocket, Test)
    {
        MockTCPClient mockTcpClient;
        EXPECT_CALL(mockTcpClient, Connect(testing::_)).Times(1);
        EXPECT_CALL(mockTcpClient, Shutdown(testing::_)).Times(1);

        mockTcpClient.Connect("");

        InputMemoryStream readStream(4096);
        int32_t recvBytes = 0;
        mockTcpClient.Receive(readStream.GetBufferPtr(), 4096, &recvBytes);

        mockTcpClient.Shutdown(SD_SEND);

        CMSTest fromMS;
        fromMS.Read(readStream);

        CMSTest expectMS(12, 3.5f, 0, 0, { 3.5, 4.5 }, 3);
        EXPECT_EQ(fromMS, expectMS);
    }
}