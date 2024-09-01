#include "ClientTest.h"
#include "SerializationTestClass.h"

using ::testing::InSequence;
using ::testing::Invoke;
using ::testing::_;

namespace Client
{
    void FixtureClient::SetUp()
    {
        m_mockTcpClient.Connect("");
    }

    void FixtureClient::TearDown()
    {
        m_mockTcpClient.Shutdown(SD_SEND);
    }

    bool MockReceive(void* data, size_t len, int32_t* recvBytes)
    {
        CMSTest toMS(12, 3.5f, 0, 0, { 3.5, 4.5 }, 3);
        OutputMemoryStream writeStream;
        toMS.Write(writeStream);

        size_t realLength = min(len, writeStream.GetBufferSize());
        std::copy(writeStream.GetBufferPtr(), writeStream.GetBufferPtr() + realLength, reinterpret_cast<std::uint8_t*>(data));
        if (recvBytes != nullptr) (*recvBytes) = static_cast<int32_t>(writeStream.GetBufferSize());

        return true;
    }

    TEST_F(FixtureClient, MemoryStream)
    {
        EXPECT_CALL(m_mockTcpClient, Receive(_, _, _)).WillOnce(Invoke(MockReceive));

        InputMemoryStream readStream(4096);
        int32_t recvBytes = 0;
        m_mockTcpClient.Receive(readStream.GetBufferPtr(), 4096, &recvBytes);

        CMSTest fromMS;
        fromMS.Read(readStream);

        CMSTest expectMS(12, 3.5f, 0, 0, { 3.5, 4.5 }, 3);
        EXPECT_EQ(fromMS, expectMS);
    }
}