#pragma once
#include <vector>
#include <memory>
#include <unordered_set>

class OutputMemoryBitStream;
class InputMemoryBitStream;
class GameObject;
class LinkingContext;
class ObjectCreationRegistry;

class ReplicationManager
{
public:
	ReplicationManager();
	ReplicationManager(const ReplicationManager&) = delete;
	ReplicationManager& operator=(const ReplicationManager&) = delete;

	void ReplicateWorldState(OutputMemoryBitStream& inStream, const std::vector<GameObject*>& allObjects);
	void ReceiveWorld(InputMemoryBitStream& inStream);

	ObjectCreationRegistry* GetRegistry();

private:
	void ReplicateIntoStream(OutputMemoryBitStream& inStream, GameObject* gameObject);
	GameObject* ReceiveReplicatedObject(InputMemoryBitStream& inStream);

	std::unique_ptr<LinkingContext> m_linkingContext;
	std::unique_ptr<ObjectCreationRegistry> m_creationRegistry;
	std::unordered_set<GameObject*> m_replicatedObjects;
};
