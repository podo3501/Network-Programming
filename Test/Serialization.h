#pragma once

#include "../Serialization/MemoryStream.h"
#include "../Serialization/MemoryBitStream.h"

namespace Serialization
{
	class FixtureMemoryStream : public ::testing::Test
	{
	protected:
		void SetUp() override;
		void TearDown() override;

	protected:
//CMSTest m_toMS;
		std::unique_ptr<MemoryStream<OutputMemoryStream>> m_writeStream;
		std::unique_ptr<MemoryStream<InputMemoryStream>> m_readStream;
	};
}