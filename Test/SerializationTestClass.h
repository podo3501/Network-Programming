#pragma once
#include "../Serialization/MemoryStream.h"
#include "../Serialization/LinkingContext.h"

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

////////////////////////////////////////////////////////////////////////

class TestLinkingData : public GameObject
{
public:
	CLASS_IDENTIFICATION('TDAT', TestLinkingData)

	TestLinkingData() = default;
	TestLinkingData(int a) : m_a{ a } {};

	virtual void WriteBit(OutputMemoryBitStream& ombs, LinkingContext* linkingContext) override;
	virtual void ReadBit(InputMemoryBitStream& imbs, LinkingContext* linkingContext) override;

private:
	int m_a{ 0 };
};

////////////////////////////////////////////////////////////////////////
//LinkingContext를 사용한 MemoryBitStream 테스트 클래스
class CUsingLinkingContextTest : public GameObject
{
public:
	CLASS_IDENTIFICATION('ULCT', CUsingLinkingContextTest)

	CUsingLinkingContextTest();
	CUsingLinkingContextTest(GameObject* data);

	virtual void WriteBit(OutputMemoryBitStream& ombs, LinkingContext* linkingContext) override;
	virtual void ReadBit(InputMemoryBitStream& imbs, LinkingContext* linkingContext) override;

private:
	GameObject* m_a1;
};