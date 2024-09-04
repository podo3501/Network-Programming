#include "gtest/gtest.h"
#include <memory>
#include "../Serialization/Endian.h"
#include "../Serialization/LinkingContext.h"
#include "../Serialization/MemoryBitStream.h"
#include "../Serialization/Type.h"
#include "../Serialization/ReplicationManager.h"

namespace NetworkUtility
{
	TEST(ExcuteEndian, Test)
	{
		std::int32_t test = 0x12345678;
		auto test32 = ByteSwap(test);
		EXPECT_EQ(test32, 0x78563412);

		std::int32_t intTest = 1;
		auto testInt = TypeAliaser<int, std::uint32_t>(ByteSwap(intTest)).Get();
		EXPECT_EQ(testInt, 0x01000000);
	}

	TEST(ExcuteGetRequiredBits, Test)
	{
		EXPECT_EQ(GetRequiredBits(4), 3);
		EXPECT_EQ(GetRequiredBits(3), 2);
		EXPECT_EQ(GetRequiredBits(1), 1);
		EXPECT_EQ(GetRequiredBits(0), 1);
	}
}