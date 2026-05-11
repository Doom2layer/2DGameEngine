#include "ECS.h"

#include "../Logger/Logger.h"

size_t IComponent::NextID = 0;

Entity::Entity(size_t InID) : ID(InID)
{
}

size_t Entity::GetID() const
{
    return ID;   
}

void Entity::Kill()
{
    Manager->KillEntity(*this);
}

void System::AddEntityToSystem(Entity InEntity)
{
    Entities.push_back(InEntity);
}

void System::RemoveEntityFromSystem(Entity InEntity)
{
    Entities.erase(std::remove_if(Entities.begin(), Entities.end(), [&InEntity](Entity OtherEntity)
    {
        return OtherEntity == InEntity;
    }), Entities.end());
}

std::vector<Entity> System::GetSystemEntities() const
{
    return Entities;
}

const Signature& System::GetComponentSignature() const
{
    return ComponentSignature;
}

Entity ECSManager::CreateEntity()
{
    size_t EntityID;
    
    if (FreeEntityIDs.empty())
    {
        // if there are no free ids waiting to be reused
        EntityID = NumberOfEntities++;
        if (EntityID >= EntityComponentSignatures.size())
        {
            EntityComponentSignatures.resize(EntityID + 1);
        }
    }
    else
    {
        // Reuse an id from the list of previously removed entities
        EntityID = FreeEntityIDs.front();
        FreeEntityIDs.pop_front();
    }
    
    Entity NewEntity(EntityID);
    
    NewEntity.Manager = this;
    
    EntitiesToBeAdded.insert(NewEntity);
    
    // Make sure the EntityComponentSignatures vector can accomodate the new entity
    if (EntityID >= EntityComponentSignatures.size()) EntityComponentSignatures.resize(EntityID + 1);
    
    Logger::Log("Entity created with ID: " + std::to_string(EntityID));
    
    return NewEntity;
}

void ECSManager::KillEntity(Entity InEntity)
{
    EntitiesToBeRemoved.insert(InEntity);
}

void ECSManager::AddEntityToSystems(Entity InEntity)
{
    const size_t EntityID = InEntity.GetID();
    
    // Match EntityComponentSignature with SystemComponentSignature
    const Signature& EntityComponentSignature = EntityComponentSignatures[EntityID];
    for (std::pair<const std::type_index, std::shared_ptr<System>>& System : Systems)
    {
        const Signature& SystemComponentSignature = System.second->GetComponentSignature();

        if ((EntityComponentSignature & SystemComponentSignature) == SystemComponentSignature)
        {
            System.second->AddEntityToSystem(InEntity);
        }
    }
}

void ECSManager::RemoveEntityFromSystems(Entity InEntity)
{
    for (std::pair<const std::type_index, std::shared_ptr<System>>& System : Systems)
    {
        System.second->RemoveEntityFromSystem(InEntity);
    }
}

void ECSManager::Update()
{
    // Process the entities that are waiting to be created to the active systems
    for (Entity Entity : EntitiesToBeAdded)
    {
        AddEntityToSystems(Entity);
    }
    EntitiesToBeAdded.clear();
    
    // Process the entities that are waiting to be killed from the active systems
    
    for (Entity Entity : EntitiesToBeRemoved)
    {
        RemoveEntityFromSystems(Entity);
        
        EntityComponentSignatures[Entity.GetID()].reset();
        
        // Make EntityID available to be reused
        FreeEntityIDs.push_back(Entity.GetID());
        
    }
    EntitiesToBeRemoved.clear();
}
