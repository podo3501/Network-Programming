#pragma once

enum class MemberType
{
	MT_Int,
	MT_String,
	MT_Float,
	MT_VectorDouble,
};

class MemberVariable
{
public:
	MemberVariable(const std::string& name, MemberType mType, std::uint32_t offset) :
		m_name{ name },
		m_type{ mType },
		m_offset{ offset }
	{}
	~MemberVariable();

	MemberVariable() = delete;
	MemberVariable& operator=(const MemberVariable&) = delete;

	MemberType GetType() const { return m_type; }
	std::uint32_t GetOffset() const { return m_offset; }
	
private:
	std::string m_name;
	MemberType m_type;
	std::uint32_t m_offset;
};

class DataType
{
public:
	DataType(std::initializer_list<MemberVariable> mvList);
	~DataType();

	DataType() = delete;
	DataType(const DataType&) = delete;
	DataType& operator=(const DataType&) = delete;

	const std::vector<MemberVariable>& GetMemberVariables() const { return m_memberVariables; }

private:
	std::vector<MemberVariable> m_memberVariables;
};
