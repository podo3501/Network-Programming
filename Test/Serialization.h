#pragma once

namespace Serialization
{
	class FixtureMemoryStream : public ::testing::Test
	{
	protected:
		void SetUp() override;
		void TearDown() override;

	private:
//CMSTest m_toMS;
	};
}