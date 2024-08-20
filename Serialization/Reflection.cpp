#include "pch.h"
#include "Reflection.h"

//MemberVariable::MemberVariable(const std::string& name, MemberType mType, std::size_t offset, const T& variable) :
//	m_name{ name },
//	m_type{ mType },
//	m_offset{ offset }
//{
//	
//}

MemberVariable::~MemberVariable() = default;

/////////////////////////////////////////////////////////////////////////

DataType::DataType(std::initializer_list<MemberVariable> mvList) :
	m_memberVariables{ mvList }
{}

DataType::~DataType() = default;