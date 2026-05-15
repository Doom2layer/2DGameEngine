#include "ECS.h"

#include "../Logger/Logger.h"

size_t IComponent::NextID = 0;

Entity::Entity(size_t InID) : ID(InID), Manager(nullptr)
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
  RemoveEntityTag(InEntity);

  const auto ExistingTag = EntityPerTag.find(Tag);
  if (ExistingTag != EntityPerTag.end())
  {
    TagPerEntity.erase(static_cast<int>(ExistingTag->second.GetID()));
    EntityPerTag.erase(ExistingTag);
  }

    EntityPerTag.emplace(Tag, InEntity);
    TagPerEntity.emplace(InEntity.GetID(), Tag);
}

bool ECSManager::EntityHasTag(Entity InEntity, const std::string& Tag) const
{
    const auto IT = TagPerEntity.find(static_cast<int>(InEntity.GetID()));
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
    const auto IT = TagPerEntity.find(static_cast<int>(InEntity.GetID()));
    if (IT != TagPerEntity.end())
    {
        EntityPerTag.erase(IT->second);
        TagPerEntity.erase(IT);
    }
}

std::string ECSManager::GetEntityTag(Entity InEntity) const
{
  const auto IT = TagPerEntity.find(static_cast<int>(InEntity.GetID()));
  if (IT == TagPerEntity.end())
  {
    return {};
  }

  return IT->second;
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

std::vector<Entity> ECSManager::GetAllEntities() const
{
  std::vector<Entity> Result;
  Result.reserve(EntityComponentSignatures.size());

  for (size_t EntityID = 0; EntityID < EntityComponentSignatures.size(); ++EntityID)
  {
    const bool bHasComponents = EntityComponentSignatures[EntityID].any();
    const bool bHasTag = TagPerEntity.find(static_cast<int>(EntityID)) != TagPerEntity.end();
    const bool bHasGroup = GroupPerEntity.find(static_cast<int>(EntityID)) != GroupPerEntity.end();

    if (bHasComponents || bHasTag || bHasGroup)
    {
      Entity InEntity(EntityID);
      InEntity.Manager = const_cast<ECSManager*>(this);
      Result.push_back(InEntity);
    }
  }

  return Result;
}

void ECSManager::RemoveEntityGroup(Entity InEntity)
{
    const auto GroupIT = GroupPerEntity.find(static_cast<int>(InEntity.GetID()));
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
    for (const Entity& InEntity : EntitiesToBeAdded)
    {
        AddEntityToSystems(InEntity);
    }
    EntitiesToBeAdded.clear();

    
    // Process the entities that are waiting to be killed from the active systems

    for (const Entity& InEntity : EntitiesToBeRemoved)
    {
        RemoveEntityFromSystems(InEntity);

        EntityComponentSignatures[InEntity.GetID()].reset();
        
        // Remove the entity from the component pools
        for (const std::shared_ptr<IPool>& Pool : ComponentPools)
        {
            if (Pool)
            {
                Pool->RemoveEntityFromPool(InEntity.GetID());
            }
        }
        
        // Make EntityID available to be reused
        FreeEntityIDs.push_back(InEntity.GetID());
        
        //Remove any traces of that entity from the tag or group maps        
        RemoveEntityTag(InEntity);
        RemoveEntityGroup(InEntity);
    }
    EntitiesToBeRemoved.clear();
}
