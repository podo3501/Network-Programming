#include "gtest/gtest.h"
#include <memory>
#include <algorithm>
#include "SerializationTestClass.h"
#include "../Serialization/MemoryStream.h"
#include "../Serialization/MemoryBitStream.h"
#include "../Serialization/LinkingContext.h"

CMemoryStreamTest::CMemoryStreamTest() : m_value{ 0 }, m_pos{ 0.f }, m_list{} {};
CMemoryStreamTest::CMemoryStreamTest(int value, float pos, std::vector<double> list) :
	m_value{ value }, m_pos{ pos }, m_list{ list } {}

void CMemoryStreamTest::Write(OutputMemoryStream& oms)
{
	oms.Write(m_value);
	oms.Write(m_pos);
	oms.Write(m_list);
}

void CMemoryStreamTest::Read(InputMemoryStream& ims)
{
	ims.Read(m_value);
	ims.Read(m_pos);
	ims.Read(m_list);
}

bool CMemoryStreamTest::operator==(const CMemoryStreamTest& rhs) const
{
	if (m_value != rhs.m_value) return false;
	if (m_pos != rhs.m_pos) return false;
	if (!std::ranges::equal(m_list, rhs.m_list)) return false;

	return true;
}

////////////////////////////////////////////////////////////////////////

CMemoryBitStreamTest::CMemoryBitStreamTest() : m_bitTest1{ 0 }, m_bitTest2{ 0 } {}
CMemoryBitStreamTest::CMemoryBitStreamTest(int bit1, int bit2) :
	m_bitTest1{ bit1 }, m_bitTest2{ bit2 } {}

void CMemoryBitStreamTest::WriteBit(OutputMemoryBitStream& ombs)
{
	ombs.Write(m_bitTest1, 5);	//이 값은 5비트만 사용한다.(0-31)
	ombs.Write(m_bitTest2, 6);//이 값은 6비트만 사용한다.(0-64)
}

void CMemoryBitStreamTest::ReadBit(InputMemoryBitStream& imbs)
{
	imbs.Read(m_bitTest1, 5);
	imbs.Read(m_bitTest2, 6);
}

bool CMemoryBitStreamTest::operator==(const CMemoryBitStreamTest& rhs) const
{
	if (m_bitTest1 != rhs.m_bitTest1) return false;
	if (m_bitTest2 != rhs.m_bitTest2) return false;

	return true;
}

////////////////////////////////////////////////////////////////////////

CUsingSerializeTest::CUsingSerializeTest() : m_value{ 0 }, m_pos{ 0.f }, m_list{} {}
CUsingSerializeTest::CUsingSerializeTest(int value, float pos, std::vector<double> list) :
	m_value{ value }, m_pos{ pos }, m_list{ list } {}

bool CUsingSerializeTest::operator==(const CUsingSerializeTest& rhs) const
{
	if (m_value != rhs.m_value) return false;
	if (m_pos != rhs.m_pos) return false;
	if (!std::ranges::equal(m_list, rhs.m_list)) return false;

	return true;
}

////////////////////////////////////////////////////////////////////////

CUsingReflectionTest::CUsingReflectionTest() : m_value{ 0 }, m_pos{ 0.f }, m_list{} 
{
	InitDataType();
}

CUsingReflectionTest::CUsingReflectionTest(int value, float pos, std::vector<double> list) :
	m_value{ value }, m_pos{ pos }, m_list{ list } 
{
	InitDataType();
}

void CUsingReflectionTest::InitDataType()
{
	std::initializer_list<MemberVariable> mvList = {
		MemberVariable("m_value", MemberType::MT_Int, offsetof(CUsingReflectionTest, m_value)),
		MemberVariable("m_pos", MemberType::MT_Float, offsetof(CUsingReflectionTest, m_pos)),
		MemberVariable("m_list", MemberType::MT_VectorDouble, offsetof(CUsingReflectionTest, m_list)),
	};
	m_dataType = std::make_unique<DataType>(mvList);
}

bool CUsingReflectionTest::operator==(const CUsingReflectionTest& rhs) const
{
	if (m_value != rhs.m_value) return false;
	if (m_pos != rhs.m_pos) return false;
	if (!std::ranges::equal(m_list, rhs.m_list)) return false;

	return true;
}

////////////////////////////////////////////////////////////////////////

class TestLinkingData : public GameObject
{
public:
	TestLinkingData() = default;
	TestLinkingData(int a) : m_a{ a } {};

	virtual void WriteBit(OutputMemoryBitStream& ombs) override
	{
		ombs.Write(m_a, 4);	// 값의 범위(0 - 15)
	}

	virtual void ReadBit(InputMemoryBitStream& imbs) override
	{
		imbs.Read(m_a, 4);
	}

private:
	int m_a{ 0 };
};

CUsingLinkingContextTest::CUsingLinkingContextTest() : 
	m_linkingContext{ std::unique_ptr<LinkingContext>() }, m_a1{ nullptr }, m_a2{ nullptr }, m_b1{ nullptr }
{}
CUsingLinkingContextTest::CUsingLinkingContextTest(int a, int b) :
	m_linkingContext{ std::unique_ptr<LinkingContext>() }, 
	m_a1{ std::make_shared<TestLinkingData>(a) }, m_a2{ m_a1 }, m_b1{ std::make_shared<TestLinkingData>(b) }
{}

void CUsingLinkingContextTest::Write(OutputMemoryBitStream& ombs)
{
	WriteBit(ombs, m_a1.get());
	WriteBit(ombs, m_a2.get());
	WriteBit(ombs, m_b1.get());
}

void CUsingLinkingContextTest::WriteBit(OutputMemoryBitStream& ombs, GameObject* gameObject)
{
	//데이터의 클래스는 TestLinkingData 하나로 한다.
	ombs.Write(m_linkingContext->GetNetworkID(gameObject, true));
	gameObject->WriteBit(ombs);
}

void CUsingLinkingContextTest::ReadBit(InputMemoryBitStream& imbs)
{
	std::uint32_t networkID{ 0 };
	imbs.Read(networkID);
	GameObject* gameObject = m_linkingContext->GetGameObject(networkID);
}