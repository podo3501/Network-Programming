#include "pch.h"
#include "ObjectCreationRegistry.h"
#include "LinkingContext.h"

std::unique_ptr<GameObject> ObjectCreationRegistry::CreateGameObject(std::uint32_t inClassId)
{
	auto find = m_createFuncList.find(inClassId);
	if (find == m_createFuncList.end())
		return nullptr;

	return std::move(find->second());
}