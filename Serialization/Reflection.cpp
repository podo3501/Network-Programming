#include "pch.h"
#include "Reflection.h"

MemberVariable::~MemberVariable() = default;

/////////////////////////////////////////////////////////////////////////

DataType::DataType(std::initializer_list<MemberVariable> mvList) :
	m_memberVariables{ mvList }
{}

DataType::~DataType() = default;