#pragma once
#include <assert.h>
#include <unordered_map>
#include <functional>

class GameObject;

class ObjectCreationRegistry
{
	using CreateFunc = std::function<GameObject*()>;

public:
	template<class T>
	void RegisterCreationFunction()
	{
		assert(m_createFuncList.find(T::kClassId) == m_createFuncList.end());
		m_createFuncList[T::kClassId] = T::CreateInstance;
	}

	GameObject* CreateGameObject(std::uint32_t inClassId);

private:
	std::unordered_map<std::uint32_t, CreateFunc> m_createFuncList;
};