#pragma once
#include "../Logger/Logger.h"
#include <bitset>
#include <cassert>
#include <deque>
#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <set>
#include <stdexcept>

/*************************************************
 *  ECS (Entity-Component-System) Architecture
 *  ───────────────────────────────────────────────
 *  - Signature: A bitset to track which components an entity has. Each bit represents a specific component type (e.g., bit 0 for TransformComponent, bit 1 for VelocityComponent).
 *************************************************/

constexpr unsigned int MAX_COMPONENTS = 32; // Maximum number of different component types

typedef std::bitset<MAX_COMPONENTS> Signature; // Bitset to track which components an entity has

/*************************************************
 *  ECS (Entity-Component-System) Architecture
 *  ───────────────────────────────────────────────
 *  - IComponent: A base class for all components. It can be used to store common functionality or data for all components (e.g., a unique ID for each component type).
 *  - Component: A template class that inherits from BaseComponent. It is used to define specific component types (e.g., TransformComponent, VelocityComponent) and assigns a unique ID to each component type using a static member variable.
 *************************************************/

struct IComponent
{
protected:
    static size_t NextID;
    
};

// Used to assign a unique id to a component type
template<typename T>
class Component : public IComponent
{
public:
    [[nodiscard]] static size_t GetID() 
    { 
        static size_t ID = NextID++; 
        return ID; 
    }
};

/*************************************************
 *  ECS (Entity-Component-System) Architecture
 *  ───────────────────────────────────────────────
 *  - Entity: A unique identifier (ID) representing a game object.
 *************************************************/

class ECSManager;

class Entity
{
public:
    Entity(size_t InID);
    [[nodiscard]] size_t GetID() const;
    void Kill();
    
    //Manage Entity Tags and Groups
    void Tag(const std::string& Tag);
    bool HasTag(const std::string& Tag) const;
    void Group(const std::string& Group);
    bool BelongsToGroup(const std::string& Group) const;
    
    template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...InArgs);
    template <typename TComponent> void RemoveComponent();
    template <typename TComponent> bool HasComponent() const;
    template <typename TComponent> TComponent& GetComponent() const;
    
    Entity& operator=(const Entity& OtherEntity) = default;
    bool operator==(const Entity& OtherEntity) const { return ID == OtherEntity.ID; }
    bool operator!=(const Entity& OtherEntity) const { return !(*this == OtherEntity); }
    bool operator<(const Entity& OtherEntity) const { return ID < OtherEntity.ID; }
    bool operator>(const Entity& OtherEntity) const { return ID > OtherEntity.ID; }
    
    ECSManager* Manager; // Pointer to the ECSManager to allow entities to add/remove components and interact with systems
    
private:
    size_t ID;
};

/*************************************************
 *  ECS (Entity-Component-System) Architecture
 *  ───────────────────────────────────────────────
 *  - System: Contains logic to process entities with specific components (e.g., MovementSystem processes entities with TransformComponent and VelocityComponent).
 *************************************************/

class System
{
public:
    System() = default;
    virtual ~System() = default;
    
    virtual void AddEntityToSystem(Entity InEntity);
    void RemoveEntityFromSystem(Entity InEntity);
    std::vector<Entity> GetSystemEntities() const;
    const Signature& GetComponentSignature() const;
    
    // Defines the component type that entities must have to be considered by the system
    template<typename TComponent> void RequireComponent();
    
private:
    Signature ComponentSignature;
    std::vector<Entity> Entities;
    
};

/*************************************************
 *  ECS (Entity-Component-System) Architecture
 *  ───────────────────────────────────────────────
 *  - IPool: An interface for component pools. It can be used to store common functionality or data for all component pools (e.g., a unique ID for each pool type).
 *  - Pool: A template class that inherits from IPool. It is used to define specific component pools (e.g., TransformComponentPool, VelocityComponentPool) and provides methods to manage component data for entities (e.g., Add, Get, Set).
 *************************************************/

class IPool
{
public:
    virtual ~IPool(){}
    virtual void RemoveEntityFromPool(size_t EntityID) = 0;
};

template<typename T>
class Pool : public IPool
{
public:
    explicit Pool(size_t Capacity = 100) : Data(Capacity), Size(0) {}
    virtual ~Pool() = default;
    virtual void RemoveEntityFromPool(size_t EntityID) override
    {
        Remove(EntityID);
    }
    
    [[nodiscard]] bool   IsEmpty() const { return Size == 0; }
    [[nodiscard]] size_t GetSize() const { return Size; }
    
    void Add(const T& Object) {Data.push_back(Object);}
    void Resize(size_t N)     { Data.resize(N); }
    void Clear()              { Data.clear(); EntityToIndex.clear(); IndexToEntity.clear(); Size = 0; }
    
    void Set(size_t EntityID, const T& Object)
    {
        const auto IT = EntityToIndex.find(EntityID);
        if (IT != EntityToIndex.end())
        {
            Data[IT->second] = Object;
            return;
        }

        size_t Index = Size;
        EntityToIndex.emplace(EntityID, Index);
        IndexToEntity.emplace(Index, EntityID);

        if (Index >= Data.size())
        {
            Data.resize(Size * 2 + 1);
        }

        Data[Index] = Object;
        Size++;
    }
    
    void Remove(size_t EntityID)
    {
        const auto IT = EntityToIndex.find(EntityID);
        if (IT == EntityToIndex.end()) return;

        size_t IndexOfRemoved = IT->second;
        size_t IndexOfLast    = Size - 1;

        if (IndexOfRemoved != IndexOfLast)
        {
            size_t LastEntityID        = IndexToEntity[IndexOfLast];
            Data[IndexOfRemoved]       = Data[IndexOfLast];
            EntityToIndex[LastEntityID] = IndexOfRemoved;
            IndexToEntity[IndexOfRemoved] = LastEntityID;
        }

        EntityToIndex.erase(IT);
        IndexToEntity.erase(IndexOfLast);
        Size--;
    }
    
    [[nodiscard]] T& Get(size_t EntityID)
    {
        assert(EntityToIndex.find(EntityID) != EntityToIndex.end() && "Entity not found in pool");
        return Data[EntityToIndex[EntityID]];
    }

    [[nodiscard]] const T& Get(size_t EntityID) const
    {
        assert(EntityToIndex.find(EntityID) != EntityToIndex.end() && "Entity not found in pool");
        return Data[EntityToIndex.at(EntityID)];
    }

    T&       operator[](size_t Index)       { return Data[Index]; }
    const T& operator[](size_t Index) const { return Data[Index]; }
    
private:
    // We keep track of vector of objects and current number of elements
    std::vector<T>                     Data;
    size_t                             Size;
    // Helper maps to keep track of entity ids per index, so the vector is always packed
    std::unordered_map<size_t, size_t> EntityToIndex;
    std::unordered_map<size_t, size_t> IndexToEntity;
    
};

/*************************************************
 *  ECS (Entity-Component-System) Architecture
 *  ───────────────────────────────────────────────
 *  - ECSManager: Manages entities, components, and systems. Responsible for creating entities, adding/removing components, and updating systems.
 *************************************************/

class ECSManager
{
public:
    ECSManager() = default;
    
    // The Manager Update() finally processes the entities that are waiting to be added/killed
    void Update();
    
    //Entity management
    Entity CreateEntity();
    void KillEntity(Entity InEntity);
    
    //Component management
    template <typename TComponent, typename ...TArgs> void AddComponent(Entity InEntity, TArgs&& ...InArgs);
    template <typename TComponent> void RemoveComponent(Entity InEntity);
    template <typename TComponent> bool HasComponent(Entity InEntity) const;
    template <typename TComponent> TComponent& GetComponent(Entity InEntity) const;

    //System Management
    template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...InArgs);
    template <typename TSystem> void RemoveSystem();
    template <typename TSystem> bool HasSystem() const;
    template <typename TSystem> TSystem& GetSystem() const;
    
    // Add and remove entities from systems
    void AddEntityToSystems(Entity InEntity);
    void RemoveEntityFromSystems(Entity InEntity);
    
    // Tag Management
    void TagEntity(Entity InEntity, const std::string& Tag);
    bool EntityHasTag(Entity InEntity, const std::string& Tag) const;
    Entity GetEntityByTag(const std::string& Tag) const;
    void RemoveEntityTag(Entity InEntity);
    
    //Group Management
    void GroupEntity(Entity InEntity, const std::string& Group);
    bool EntityBelongsToGroup(Entity InEntity, const std::string& Group) const;
    std::vector<Entity> GetEntitiesByGroup(const std::string& Group) const;
    void RemoveEntityGroup(Entity InEntity);
    
    
private:
    // Keep track of how many entites were added to the scene
    int NumberOfEntities{0};
    
    // vector of component pools, each pool contains all the data for a certain component type
    // vector index = component type id
    // pool index = entity id
    std::vector<std::shared_ptr<IPool>> ComponentPools;
    
    // Vector of component signatures per entity, saying which component is turned on for a given entity
    // vector index = entity id
    std::vector<Signature> EntityComponentSignatures;
    
        
    std::unordered_map<std::type_index, std::shared_ptr<System>> Systems;
    
    //Set of entities that are flagged to be added or removed in the next Manager update()
    std::set<Entity> EntitiesToBeAdded;
    std::set<Entity> EntitiesToBeRemoved;
    
    //Entity Tags (one tag name per entity)
    std::unordered_map<std::string, Entity> EntityPerTag;
    std::unordered_map<int, std::string> TagPerEntity;
    
    //Entity Groups (a set of entities per group name)
    std::unordered_map<std::string, std::set<Entity>> EntitiesPerGroup;
    std::unordered_map<int, std::string> GroupPerEntity;
    
    //List of free entity IDs that were previously removed
    std::deque<size_t> FreeEntityIDs;
    
};

template <typename TComponent, typename ... TArgs>
void Entity::AddComponent(TArgs&&... InArgs)
{
    static_assert(std::is_base_of<IComponent, TComponent>::value, "Entity::AddComponent TComponent must derive from IComponent.");
    Manager->AddComponent<TComponent>(*this, std::forward<TArgs>(InArgs)...);
}

template <typename TComponent>
void Entity::RemoveComponent()
{
    static_assert(std::is_base_of<IComponent, TComponent>::value, "Entity::RemoveComponent TComponent must derive from IComponent.");
    Manager->RemoveComponent<TComponent>(*this);   
}

template <typename TComponent>
bool Entity::HasComponent() const
{
    static_assert(std::is_base_of<IComponent, TComponent>::value, "Entity::HasComponent TComponent must derive from IComponent.");
    return Manager->HasComponent<TComponent>(*this);  
}

template <typename TComponent>
TComponent& Entity::GetComponent() const
{
    static_assert(std::is_base_of<IComponent, TComponent>::value, "Entity::GetComponent TComponent must derive from IComponent.");
    return Manager->GetComponent<TComponent>(*this); 
}

template <typename TComponent>
void System::RequireComponent()
{
    static_assert(std::is_base_of<IComponent, TComponent>::value, "ECSManager::RequireComponent TComponent must derive from IComponent.");
    
    const auto ComponentID = Component<TComponent>::GetID();
    ComponentSignature.set(ComponentID);
}

template <typename TComponent, typename ... TArgs>
void ECSManager::AddComponent(Entity InEntity, TArgs&&... InArgs)
{
    static_assert(std::is_base_of<IComponent, TComponent>::value, "ECSManager::AddComponent TComponent must derive from IComponent.");
    // catches wrong arguments with a clear error message
    static_assert(std::is_constructible<TComponent, TArgs...>::value, 
        "ECSManager::AddComponent arguments do not match TComponent constructor.");

    const size_t ComponentID = Component<TComponent>::GetID();
    const size_t EntityID = InEntity.GetID();
    
    // entity already has this component
    if (HasComponent<TComponent>(InEntity))
    {
        Logger::Warning("ECSManager::AddComponent entity ID = " + std::to_string(EntityID) + " already has component ID = " + std::to_string(ComponentID));
        return;
    }
    
    if (ComponentID >= ComponentPools.size())
    {
        ComponentPools.resize(ComponentID + 1, nullptr);
    }
    
    if (!ComponentPools[ComponentID])
    {
        ComponentPools[ComponentID] = std::make_shared<Pool<TComponent>>();
    }
    
    std::shared_ptr<Pool<TComponent>> ComponentPool = std::static_pointer_cast<Pool<TComponent>>(ComponentPools[ComponentID]);
    
    TComponent NewComponent(std::forward<TArgs>(InArgs)...);

    ComponentPool->Set(EntityID, NewComponent);
    
    EntityComponentSignatures[EntityID].set(ComponentID);
    
    Logger::Log("Component ID = " + std::to_string(ComponentID) + " added to Entity ID = " + std::to_string(EntityID));
    
}

template <typename TComponent>
void ECSManager::RemoveComponent(Entity InEntity)
{
    static_assert(std::is_base_of<IComponent, TComponent>::value, "ECSManager::RemoveComponent TComponent must derive from IComponent.");
    
    const size_t ComponentID = Component<TComponent>::GetID();
    const size_t EntityID = InEntity.GetID();

    // entity doesn't have this component
    if (!HasComponent<TComponent>(InEntity))
    {
        Logger::Warning("ECSManager::RemoveComponent entity ID = " + std::to_string(EntityID) + " does not have component ID = " + std::to_string(ComponentID));
        return;
    }

    // Remove the component from the component list for that entity
    std::shared_ptr<Pool<TComponent>> ComponentPool = std::static_pointer_cast<Pool<TComponent>>(ComponentPools[ComponentID]);
    ComponentPool->Remove(EntityID);
    
    // Set this component signature for that entity to false
    EntityComponentSignatures[EntityID].set(ComponentID, false);
    Logger::Log("Component ID = " + std::to_string(ComponentID) + " removed from Entity ID = " + std::to_string(EntityID));   
}

template <typename TComponent>
bool ECSManager::HasComponent(Entity InEntity) const
{
    static_assert(std::is_base_of<IComponent, TComponent>::value, "ECSManager::HasComponent TComponent must derive from IComponent.");
    
    const size_t ComponentID = Component<TComponent>::GetID();
    const size_t EntityID = InEntity.GetID();

    // entity ID out of bounds
    if (EntityID >= EntityComponentSignatures.size())
    {
        Logger::Warning("ECSManager::HasComponent entity ID = " + std::to_string(EntityID) + " is out of bounds.");
        return false;
    }

    // component ID out of bounds
    if (ComponentID >= MAX_COMPONENTS)
    {
        Logger::Warning("ECSManager::HasComponent component ID = " + std::to_string(ComponentID) + " exceeds MAX_COMPONENTS.");
        return false;
    }

    return EntityComponentSignatures[EntityID].test(ComponentID);
}

template <typename TComponent>
TComponent& ECSManager::GetComponent(Entity InEntity) const
{
    static_assert(std::is_base_of<IComponent, TComponent>::value, "ECSManager::GetComponent TComponent must derive from IComponent.");
    
    const size_t ComponentID = Component<TComponent>::GetID();
    const size_t EntityID = InEntity.GetID();

    // does the pool exist for this component type?
    if (ComponentID >= ComponentPools.size() || !ComponentPools[ComponentID])
    {
        Logger::Error("ECSManager::GetComponent pool does not exist for component ID = " + std::to_string(ComponentID));
        throw std::runtime_error("Component pool not found.");
    }

    // does this entity actually have this component?
    if (!EntityComponentSignatures[EntityID].test(ComponentID))
    {
        Logger::Error("ECSManager::GetComponent entity ID = " + std::to_string(EntityID) + " does not have component ID = " + std::to_string(ComponentID));
        throw std::runtime_error("Entity does not have component.");
    }

    /*
    Logger::Log("Component ID = " + std::to_string(ComponentID) + " retrieved from Entity ID = " + std::to_string(EntityID));
    */

    return std::static_pointer_cast<Pool<TComponent>>(ComponentPools[ComponentID])->Get(EntityID);
}

template <typename TSystem, typename ... TArgs>
void ECSManager::AddSystem(TArgs&&... InArgs)
{
    static_assert(std::is_base_of<System, TSystem>::value, "ECSManager::AddSystem TSystem must derive from System.");
    std::shared_ptr<TSystem> NewSystem =  std::make_shared<TSystem>(std::forward<TArgs>(InArgs)...);
    Systems.insert(std::make_pair(std::type_index(typeid(TSystem)), NewSystem));
}

template <typename TSystem>
void ECSManager::RemoveSystem()
{
    static_assert(std::is_base_of<System, TSystem>::value, "ECSManager::RemoveSystem TSystem must derive from System.");
    auto It = Systems.find(std::type_index(typeid(TSystem)));
    if (It == Systems.end())
    {
        Logger::Warning("ECSManager::RemoveSystem system not found.");
        return;
    }
    Systems.erase(It);
}

template <typename TSystem>
bool ECSManager::HasSystem() const
{
    static_assert(std::is_base_of<System, TSystem>::value, "ECSManager::HasSystem TSystem must derive from System.");
    return Systems.find(std::type_index(typeid(TSystem))) != Systems.end();
}

template <typename TSystem>
TSystem& ECSManager::GetSystem() const
{
    static_assert(std::is_base_of<System, TSystem>::value, "ECSManager::GetSystem TSystem must derive from System.");
    auto It = Systems.find(std::type_index(typeid(TSystem)));
    if (It == Systems.end())
    {
        Logger::Error("ECSManager::GetSystem system not found.");
    }
    return *(std::static_pointer_cast<TSystem>(It->second));
}
