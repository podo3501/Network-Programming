#pragma once

class OutputMemoryBitStream;
class InputMemoryBitStream;

#ifndef CLASS_IDENTIFICATION
#define CLASS_IDENTIFICATION(inCode, inClass) \
enum { kClassId = inCode }; \
virtual std::uint32_t GetClassID() const { return kClassId; } \
static std::unique_ptr<inClass> CreateInstance() { return std::make_unique<inClass>(); }
#endif

class GameObject
{
public:
	CLASS_IDENTIFICATION('GOBJ', GameObject)

	virtual ~GameObject() = default;
	virtual void WriteBit(OutputMemoryBitStream& ombs) {};
	virtual void ReadBit(InputMemoryBitStream& imbs) {};
};

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