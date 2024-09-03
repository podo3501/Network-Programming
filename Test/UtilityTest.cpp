#include "gtest/gtest.h"
#include "../Serialization/Endian.h"
#include "../Serialization/LinkingContext.h"
#include "../Serialization/MemoryBitStream.h"
#include "../Serialization/Type.h"
#include <memory>

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

	///////////////////////////////////////////////////////////////////////////
	//LinkingContext. Ŭ������ �������� ������ ���� ��� �����Ϳ� ������ ��Ʈ��ũ ID�� �ο��ϰ�
	//�޾��� ��� �� ��Ʈ��ũ ���̵�� 
	TEST(LinkingContext, Test)
	{
		//�׽�Ʈ�� �ҷ��� ���� �ᱹ Ŭ������ ������ִ� ObjectCreationRegistry���� ������ �Ѵ�.
		//�׷��� �̰� �� ����� ���� ���÷��� �ý��ۿ� �����ϴ� ������� ������ �����̴�.
	
		//auto gameObjectPtr1 = std::make_unique<GameObject>();
		//GameObject* a1 = gameObjectPtr1.get();
		//GameObject* a2 = gameObjectPtr1.get();

		//auto gameObjectPtr2 = std::make_unique<GameObject>();
		//GameObject* b1 = gameObjectPtr2.get();

		//LinkingContext linkingContext;
		//EXPECT_EQ(
		//	linkingContext.GetNetworkID(a1, true),
		//	linkingContext.GetNetworkID(a2, true));
		//EXPECT_NE(
		//	linkingContext.GetNetworkID(a1, true),
		//	linkingContext.GetNetworkID(b1, true));
	}
}