#include "pch.h"
#include "Serialization.h"
#include "../Serialization/MemoryStream.h"
#include "../Serialization/MemoryBitStream.h"
#include "SerializationTestClass.h"

namespace Serialization
{
	void FixtureMemoryStream::SetUp()
	{
		
	}

	void FixtureMemoryStream::TearDown()
	{}

	TEST_F(FixtureMemoryStream, ExcuteInoutMemoryStream)
	{
		OutputMemoryStream oms;

		CMSTest toMS(12, 3.5f, 0, 0, { 3.5, 4.5 }, 3);
		toMS.Write(oms);

		//테스트용으로 간단하게 copy했지만 원래대로면 패킷으로 send, receive 해야 한다.
		InputMemoryStream ims(4096);
		std::copy(oms.GetBufferPtr(), oms.GetBufferPtr() + oms.GetBufferSize(), ims.GetBufferPtr());

		CMSTest fromMS;
		fromMS.Read(ims);

		EXPECT_EQ(toMS, fromMS);
	}

	TEST_F(FixtureMemoryStream, ExcuteMemoryStream)
	{
		std::unique_ptr<MemoryStream<OutputMemoryStream>> writeStream = std::make_unique<OutputMemoryStream>();

		CMSTest toMS(12, 3.5f, 0, 0, { 3.5, 4.5 }, 3);
		toMS.Serialize(writeStream.get());

		std::unique_ptr<MemoryStream<InputMemoryStream>> readStream = std::make_unique<InputMemoryStream>(4096);
		std::copy(writeStream->GetBufferPtr(), writeStream->GetBufferPtr() + writeStream->GetBufferSize(), readStream->GetBufferPtr());

		CMSTest fromMS;
		fromMS.Serialize(readStream.get());

		EXPECT_EQ(toMS, fromMS);
	}

	TEST_F(FixtureMemoryStream, ExcuteReflectionStream)
	{
		std::unique_ptr<MemoryStream<OutputMemoryStream>> writeStream = std::make_unique<OutputMemoryStream>();
		CMSTest toMS(12, 3.5f, 0, 0, { 3.5, 4.5 }, 3);

		toMS.SerializeRef(writeStream.get(), toMS);

		std::unique_ptr<MemoryStream<InputMemoryStream>> readStream = std::make_unique<InputMemoryStream>(4096);
		std::copy(writeStream->GetBufferPtr(), writeStream->GetBufferPtr() + writeStream->GetBufferSize(), readStream->GetBufferPtr());

		CMSTest fromMS;
		fromMS.SerializeRef(readStream.get(), fromMS);

		EXPECT_EQ(toMS, fromMS);
	}


	TEST_F(FixtureMemoryStream, ExcuteMemoryBitStream)
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