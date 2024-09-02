#pragma once
#include "gmock/gmock.h"
#include "../Include/NetworkInterface.h"
#include <memory>

template<typename T>
class MemoryStream;
class OutputMemoryStream;
class InputMemoryStream;

namespace Client
{
    class MockTCPClient : public TCPClient
    {
    public:
        MOCK_METHOD(bool, Connect, (const std::string& serverAddress), (override));
        MOCK_METHOD(bool, Send, (const void* data, size_t len, int32_t* recvBytes), (override));
        MOCK_METHOD(bool, Receive, (void* data, size_t len, int32_t* recvBytes), (override));
        MOCK_METHOD(bool, Shutdown, (int shutdownFlag), (override));
    };

    class FixtureSerialization : public ::testing::Test
    {
    protected:
        void SetUp() override;
        void TearDown() override;

    protected:
        MockTCPClient m_mockTcpClient;

        std::unique_ptr<MemoryStream<OutputMemoryStream>> m_writeStream;
        std::unique_ptr<MemoryStream<InputMemoryStream>> m_readStream;
    };
}
