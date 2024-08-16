#pragma once

class OutputMemoryStream
{
public:
	OutputMemoryStream();
	~OutputMemoryStream();

	OutputMemoryStream(const OutputMemoryStream&) = delete;
	OutputMemoryStream& operator=(const OutputMemoryStream&) = delete;

	std::shared_ptr<std::vector<std::uint8_t>> GetBufferPtr() const { return m_buffer; }
	std::uint32_t GetLength() const { return static_cast<std::uint32_t>(m_buffer->size()); }

	template<typename T>
	void Write(const T& inData);

private:
	void Write(const void* inData, std::size_t inSize);

	std::shared_ptr<std::vector<std::uint8_t>> m_buffer;
};

template<typename T>
void OutputMemoryStream::Write(const T& inData)
{
	static_assert(
		std::is_arithmetic<T>::value ||
		std::is_enum<T>::value,
		"Generic Write only supports primitive data types");

	Write(&inData, sizeof(T));
}