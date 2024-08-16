#include "pch.h"
#include "MemoryStream.h"

using namespace std;

OutputMemoryStream::OutputMemoryStream() :
	m_buffer{ nullptr }
{
	m_buffer = make_shared<vector<uint8_t>>();
	m_buffer->resize(32);
}

OutputMemoryStream::~OutputMemoryStream()
{}

void OutputMemoryStream::Write(const void* inData, size_t inSize)
{
	copy(static_cast<const uint8_t*>(inData), static_cast<const uint8_t*>(inData) + inSize, m_buffer->end());
}

