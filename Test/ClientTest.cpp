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

    //��Ŷ�� �������� Ŭ���̾�Ʈ�� ���ٰ� �ϴ� ��¥ �Լ�
    inline void MockSendPacket(std::uint8_t* sendData, size_t len, void* recvData)
    {
        std::copy(sendData, sendData + len, reinterpret_cast<std::uint8_t*>(recvData));
    }

    //////////////////////////////////////////////////////////////////////
    //������ ������ ����
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
    //���۷��� ���̱� ���ؼ� bit ����
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
        readStream.Resize(recvBytes);   //���� �����Ͱ� �ִ� ũ�⸸ŭ ����� ������ ����

        CMemoryBitStreamTest fromMBS;
        fromMBS.ReadBit(readStream);

        EXPECT_EQ(fromMBS, toMBS);
    }

    //////////////////////////////////////////////////////////////////////
    //Serialize�Լ��� �Ἥ Read Write �Լ��� �ѱ����� ó���ϵ��� ��.
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
    //���÷���(�ڽ��� � ������ ������ �ִ��� �� �� �ְ� ��)�� 
    // Read Write �ϴ� ������ �ϰ������� ó����.
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
    //LinkingContext. Ŭ������ �������� ������ ���� ��� �����Ϳ� ������ ��Ʈ��ũ ID�� �ο��ϰ�
	//Ŭ���̾�Ʈ���� �޾����� ��Ʈ��ũ ���̵�� � Ŭ�������� Ȯ���ؼ� ���� ������ Ŭ������ �����Ѵ���
    //������ �Ҵ���. 
    TEST_F(FixtureSerialization, MemoryBitStreamUsingLinkingContext)
    {
        TestLinkingData data1(1);
        CUsingLinkingContextTest toUsingLinkingContext(&data1);
        std::vector<GameObject*> objectList{ &data1, &toUsingLinkingContext };
        //��Ŷ�� ������ �����ϴ��� �׽�Ʈ �� ����
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
        readStream.Resize(recvBytes);   //���� �����Ͱ� �ִ� ũ�⸸ŭ ����� ������ ����

        ReplicationManager replicationMng;
        replicationMng.GetRegistry()->RegisterCreationFunction<CUsingLinkingContextTest>();
        replicationMng.GetRegistry()->RegisterCreationFunction<TestLinkingData>();

        std::uint32_t packetType{ 0 };
        readStream.Read(packetType, GetRequiredBits(EtoV(PacketType::PT_Max)));
        EXPECT_EQ(static_cast<PacketType>(packetType), PacketType::PT_ReplicationData);

        replicationMng.ReceiveWorld(readStream);
    }
}