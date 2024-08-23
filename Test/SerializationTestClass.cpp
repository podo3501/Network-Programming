#include "gtest/gtest.h"
#include <algorithm>
#include "SerializationTestClass.h"
#include "../Serialization/MemoryStream.h"
#include "../Serialization/MemoryBitStream.h"

bool CData::operator==(const CData& rhs) const
{
	return true;

	if (m_ID != rhs.m_ID) return false;

	return true;
}

CMSTest::CMSTest() : m_value{ 0 }, m_pos{ 0.f }, m_bitTest1{ 0 }, m_bitTest2{ 0 }, m_list{}, m_data{ std::make_unique<CData>() }
{
	InitDataType();
}

CMSTest::CMSTest(int value, float pos, int bit1, int bit2, std::vector<double> list, int id) : m_value{ value }, m_pos{ pos },
	m_bitTest1{ bit1 }, m_bitTest2{ bit2 }, m_list{ list }, m_data{ std::make_unique<CData>(id) }
{
	InitDataType();
}

void CMSTest::InitDataType()
{
	std::initializer_list<MemberVariable> mvList = {
		MemberVariable("m_value", MemberType::MT_Int, offsetof(CMSTest, m_value)),
		MemberVariable("m_pos", MemberType::MT_Float, offsetof(CMSTest, m_pos)),
		MemberVariable("m_list", MemberType::MT_VectorDouble, offsetof(CMSTest, m_list)),
	};
	m_dataType = std::make_unique<DataType>(mvList);
}

void CMSTest::Write(OutputMemoryStream& oms)
{
	oms.Write(m_value);
	oms.Write(m_pos);
	oms.Write(m_list);
}

void CMSTest::Read(InputMemoryStream& ims)
{
	ims.Read(m_value);
	ims.Read(m_pos);
	ims.Read(m_list);
}

void CMSTest::WriteBit(OutputMemoryBitStream& ombs)
{
	ombs.Write(m_value);
	ombs.Write(m_bitTest1, 5);
	ombs.Write(m_bitTest2, 6);
	ombs.Write(m_pos);
}

void CMSTest::ReadBit(InputMemoryBitStream& imbs)
{
	imbs.Read(m_value);
	imbs.Read(m_bitTest1, 5);
	imbs.Read(m_bitTest2, 6);
	imbs.Read(m_pos);
}

bool CMSTest::operator==(const CMSTest& rhs) const
{
	if (m_value != rhs.m_value) return false;
	if (m_pos != rhs.m_pos) return false;
	if (m_bitTest1 != rhs.m_bitTest1) return false;
	if (m_bitTest2 != rhs.m_bitTest2) return false;
	if (!std::ranges::equal(m_list, rhs.m_list)) return false;
	if ((*m_data) != (*rhs.m_data)) return false;

	return true;
}
