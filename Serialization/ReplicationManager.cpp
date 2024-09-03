#include "pch.h"
#include "ReplicationManager.h"
#include <unordered_set>
#include <algorithm>
#include <ranges>
#include "Type.h"
#include "MemoryBitStream.h"
#include "LinkingContext.h"
#include "ObjectCreationRegistry.h"

using enum PacketType;

ReplicationManager::ReplicationManager() :
	m_linkingContext{ std::make_unique<LinkingContext>() },
	m_creationRegistry{ std::make_unique<ObjectCreationRegistry>() }
{}

void ReplicationManager::ReplicateWorldState(OutputMemoryBitStream& inStream, const std::vector<GameObject*>& allObjects)
{
	inStream.Write(PT_ReplicationData, GetRequiredBits(EtoV(PT_Max)));
	for (GameObject* object : allObjects)
	{
		ReplicateIntoStream(inStream, object);
	}
}

void ReplicationManager::ReplicateIntoStream(OutputMemoryBitStream& inStream, GameObject* gameObject)
{
	inStream.Write(m_linkingContext->GetNetworkID(gameObject, true));
	inStream.Write(gameObject->GetClassID());
	gameObject->WriteBit(inStream);
}

void ReplicationManager::ReceiveWorld(InputMemoryBitStream& inStream)
{
	std::unordered_set<GameObject*> receivedObjects;

	while (inStream.GetRemainingBitCount() > 0)	//정확히 비트 숫자만큼은 아니니 나중에 테스트할때 확인해보자
	{
		GameObject* receivedGameObject = ReceiveReplicatedObject(inStream);
		receivedObjects.insert(receivedGameObject);
	}

	auto removeGameObjects = m_replicatedObjects | std::views::filter([&receivedObjects](auto object) {
		return receivedObjects.find(object) == receivedObjects.end();
		});

	std::ranges::for_each(removeGameObjects, [this](auto& object) {
		m_linkingContext->RemoveGameObject(object);
		});

	m_replicatedObjects = receivedObjects;
}

GameObject* ReplicationManager::ReceiveReplicatedObject(InputMemoryBitStream& inStream)
{
	std::uint32_t networkID{ 0 };
	std::uint32_t classID{ 0 };
	inStream.Read(networkID);
	inStream.Read(classID);

	GameObject* object = m_linkingContext->GetGameObject(networkID);
	if (object == nullptr)
	{
		//누가 Object관리역할을 맡을지 생각하자. 아마 linkingContext가 될듯?
		object = m_creationRegistry->CreateGameObject(classID);	
		m_linkingContext->AddGameObject(object, networkID);
	}
}