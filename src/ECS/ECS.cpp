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
    size_t EntityID = NumberOfEntities++;
    
    Entity NewEntity(EntityID);
    
    NewEntity.Manager = this;
    
    EntitiesToBeAdded.insert(NewEntity);
    
    // Make sure the EntityComponentSignatures vector can accomodate the new entity
    if (EntityID >= EntityComponentSignatures.size()) EntityComponentSignatures.resize(EntityID + 1);
    
    Logger::Log("Entity created with ID: " + std::to_string(EntityID));
    
    return NewEntity;
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

void ECSManager::Update()
{
    // Add the entities that are waiting to be created to the active systems
    for (Entity Entity : EntitiesToBeAdded)
    {
        AddEntityToSystems(Entity);
    }
    EntitiesToBeAdded.clear();
    
    // Remove the entities that are waiting to be killed from the active systems
}
