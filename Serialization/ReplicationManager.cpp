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
{
	m_creationRegistry->RegisterCreationFunction<GameObject>();
}

ObjectCreationRegistry* ReplicationManager::GetRegistry()
{
	return m_creationRegistry.get();
}

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
	gameObject->WriteBit(inStream, m_linkingContext.get());
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
		//생성시에는 new로 하지만 각 클래스에 배치 될때 스마트 포인터로 바뀌기 때문에 메모리 걱정을 할 필요가 없다.
		object = m_creationRegistry->CreateGameObject(classID);
		m_linkingContext->AddGameObject(object, networkID);
	}

	object->ReadBit(inStream, m_linkingContext.get());

	return object;
}