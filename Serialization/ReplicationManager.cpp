#include "pch.h"
#include "ReplicationManager.h"
#include <unordered_set>
#include <algorithm>
#include <ranges>
#include "Type.h"
#include "MemoryBitStream.h"
#include "LinkingContext.h"
#include "ObjectCreationRegistry.h"

enum class ReplicationAction
{
	RA_Create,
	RA_Update,
	RA_Destroy,
	RA_MAX,
};

using enum ReplicationAction;
using enum PacketType;

class ReplicationHeader
{
public:
	ReplicationHeader() :
		m_replicationAction{ RA_MAX }, m_networkID{ 0 }, m_classID{ 0 } {}

	ReplicationHeader(ReplicationAction inRA, std::uint32_t inNetworkID, std::uint32_t inClassID = 0) :
		m_replicationAction{ inRA }, m_networkID{ inNetworkID }, m_classID{ inClassID } {}

	void Write(OutputMemoryBitStream& ombs);
	void Read(InputMemoryBitStream& imbs);

	ReplicationAction m_replicationAction;
	std::uint32_t m_networkID;
	std::uint32_t m_classID;
};

void ReplicationHeader::Write(OutputMemoryBitStream& ombs)
{
	ombs.Write(m_replicationAction, GetRequiredBits(EtoV(RA_MAX)));
	ombs.Write(m_networkID);

	if (m_replicationAction != RA_Destroy)
		ombs.Write(m_classID);
}

void ReplicationHeader::Read(InputMemoryBitStream& imbs)
{
	imbs.Read(m_replicationAction, GetRequiredBits(EtoV(RA_MAX)));
	imbs.Read(m_networkID);

	if (m_replicationAction != RA_Destroy)
		imbs.Read(m_classID);
}

////////////////////////////////////////////////////////////////////////////////

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

	//비트스트림이라 길이가 비트 단위이지만 패킷은 바이트 단위라 최대 7비트가 남을 수 있다.
	while (inStream.GetRemainingBitCount() >= 8)	
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

void ReplicationManager::ReplicateCreate(OutputMemoryBitStream& inStream, GameObject* gameObject)
{
	ReplicationHeader repHeader(RA_Create, 
		m_linkingContext->GetNetworkID(gameObject, true), gameObject->GetClassID());

	repHeader.Write(inStream);
	gameObject->WriteBit(inStream, m_linkingContext.get());
}

void ReplicationManager::ReplicateUpdate(OutputMemoryBitStream& inStream, GameObject* gameObject)
{
	ReplicationHeader repHeader(RA_Update,
		m_linkingContext->GetNetworkID(gameObject, false), gameObject->GetClassID());

	repHeader.Write(inStream);
	gameObject->WriteBit(inStream, m_linkingContext.get());
}

void ReplicationManager::ReplicateDestroy(OutputMemoryBitStream& inStream, GameObject* gameObject)
{
	ReplicationHeader repHeader(RA_Destroy,
		m_linkingContext->GetNetworkID(gameObject, false), gameObject->GetClassID());

	repHeader.Write(inStream);
}

void ReplicationManager::ProcessReplicationAction(InputMemoryBitStream& imbs)
{
	ReplicationHeader repHeader;
	repHeader.Read(imbs);
	GameObject* object{ nullptr };

	switch (repHeader.m_replicationAction)
	{
	case RA_Create:
		object = m_creationRegistry->CreateGameObject(repHeader.m_classID);
		m_linkingContext->AddGameObject(object, repHeader.m_networkID);
		object->ReadBit(imbs, m_linkingContext.get());
		break;
	case RA_Update:
		object = m_linkingContext->GetGameObject(repHeader.m_networkID);
		if (object == nullptr)
		{
			//생성동작을 아직 받지 못했다면 데이터를 읽은다음 폐기함(지금 읽어도 필요없기 때문에)
			object = m_creationRegistry->CreateGameObject(repHeader.m_classID);
			object->ReadBit(imbs, m_linkingContext.get());
			delete object;
			break;
		}
		object->ReadBit(imbs, m_linkingContext.get());
		break;
	case RA_Destroy:
		object = m_linkingContext->GetGameObject(repHeader.m_networkID);
		m_linkingContext->RemoveGameObject(object);
		break;
	default:
		break;
	}
}