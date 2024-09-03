#pragma once
#include <assert.h>
#include <unordered_map>
#include <memory>
#include <functional>

class GameObject;

class ObjectCreationRegistry
{
	using CreateFunc = std::function<std::unique_ptr<GameObject>()>;

public:
	template<class T>
	void RegisterCreationFunction()
	{
		assert(m_createFuncList.find(T::kClassId) == m_createFuncList.end());
		m_createFuncList[T::kClassId] = T::CreateInstance;
	}

	std::unique_ptr<GameObject> CreateGameObject(std::uint32_t inClassId);

private:
	std::unordered_map<std::uint32_t, CreateFunc> m_createFuncList;
};