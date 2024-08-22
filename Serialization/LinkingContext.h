#pragma once

class GameObject;

class LinkingContext
{
public:
	LinkingContext();
	~LinkingContext();

	LinkingContext(const LinkingContext&) = delete;
	LinkingContext operator=(const LinkingContext&) = delete;

	std::uint32_t GetNetworkID(GameObject* gameObject, bool shouldCreateIfNotFound);
	GameObject* GetGameObject(std::uint32_t networkID) const;

	void AddGameObject(GameObject* gameObject, std::uint32_t networkID);
	void RemoveGameObject(GameObject* gameObject);

private:
	std::unordered_map<std::uint32_t, GameObject*> m_networkListToGameObject;
	std::unordered_map<const GameObject*, std::uint32_t> m_gameObjectToNetworkList;
	std::uint32_t m_nextNetworkID;
};