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
	//LinkingContext. 클래스에 포인터형 변수가 있을 경우 데이터에 임의의 네트워크 ID를 부여하고
	//받았을 경우 그 네트워크 아이디로 
	TEST(LinkingContext, Test)
	{
		//테스트를 할려고 보니 결국 클래스를 만들어주는 ObjectCreationRegistry까지 만들어야 한다.
		//그래서 이걸 다 만들고 나서 리플렉션 시스템에 구현하는 방식으로 진행할 예정이다.
	
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