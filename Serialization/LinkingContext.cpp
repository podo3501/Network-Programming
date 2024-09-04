#include "pch.h"
#include "LinkingContext.h"

using namespace std;

GameObject::~GameObject() = default;
void GameObject::WriteBit(OutputMemoryBitStream& ombs, LinkingContext* linkingContext)
{}
void GameObject::ReadBit(InputMemoryBitStream& imbs, LinkingContext* linkingContext)
{}

/////////////////////////////////////////////////////////////////

LinkingContext::~LinkingContext() = default;
LinkingContext::LinkingContext() :
	m_nextNetworkID{ 1 }
{}

uint32_t LinkingContext::GetNetworkID(GameObject* gameObject, bool shouldCreateIfNotFound)
{
	auto find = m_gameObjectToNetworkList.find(gameObject);
	if (find != m_gameObjectToNetworkList.end())
		return find->second;
	else if (shouldCreateIfNotFound)
	{
		uint32_t newNetworkID = m_nextNetworkID++;
		AddGameObject(gameObject, newNetworkID);
		return newNetworkID;
	}
	
	return 0;
}

GameObject* LinkingContext::GetGameObject(uint32_t networkID) const
{
	auto find = m_networkListToGameObject.find(networkID);
	if (find != m_networkListToGameObject.end())
		return find->second;
	else
		return nullptr;
}

void LinkingContext::AddGameObject(GameObject* gameObject, uint32_t networkID)
{
	m_networkListToGameObject[networkID] = gameObject;
	m_gameObjectToNetworkList[gameObject] = networkID;
}

void LinkingContext::RemoveGameObject(GameObject* gameObject)
{
	uint32_t networkID = m_gameObjectToNetworkList[gameObject];
	m_gameObjectToNetworkList.erase(gameObject);
	m_networkListToGameObject.erase(networkID);
}