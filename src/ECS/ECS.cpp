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
    if (TagPerEntity.find(InEntity.GetID()) == TagPerEntity.end())
    {
        return false;
    }
    return EntityPerTag.find(Tag)->second == InEntity;
}

Entity ECSManager::GetEntityByTag(const std::string& Tag) const
{
    return EntityPerTag.at(Tag);
}

void ECSManager::RemoveEntityTag(Entity InEntity)
{
    std::unordered_map<int, std::string>::iterator TaggedEntity = TagPerEntity.find(InEntity.GetID());
    if (TaggedEntity != TagPerEntity.end())
    {
        std::string Tag = TaggedEntity->second;
        EntityPerTag.erase(Tag);
        TagPerEntity.erase(TaggedEntity);
    }
}

void ECSManager::GroupEntity(Entity InEntity, const std::string& Group)
{
    EntitiesPerGroup.emplace(Group, std::set<Entity>{});
    EntitiesPerGroup[Group].emplace(InEntity);
    GroupPerEntity.emplace(InEntity.GetID(), Group);
}

bool ECSManager::EntityBelongsToGroup(Entity InEntity, const std::string& Group)
{
    std::set<Entity> GroupEntities = EntitiesPerGroup.at(Group);
    return GroupEntities.find(InEntity.GetID()) != GroupEntities.end();   
}

std::vector<Entity> ECSManager::GetEntitiesByGroup(const std::string& Group) const
{
    const std::set<Entity>& SetOfEntities = EntitiesPerGroup.at(Group);
    return std::vector<Entity>(SetOfEntities.begin(), SetOfEntities.end());  
}

void ECSManager::RemoveEntityGroup(Entity InEntity)
{
    std::unordered_map<int, std::string>::iterator GroupedEntities = GroupPerEntity.find(InEntity.GetID());
    if (GroupedEntities != GroupPerEntity.end())
    {
        std::unordered_map<std::string, std::set<Entity>>::iterator Group = EntitiesPerGroup.find(GroupedEntities->second);
        
        if (Group != EntitiesPerGroup.end())
        {
            std::set<Entity>::iterator EntitiesInGroup = Group->second.find(InEntity);
            if (EntitiesInGroup != Group->second.end())
            {
                Group->second.erase(EntitiesInGroup);
            }
        }
        GroupPerEntity.erase(GroupedEntities);
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
        
        //Remove any traces of that entity from the tag or group maps
        RemoveEntityTag(Entity);
        RemoveEntityGroup(Entity);       
        
    }
    EntitiesToBeRemoved.clear();
}
