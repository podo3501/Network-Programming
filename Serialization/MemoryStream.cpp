#include "pch.h"
#include "MemoryStream.h"

using namespace std;

OutputMemoryStream::OutputMemoryStream() :
	m_buffer{ nullptr },
	m_head{ 0 }
{
	m_buffer = make_shared<vector<uint8_t>>();
}

OutputMemoryStream::~OutputMemoryStream()
{}

void OutputMemoryStream::Write(const uint8_t* data, size_t size)
{
	if (m_buffer->capacity() < size)
		m_buffer->resize(size * 3);

	auto resultHead = m_head + size;
	copy(data, data + size, m_buffer->begin() + m_head);
	m_head = resultHead;
}

//////////////////////////////////////////////////////////

InputMemoryStream::InputMemoryStream(std::uint32_t size) : m_head{ 0 }
{
	m_buffer = make_shared<vector<uint8_t>>();
	m_buffer->resize(size);
}

InputMemoryStream::~InputMemoryStream() 
{}

void InputMemoryStream::Read(std::uint8_t* data, std::size_t size)
{
	auto start = m_buffer->begin();
	std::size_t resultHead = m_head + size;
	assert(resultHead <= m_buffer->capacity() && "InputMemoryStream::Read - Out of Capacity");

	copy(start + m_head, start + resultHead, data);
	m_head = resultHead;
}