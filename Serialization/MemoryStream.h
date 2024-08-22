#pragma once

#include "Endian.h"
#include "Reflection.h"

#define STREAM_ENDIANNESS 0
#define PLATFORM_ENDIANNESS 0

template<typename T>
concept Primitive = std::is_arithmetic<T>::value || std::is_enum<T>::value;

template<typename Stream>
class MemoryStream
{
public:
	MemoryStream() = default;
	virtual ~MemoryStream() {};

	inline Stream& GetStream() { return static_cast<Stream&>(*this); }

	template<typename T>
	void Serialize(T& ioData) requires Primitive<T>
	{
		if (STREAM_ENDIANNESS == PLATFORM_ENDIANNESS)
		{
			GetStream().Serialize(reinterpret_cast<uint8_t*>(&ioData), sizeof(ioData));
			return;
		}

		if (static_cast<Stream&>(*this).IsInput())
		{
			T data;
			GetStream().Serialize(reinterpret_cast<uint8_t*>(&data), sizeof(T));
			ioData = ByteSwap(data);
			return;
		}

		T swappedData = ByteSwap(ioData);
		GetStream().Serialize(reinterpret_cast<uint8_t*>(&swappedData), sizeof(swappedData));
	}

	template<typename T>
	void Serialize(std::vector<T>& vector)
	{
		if (GetStream().IsInput()) GetStream().Serialize(vector);
		else GetStream().Serialize(vector);
	}

	void Serialize(const DataType* dataType, std::uint8_t* classOffset)
	{
		for (auto& mv : dataType->GetMemberVariables())
		{
			void* mvOffset = classOffset + mv.GetOffset();
			switch (mv.GetType())
			{
			case MemberType::MT_Int:
				Serialize(*(int*)mvOffset);
				break;
			case MemberType::MT_Float:
				Serialize(*(float*)mvOffset);
				break;
			case MemberType::MT_VectorDouble:
				Serialize(*(std::vector<double>*)mvOffset);
				break;
			}
		}
	}

	virtual std::uint8_t* GetBufferPtr() const = 0;
	virtual std::size_t GetBufferSize() const = 0;
	
};

class OutputMemoryStream : public MemoryStream<OutputMemoryStream>
{
public:
	OutputMemoryStream();
	~OutputMemoryStream();

	OutputMemoryStream(const OutputMemoryStream&) = delete;
	OutputMemoryStream& operator=(const OutputMemoryStream&) = delete;

	virtual std::uint8_t* GetBufferPtr() const override { return m_buffer->data(); }
	virtual std::size_t GetBufferSize() const override { return m_head; }

	void Serialize(const std::uint8_t* inData, std::size_t size)	{ Write(inData, size); }

	template<typename T>
	void Serialize(std::vector<T>& vector) { Write(vector); }

	bool IsInput() const { return false; }

	template<typename T>
	void Write(const T& data) requires Primitive<T>;

	template<typename T>
	void Write(const std::vector<T>& vector);

private:
	void Write(const std::uint8_t* data, std::size_t size);

	std::shared_ptr<std::vector<std::uint8_t>> m_buffer;
	std::size_t m_head;
};

template<typename T>
void OutputMemoryStream::Write(const T& data) requires Primitive<T>
{
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

template<typename T>
void OutputMemoryStream::Write(const std::vector<T>& vector)
{
	std::size_t elementCount = vector.size();
	Write(elementCount);
	for (const T& element : vector)
		Write(element);
}

class InputMemoryStream : public MemoryStream<InputMemoryStream>
{
public:
	InputMemoryStream(std::size_t size);
	~InputMemoryStream();

	InputMemoryStream() = delete;
	InputMemoryStream(const InputMemoryStream&) = delete;
	InputMemoryStream& operator=(const InputMemoryStream&) = delete;

	void Serialize(std::uint8_t* outData, std::size_t size) { Read(outData, size); }

	template<typename T>
	void Serialize(std::vector<T>& vector) { Read(vector); }

	bool IsInput() const { return true; }

	template<typename T>
	void Read(T& data) requires Primitive<T> { 
		Read(reinterpret_cast<uint8_t*>(&data), sizeof(T)); }

	template<typename T>
	void Read(std::vector<T>& outVector);

	virtual std::uint8_t* GetBufferPtr() const override { return m_buffer->data(); }
	virtual std::size_t GetBufferSize() const override { return m_head; };

private:
	void Read(std::uint8_t* data, std::size_t size);

	std::shared_ptr<std::vector<std::uint8_t>> m_buffer;
	std::size_t m_head;
};

template<typename T>
void InputMemoryStream::Read(std::vector<T>& outVector)
{
	std::size_t elementCount;
	Read(elementCount);
	outVector.resize(elementCount);
	for (T& element : outVector)
		Read(element);
}