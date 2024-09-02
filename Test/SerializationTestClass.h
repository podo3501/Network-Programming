#pragma once
#include "../Serialization/MemoryStream.h"

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

////////////////////////////////////////////////////////////////////////
//MemoryStream 테스트 클래스
class CMemoryStreamTest
{
public:
	CMemoryStreamTest();
	CMemoryStreamTest(int value, float pos, std::vector<double> list);

	void Write(OutputMemoryStream& oms);
	void Read(InputMemoryStream& ims);

	bool operator==(const CMemoryStreamTest& rhs) const;

private:
	int m_value;
	float m_pos;
	std::vector<double> m_list;
};

////////////////////////////////////////////////////////////////////////
//MemoryBitStream 테스트 클래스
class CMemoryBitStreamTest
{
public:
	CMemoryBitStreamTest();
	CMemoryBitStreamTest(int bit1, int bit2);

	void WriteBit(OutputMemoryBitStream& ombs);
	void ReadBit(InputMemoryBitStream& imbs);

	bool operator==(const CMemoryBitStreamTest& rhs) const;

private:
	int m_bitTest1;
	int m_bitTest2;
};

////////////////////////////////////////////////////////////////////////
//Serialize를 사용한 MemoryStream 테스트 클래스
class CUsingSerializeTest
{
public:
	CUsingSerializeTest();
	CUsingSerializeTest(int value, float pos, std::vector<double> list);

	template<typename Stream>
	void Serialize(MemoryStream<Stream>* memoryStream)
	{
		memoryStream->Serialize(m_value);
		memoryStream->Serialize(m_pos);
		memoryStream->Serialize(m_list);
	}

	bool operator==(const CUsingSerializeTest& rhs) const;

private:
	int m_value;
	float m_pos;
	std::vector<double> m_list;
};

////////////////////////////////////////////////////////////////////////
//Reflection를 사용한 MemoryStream 테스트 클래스
class CUsingReflectionTest : public Reflection
{
public:
	CUsingReflectionTest();
	CUsingReflectionTest(int value, float pos, std::vector<double> list);

	virtual void InitDataType() override;

	bool operator==(const CUsingReflectionTest& rhs) const;

private:
	int m_value;
	float m_pos;
	std::vector<double> m_list;
};

class GameObject;
class LinkingContext;
////////////////////////////////////////////////////////////////////////
//LinkingContext를 사용한 MemoryBitStream 테스트 클래스
class CUsingLinkingContextTest
{
public:
	CUsingLinkingContextTest();
	CUsingLinkingContextTest(int a, int b);

	void Write(OutputMemoryBitStream& ombs);

private:
	void WriteBit(OutputMemoryBitStream& ombs, GameObject* gameObject);
	void ReadBit(InputMemoryBitStream& imbs);

	std::unique_ptr<LinkingContext> m_linkingContext;

	std::shared_ptr<GameObject> m_a1;
	std::shared_ptr<GameObject> m_a2;
	std::shared_ptr<GameObject> m_b1;
};