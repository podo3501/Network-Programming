#pragma once

class OutputMemoryBitStream
{
public:
	OutputMemoryBitStream();
	~OutputMemoryBitStream();

	OutputMemoryBitStream(const OutputMemoryBitStream&) = delete;
	OutputMemoryBitStream& operator=(const OutputMemoryBitStream&) = delete;

	template<typename T>
	void Write(T data, std::size_t bitCount = sizeof(T) * 8);

	std::uint8_t* GetBufferPtr() const { return m_buffer->data(); }
	std::size_t GetBitLength() const { return m_bitHead; }
	std::size_t GetByteLength() const { return (m_bitHead + 7) >> 3; }	 //7을 플러스 하는 이유는 1비트라도 있으면 1Byte로 취급

	void WriteBytes(const void* data, std::size_t byteCount) { WriteBits(data, byteCount << 3); } // * 8의 효과 (byte->bit 변환)

private:
	void WriteBits(std::uint8_t data, std::size_t bitCount);
	void WriteBits(const void* data, std::size_t bitCount);

	std::shared_ptr<std::vector<std::uint8_t>> m_buffer;
	std::size_t m_bitHead;
};

template<typename T>
void OutputMemoryBitStream::Write(T data, std::size_t bitCount)
{
	static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Generic Write only supports primitive data types");

	WriteBits(&data, bitCount);
}

//////////////////////////////////////////////////////////

class InputMemoryBitStream
{
public:
	InputMemoryBitStream(std::size_t size);
	~InputMemoryBitStream();

	InputMemoryBitStream() = delete;
	InputMemoryBitStream(const InputMemoryBitStream&) = delete;
	InputMemoryBitStream& operator=(const InputMemoryBitStream&) = delete;

	std::uint8_t* GetBufferPtr() const { return m_buffer->data(); }
	std::size_t GetBitLength() const { return m_bitHead; }
	std::size_t GetByteLength() const { return (m_bitHead + 7) >> 3; }	 //7을 플러스 하는 이유는 1비트라도 있으면 1Byte로 취급

private:
	std::shared_ptr<std::vector<std::uint8_t>> m_buffer;
	std::size_t m_bitHead;
};