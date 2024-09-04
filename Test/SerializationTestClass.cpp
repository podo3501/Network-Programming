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

void TestLinkingData::WriteBit(OutputMemoryBitStream& ombs, LinkingContext* linkingContext)
{
	ombs.Write(m_a, 4);	// 값의 범위(0 - 15)
}

void TestLinkingData::ReadBit(InputMemoryBitStream& imbs, LinkingContext* linkingContext)
{
	imbs.Read(m_a, 4);
}

////////////////////////////////////////////////////////////////////////

CUsingLinkingContextTest::CUsingLinkingContextTest() : 
	m_a1{ nullptr }
{}


CUsingLinkingContextTest::CUsingLinkingContextTest(GameObject* data) :
	m_a1{ data }
{}

void CUsingLinkingContextTest::WriteBit(OutputMemoryBitStream& ombs, LinkingContext* linkingContext)
{
	ombs.Write(linkingContext->GetNetworkID(m_a1, true));
}

void CUsingLinkingContextTest::ReadBit(InputMemoryBitStream& imbs, LinkingContext* linkingContext)
{
	std::uint32_t networkID{ 0 };
	imbs.Read(networkID);
	m_a1 = linkingContext->GetGameObject(networkID);
}