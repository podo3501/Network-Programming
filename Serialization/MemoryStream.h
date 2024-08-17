#pragma once

#include "Endian.h"

#define STREAM_ENDIANNESS 0
#define PLATFORM_ENDIANNESS 0

class OutputMemoryStream
{
public:
	OutputMemoryStream();
	~OutputMemoryStream();

	OutputMemoryStream(const OutputMemoryStream&) = delete;
	OutputMemoryStream& operator=(const OutputMemoryStream&) = delete;

	std::uint8_t* GetBufferPtr() const { return m_buffer->data(); }
	std::size_t GetBufferSize() const { return m_head; }

	template<typename T>
	void Write(const T& data);

private:
	void Write(const std::uint8_t* data, std::size_t size);

	std::shared_ptr<std::vector<std::uint8_t>> m_buffer;
	std::size_t m_head;
};

template<typename T>
void OutputMemoryStream::Write(const T& data)
{
	static_assert(
		std::is_arithmetic<T>::value ||
		std::is_enum<T>::value,
		"Generic Write only supports primitive data types");

	if (STREAM_ENDIANNESS == PLATFORM_ENDIANNESS)
	{
		Write(reinterpret_cast<const uint8_t*>(&data), sizeof(T));
	}
	else
	{
		T swappedData = ByteSwap(data);
		Write(reinterpret_cast<uint8_t*>(&swappedData), sizeof(T));
	}
}

class InputMemoryStream
{
public:
	InputMemoryStream(std::uint32_t size);
	~InputMemoryStream();

	InputMemoryStream(const InputMemoryStream&) = delete;
	InputMemoryStream& operator=(const InputMemoryStream&) = delete;

	template<typename T>
	void Read(T& data);

	std::uint8_t* GetBufferPtr() { return m_buffer->data(); }

private:
	void Read(std::uint8_t* data, std::size_t size);

	std::shared_ptr<std::vector<std::uint8_t>> m_buffer;
	std::size_t m_head;
};

template<typename T>
void InputMemoryStream::Read(T& data)
{
	static_assert(
		std::is_arithmetic<T>::value ||
		std::is_enum<T>::value,
		"Generic Write only supports primitive data types");

	Read(reinterpret_cast<uint8_t*>(&data), sizeof(T));
}