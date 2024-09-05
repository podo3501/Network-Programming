#pragma once

//컴파일 타임에 적용할 수 있도록 함. 그 숫자가 표현될 수 있는 최소한의 비트수를 계산해준다.
template <typename E>
constexpr auto
EtoV(E enumerator) noexcept
{
	return static_cast<std::underlying_type_t<E>>(enumerator);
}

constexpr int GetRequiredBits(unsigned int n) 
{
	return (n == 0 || n == 1) ? 1 : 1 + GetRequiredBits(n >> 1);
}

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

	void Resize(std::uint32_t size);

	template<typename T>
	void Read(T& data, std::size_t bitCount = sizeof(T) * 8);

	std::size_t GetRemainingBitCount() const 
	{ 
		return (m_buffer->size() * 8) - m_bitHead; 
	}
	std::uint8_t* GetBufferPtr() const { return m_buffer->data(); }
	std::size_t GetBitLength() const { return m_bitHead; }
	std::size_t GetByteLength() const { return (m_bitHead + 7) >> 3; }	 //7을 플러스 하는 이유는 1비트라도 있으면 1Byte로 취급

	void ReadBytes(void* outData, std::size_t byteCount) { ReadBits(outData, byteCount << 3); } // * 8의 효과 (byte->bit 변환)

private:
	void ReadBits(std::uint8_t& outData, std::size_t bitCount);
	void ReadBits(void* data, size_t bitCount);

	std::shared_ptr<std::vector<std::uint8_t>> m_buffer;
	std::size_t m_bitHead;
};

template<typename T>
void InputMemoryBitStream::Read(T& data, std::size_t bitCount)
{
	static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Generic Write only supports primitive data types");

	ReadBits(&data, bitCount);
}