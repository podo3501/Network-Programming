#include "ClientTest.h"
#include "SerializationTestClass.h"
#include "../Serialization/MemoryBitStream.h"
#include "../Serialization/ReplicationManager.h"
#include "../Serialization/ObjectCreationRegistry.h"
#include "../Serialization/Type.h"
#include <vector>

using ::testing::InSequence;
using ::testing::Invoke;
using ::testing::_;

namespace Client
{
    void FixtureSerialization::SetUp()
    {
        m_readStream = std::make_unique<InputMemoryStream>(4096);

        m_mockTcpClient.Connect("");
    }

    void FixtureSerialization::TearDown()
    {
        m_mockTcpClient.Shutdown(SD_SEND);
    }

    //패킷이 서버에서 클라이언트로 간다고 하는 가짜 함수
    inline void MockSendPacket(std::uint8_t* sendData, size_t len, void* recvData)
    {
        std::copy(sendData, sendData + len, reinterpret_cast<std::uint8_t*>(recvData));
    }

    //////////////////////////////////////////////////////////////////////
    //간단한 데이터 전송
    TEST_F(FixtureSerialization, MemoryStream)
    {
        CMemoryStreamTest toMS(12, 3.5f, { 3.5, 4.5 });

        auto MockReceive = [&toMS](void* data, size_t len, int32_t* recvBytes)->bool {
            OutputMemoryStream writeStream;
            toMS.Write(writeStream);

            size_t realLength = min(len, writeStream.GetBufferSize());
            MockSendPacket(writeStream.GetBufferPtr(), realLength, data);
            if (recvBytes != nullptr) (*recvBytes) = static_cast<int32_t>(writeStream.GetBufferSize());

            return true;
            };

        EXPECT_CALL(m_mockTcpClient, Receive(_, _, _)).WillOnce(Invoke(MockReceive));

        InputMemoryStream* readStream = static_cast<InputMemoryStream*>(m_readStream.get());
        int32_t recvBytes = 0;
        m_mockTcpClient.Receive(readStream->GetBufferPtr(), 4096, &recvBytes);

        CMemoryStreamTest fromMS;
        fromMS.Read(*readStream);

        EXPECT_EQ(fromMS, toMS);
    }

    //////////////////////////////////////////////////////////////////////
    //전송량을 줄이기 위해서 bit 전송
    TEST_F(FixtureSerialization, MemoryBitStream)
    {
        CMemoryBitStreamTest toMBS(13, 52);

        auto MockReceive = [&toMBS](void* data, size_t len, int32_t* recvBytes)->bool {
            OutputMemoryBitStream writeStream;
            toMBS.WriteBit(writeStream);

            size_t realLength = min(len, writeStream.GetByteLength());
            MockSendPacket(writeStream.GetBufferPtr(), realLength, data);
            if (recvBytes != nullptr) (*recvBytes) = static_cast<int32_t>(writeStream.GetByteLength());

            return true;
            };

        EXPECT_CALL(m_mockTcpClient, Receive(_, _, _)).WillOnce(Invoke(MockReceive));

        InputMemoryBitStream readStream(4096);
        int32_t recvBytes = 0;
        m_mockTcpClient.Receive(readStream.GetBufferPtr(), 4096, &recvBytes);
        readStream.Resize(recvBytes);   //실제 데이터가 있는 크기만큼 남기고 공간을 줄임

        CMemoryBitStreamTest fromMBS;
        fromMBS.ReadBit(readStream);

        EXPECT_EQ(fromMBS, toMBS);
    }

    //////////////////////////////////////////////////////////////////////
    //Serialize함수를 써서 Read Write 함수를 한군데서 처리하도록 함.
    TEST_F(FixtureSerialization, MemoryStreamUsingSerialize)
    {
        CUsingSerializeTest toUsingSerialize(12, 3.5f, { 3.5, 4.5 });

        auto MockReceive = [&toUsingSerialize](void* data, size_t len, int32_t* recvBytes)->bool {
            auto writeStream = std::make_unique<OutputMemoryStream>();
            toUsingSerialize.Serialize(writeStream.get());

            size_t realLength = min(len, writeStream->GetBufferSize());
            MockSendPacket(writeStream->GetBufferPtr(), realLength, data);
            if (recvBytes != nullptr) (*recvBytes) = static_cast<int32_t>(writeStream->GetBufferSize());

            return true;
            };

        EXPECT_CALL(m_mockTcpClient, Receive(_, _, _)).WillOnce(Invoke(MockReceive));

        InputMemoryStream* readStream = static_cast<InputMemoryStream*>(m_readStream.get());
        int32_t recvBytes = 0;
        m_mockTcpClient.Receive(readStream->GetBufferPtr(), 4096, &recvBytes);

        CUsingSerializeTest fromUsingSerialize;
        fromUsingSerialize.Serialize(readStream);

        EXPECT_EQ(fromUsingSerialize, toUsingSerialize);
    }

    //////////////////////////////////////////////////////////////////////
    //리플렉션(자신이 어떤 변수를 가지고 있는지 알 수 있게 함)을 
    // Read Write 하는 변수를 일괄적으로 처리함.
    TEST_F(FixtureSerialization, MemoryStreamUsingReflection)
    {
        CUsingReflectionTest toUsingReflection(12, 3.5f, { 3.5, 4.5 });

        auto MockReceive = [&toUsingReflection](void* data, size_t len, int32_t* recvBytes)->bool {
            auto writeStream = std::make_unique<OutputMemoryStream>();
            toUsingReflection.SerializeRef(writeStream.get(), toUsingReflection);

            size_t realLength = min(len, writeStream->GetBufferSize());
            MockSendPacket(writeStream->GetBufferPtr(), realLength, data);
            if (recvBytes != nullptr) (*recvBytes) = static_cast<int32_t>(writeStream->GetBufferSize());

            return true;
            };

        EXPECT_CALL(m_mockTcpClient, Receive(_, _, _)).WillOnce(Invoke(MockReceive));

        InputMemoryStream* readStream = static_cast<InputMemoryStream*>(m_readStream.get());
        int32_t recvBytes = 0;
        m_mockTcpClient.Receive(readStream->GetBufferPtr(), 4096, &recvBytes);

        CUsingReflectionTest fromUsingReflection;
        fromUsingReflection.SerializeRef(readStream, fromUsingReflection);

        EXPECT_EQ(fromUsingReflection, toUsingReflection);
    }

    //////////////////////////////////////////////////////////////////////
    //LinkingContext. 클래스에 포인터형 변수가 있을 경우 데이터에 임의의 네트워크 ID를 부여하고
	//클라이언트에서 받았을때 네트워크 아이디와 어떤 클래스인지 확인해서 같은 형태의 클래스를 생성한다음
    //변수에 할당함. 
    TEST_F(FixtureSerialization, MemoryBitStreamUsingLinkingContext)
    {
        TestLinkingData data1(1);
        CUsingLinkingContextTest toUsingLinkingContext(&data1);
        std::vector<GameObject*> objectList{ &data1, &toUsingLinkingContext };
        //패킷을 날리고 동작하는지 테스트 해 보자
        auto MockReceive = [&objectList](void* data, size_t len, int32_t* recvBytes)->bool {
            ReplicationManager replicationMng;
            replicationMng.GetRegistry()->RegisterCreationFunction<CUsingLinkingContextTest>();
            replicationMng.GetRegistry()->RegisterCreationFunction<TestLinkingData>();

            OutputMemoryBitStream writeStream;
            replicationMng.ReplicateWorldState(writeStream, objectList);

            size_t realLength = min(len, writeStream.GetByteLength());
            MockSendPacket(writeStream.GetBufferPtr(), realLength, data);
            if (recvBytes != nullptr) (*recvBytes) = static_cast<int32_t>(writeStream.GetByteLength());

            return true;
            };

        EXPECT_CALL(m_mockTcpClient, Receive(_, _, _)).WillOnce(Invoke(MockReceive));

        InputMemoryBitStream readStream(4096);
        int32_t recvBytes = 0;
        m_mockTcpClient.Receive(readStream.GetBufferPtr(), 4096, &recvBytes);
        readStream.Resize(recvBytes);   //실제 데이터가 있는 크기만큼 남기고 공간을 줄임

        ReplicationManager replicationMng;
        replicationMng.GetRegistry()->RegisterCreationFunction<CUsingLinkingContextTest>();
        replicationMng.GetRegistry()->RegisterCreationFunction<TestLinkingData>();

        std::uint32_t packetType{ 0 };
        readStream.Read(packetType, GetRequiredBits(EtoV(PacketType::PT_Max)));
        EXPECT_EQ(static_cast<PacketType>(packetType), PacketType::PT_ReplicationData);

        replicationMng.ReceiveWorld(readStream);
    }
}