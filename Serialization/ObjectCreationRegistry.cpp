#include "pch.h"
#include "ObjectCreationRegistry.h"
#include "LinkingContext.h"

GameObject* ObjectCreationRegistry::CreateGameObject(std::uint32_t inClassId)
{
	auto find = m_createFuncList.find(inClassId);
	if (find == m_createFuncList.end())
		return nullptr;

	return find->second();
}