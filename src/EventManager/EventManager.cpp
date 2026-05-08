#include "EventManager.h"

#include "../Logger/Logger.h"

EventManager::EventManager()
{
    Logger::Log("EventManager::EventManager() Called EventManager Object Constructor Called");
}

EventManager::~EventManager()
{
    Logger::Log("EventManager::~EventManager() Called EventManager Object Destructor Called");   
}
