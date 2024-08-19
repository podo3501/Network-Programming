#pragma once

class GameObject;

class LinkingContext
{
public:
	LinkingContext() {}

	std::uint32_t GetNetworkID(GameObject* gameObject)
	{
		auto find = m_gameObjectToNetworkList.find(gameObject);
		if (find != m_gameObjectToNetworkList.end())
			return find->second;

		return 0;
	}

	GameObject* GetGameObject(std::uint32_t networkID) const
	{
		auto find = m_networkListToGameObject.find(networkID);
		if (find != m_networkListToGameObject.end())
			return find->second;
		else
			return nullptr;
	}

	void AddGameObject(GameObject* gameObject, std::uint32_t networkID)
	{
		m_networkListToGameObject[networkID] = gameObject;
		m_gameObjectToNetworkList[gameObject] = networkID;
	}

	void RemoveGameObject(GameObject* gameObject)
	{
		std::uint32_t networkID = m_gameObjectToNetworkList[gameObject];
		m_gameObjectToNetworkList.erase(gameObject);
		m_networkListToGameObject.erase(networkID);
	}

private:
	std::unordered_map<std::uint32_t, GameObject*> m_networkListToGameObject;
	std::unordered_map<const GameObject*, std::uint32_t> m_gameObjectToNetworkList;
};