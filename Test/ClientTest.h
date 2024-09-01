#pragma once
#include "gmock/gmock.h"
#include "../Include/NetworkInterface.h"
#include <memory>

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

    class FixtureClient : public ::testing::Test
    {
    protected:
        void SetUp() override;
        void TearDown() override;

        MockTCPClient m_mockTcpClient;
    };
}
