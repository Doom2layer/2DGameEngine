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

void Entity::Tag(const std::string& Tag)
{
    Manager->TagEntity(*this, Tag);
}

bool Entity::HasTag(const std::string& Tag) const
{
    return Manager->EntityHasTag(*this, Tag);
}

void Entity::Group(const std::string& Group)
{
    Manager->GroupEntity(*this, Group);   
}

bool Entity::BelongsToGroup(const std::string& Group) const
{
    return Manager->EntityBelongsToGroup(*this, Group);
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

void ECSManager::TagEntity(Entity InEntity, const std::string& Tag)
{
    EntityPerTag.emplace(Tag, InEntity);
    TagPerEntity.emplace(InEntity.GetID(), Tag);
}

bool ECSManager::EntityHasTag(Entity InEntity, const std::string& Tag) const
{
    const auto IT = TagPerEntity.find(InEntity.GetID());
    if (IT == TagPerEntity.end())
    {
        return false;
    }
    return IT->second == Tag;
}

Entity ECSManager::GetEntityByTag(const std::string& Tag) const
{
    const auto IT = EntityPerTag.find(Tag);
    if (IT == EntityPerTag.end())
    {
        Logger::Warning("ECSManager::GetEntityByTag tag not found: " + Tag);
        return Entity(-1); // Return an invalid entity if tag not found
    }
    return IT->second;
}

void ECSManager::RemoveEntityTag(Entity InEntity)
{
    const auto IT = TagPerEntity.find(InEntity.GetID());
    if (IT != TagPerEntity.end())
    {
        EntityPerTag.erase(IT->second);
        TagPerEntity.erase(IT);
    }
}

void ECSManager::GroupEntity(Entity InEntity, const std::string& Group)
{
    EntitiesPerGroup.try_emplace(Group).first->second.emplace(InEntity);
    GroupPerEntity.emplace(InEntity.GetID(), Group);
}

bool ECSManager::EntityBelongsToGroup(Entity InEntity, const std::string& Group) const
{
    const auto IT = EntitiesPerGroup.find(Group);
    if (IT == EntitiesPerGroup.end())
    {
        return false;
    }
    return IT->second.find(InEntity) != IT->second.end();
}

std::vector<Entity> ECSManager::GetEntitiesByGroup(const std::string& Group) const
{
    const auto IT = EntitiesPerGroup.find(Group);
    if (IT == EntitiesPerGroup.end())
    {
        return {};
    }
    return std::vector<Entity>(IT->second.begin(), IT->second.end());
}

void ECSManager::RemoveEntityGroup(Entity InEntity)
{
    const auto GroupIT = GroupPerEntity.find(InEntity.GetID());
    if (GroupIT == GroupPerEntity.end())
    {
        return;
    }

    const auto EntitiesIT = EntitiesPerGroup.find(GroupIT->second);
    if (EntitiesIT != EntitiesPerGroup.end())
    {
        EntitiesIT->second.erase(InEntity);
    }

    GroupPerEntity.erase(GroupIT);
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
        
        //Remove any traces of that entity from the tag or group maps
        RemoveEntityTag(Entity);
        RemoveEntityGroup(Entity);       
        
    }
    EntitiesToBeRemoved.clear();
}
