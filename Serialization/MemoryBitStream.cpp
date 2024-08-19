#include "pch.h"
#include "MemoryBitStream.h"

using namespace std;

OutputMemoryBitStream::OutputMemoryBitStream() : m_bitHead{ 0 }
{
	m_buffer = make_shared<vector<uint8_t>>();
}

OutputMemoryBitStream::~OutputMemoryBitStream()
{}

void OutputMemoryBitStream::WriteBits(uint8_t data, size_t bitCount)
{
	size_t nextBitHead = m_bitHead + bitCount;
	if (m_buffer->capacity() < nextBitHead)
		m_buffer->resize(max(m_buffer->capacity() * 2, nextBitHead));

	size_t byteOffset = m_bitHead >> 3;
	size_t bitOffset = m_bitHead & 0x7;

	uint8_t currentMask = ~(0xff << bitOffset);	//���� ���� �ִٸ� ����� �ʱ� ���ؼ� ����ũ�� �����.
	(*m_buffer)[byteOffset] = ((*m_buffer)[byteOffset] & currentMask) | (data << bitOffset);
	
	size_t bitsFreeThisByte = 8 - bitOffset;	//����� ����Ʈ�� �ִ��� Ȯ��
	
	//�����Ͱ� �����(�ִ�8��Ʈ) ��Ʈ���� Ŀ�� �Ѿ�� �Ѵٸ�
	//�� ���� ����Ʈ�� ����Ѵ�. |---2---|-1-|  => |-1-|--------|  |---|---2---|
	if (bitsFreeThisByte < bitCount) 
		(*m_buffer)[byteOffset + 1] = data >> bitsFreeThisByte;		

	m_bitHead = nextBitHead;
}

void OutputMemoryBitStream::WriteBits(const void* data, size_t bitCount)
{
	const uint8_t* srcByte = static_cast<const uint8_t*>(data);

	while (bitCount > 8)
	{
		WriteBits(*srcByte, 8);
		++srcByte;
		bitCount -= 8;
	}

	if (bitCount > 0)
		WriteBits(*srcByte, bitCount);
}

//////////////////////////////////////////////////////////

InputMemoryBitStream::InputMemoryBitStream(std::size_t size) : m_bitHead{ 0 }
{
	m_buffer = make_shared<vector<uint8_t>>();
	m_buffer->resize(size);
}

InputMemoryBitStream::~InputMemoryBitStream()
{}

void InputMemoryBitStream::ReadBits(uint8_t& outData, size_t bitCount)
{
	size_t byteOffset = m_bitHead >> 3;
	size_t bitOffset = m_bitHead & 0x7;
	
	outData = static_cast<uint8_t>((*m_buffer)[byteOffset] >> bitOffset);

	size_t bitsReadThisByte = 8 - bitOffset;	//�� �о�鿩�� ����Ʈ�� �ִ°�
	if (bitsReadThisByte < bitCount)
		outData |= static_cast<uint8_t>((*m_buffer)[byteOffset + 1] << bitsReadThisByte);

	//5��Ʈ��� 8��Ʈ�� 3��Ʈ�� �ʿ���� ���̱� ������ ����ũ�� �����Ѵ�.
	outData &= (~(0x00ff << bitCount));

	m_bitHead += bitCount;
}

void InputMemoryBitStream::ReadBits(void* data, size_t bitCount)
{
	uint8_t* destByte = static_cast<uint8_t*>(data);

	while (bitCount > 8)
	{
		ReadBits(*destByte, 8);
		++destByte;
		bitCount -= 8;
	}

	if (bitCount > 0)
		ReadBits(*destByte, bitCount);
}


