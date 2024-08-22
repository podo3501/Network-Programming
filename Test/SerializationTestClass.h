#pragma once
#include "../Serialization/MemoryStream.h"

class GameObject
{
public:
	GameObject() {};
	virtual ~GameObject() {}
};

class CData : public GameObject
{
public:
	CData() : m_ID{ 0 } {}
	CData(int id) : m_ID{ id } {}

	bool operator==(const CData& rhs) const;
private:
	int m_ID;
};

class OutputMemoryBitStream;
class InputMemoryBitStream;

class Reflection
{
public:
	Reflection() : m_dataType{ nullptr } {}
	virtual ~Reflection() {};
	virtual void InitDataType() = 0;
	DataType* Get() { return m_dataType.get(); }

	template<typename Stream, typename T>
	void SerializeRef(MemoryStream<Stream>* memoryStream, T& offset)
	{
		if (m_dataType != nullptr)
			memoryStream->Serialize(m_dataType.get(), reinterpret_cast<std::uint8_t*>(&offset));
	}

protected:
	std::unique_ptr<DataType> m_dataType;
};

class CMSTest : public Reflection
{
public:
	CMSTest();
	CMSTest(int value, float pos, int bit1, int bit2, std::vector<double> list, int id);

	virtual void InitDataType() override;

	void Write(OutputMemoryStream& oms);
	void Read(InputMemoryStream& ims);

	template<typename Stream>
	void Serialize(MemoryStream<Stream>* memoryStream)
	{
		memoryStream->Serialize(m_value);
		memoryStream->Serialize(m_pos);
		memoryStream->Serialize(m_list);
	}

	void WriteBit(OutputMemoryBitStream& ombs);
	void ReadBit(InputMemoryBitStream& imbs);

	int Get() { return m_value; }
	bool operator==(const CMSTest& rhs) const;

private:
	int m_value;
	float m_pos;
	std::vector<double> m_list;
	int m_bitTest1;
	int m_bitTest2;
	std::unique_ptr<CData> m_data;
};