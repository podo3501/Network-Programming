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

	uint8_t currentMask = ~(0xff << bitOffset);	//현재 값이 있다면 덮어쓰지 않기 위해서 마스크를 만든다.
	(*m_buffer)[byteOffset] = ((*m_buffer)[byteOffset] & currentMask) | (data << bitOffset);
	
	size_t bitsFreeThisByte = 8 - bitOffset;	//기록할 바이트가 있는지 확인
	
	//데이터가 기록할(최대8비트) 비트보다 커서 넘어가야 한다면
	//그 다음 바이트에 기록한다. |---2---|-1-|  => |-1-|--------|  |---|---2---|
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

	size_t bitsReadThisByte = 8 - bitOffset;	//더 읽어들여할 바이트가 있는가
	if (bitsReadThisByte < bitCount)
		outData |= static_cast<uint8_t>((*m_buffer)[byteOffset + 1] << bitsReadThisByte);

	//5비트라면 8비트중 3비트가 필요없는 것이기 때문에 마스크로 제거한다.
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


