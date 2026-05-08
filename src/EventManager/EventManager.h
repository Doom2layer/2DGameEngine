#pragma once

#include "Event.h"
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <list>

/*
 * The IEventCallback is like a contract/interface it is like saying "Anyone who wants to be a listener MUST know how to handle an event"
 */

class IEventCallback
{
public:
    virtual ~IEventCallback() = default;

    void Execute(Event& E) {Call(E);}

private:
    virtual void Call(Event& E) = 0;
};

/*
 * The EventCallback is like a wrapper around the callback function pointers that we want to invoke when an event is broadcasted.
 * It inherits from IEventCallback and implements the Call method, which is responsible for invoking the actual callback function pointer with the correct event type.
 * This is a concrete listener. It knows two things:
 * Who owns the callback (TOwner — e.g. Game class)
 * What event it listens for (TEvent — e.g. CollisionEvent)
*/

template <typename TOwner, typename TEvent>
class EventCallback : public IEventCallback
{
    /* 
     * This is a member function pointer, it is a pointer to a member function of the TOwner class that takes a TEvent reference as an argument and returns void.
     * this is like a saving a phone number to call later, but instead of a phone number we are saving a pointer to a member function
     * that we will call later when the event is broadcasted.
     */
private:
    typedef void (TOwner::*CallbackFunction)(TEvent&);
    
public:
    EventCallback(TOwner* InOwner, CallbackFunction InCallbackFunction)
    {
        this->OwnerInstance = InOwner;
        this->CallbackFunctionInstance = InCallbackFunction;
    }
    virtual ~EventCallback() = default;
    
private:
    
    TOwner* OwnerInstance;
    CallbackFunction CallbackFunctionInstance;
    
    virtual void Call(Event& E) override
    {
        std::invoke(CallbackFunctionInstance, OwnerInstance, static_cast<TEvent&>(E));
    }
};
/*
 * A HandlerList is a linked list of smart pointers to listeners
 * We use a linked list because we want to be able to add and remove listeners from the list without having to worry about invalidating iterators or indices (which can happen with a vector if we add/remove elements).
 */

typedef std::list<std::unique_ptr<IEventCallback>> HandlerList;

/*
 * The EventManager is like a radio station 
 * - Listeners (e.g. Game class) can subscribe to certain event types (e.g. CollisionEvent) by providing a callback function pointer that will be called when the event is broadcasted.
 * - When an event is broadcasted, the EventManager goes through all the listeners that are subscribed to that event type and calls their callback functions with the event data.
 * - The EventManager uses a hash map (unordered_map) to store the list of listeners for each event type, where the key is the type index of the event type and the value is a unique pointer to a HandlerList (which is a linked list of listeners).
 * - The EventManager provides two main methods:
 *   - SubscribeToEvent: This method allows listeners to subscribe to a specific event type by providing a callback function pointer. It checks if there is already a HandlerList for that event type, and if not, it creates one. Then it adds the listener to the HandlerList for that event type.
 *   - BroadcastEvent: This method allows broadcasting an event of a specific type. It checks if there are any listeners subscribed to that event type, and if so, it goes through them and calls their callback functions with the event data. It creates an instance of the event using the provided arguments and passes it to the callback functions.
 */

class EventManager
{
public:
    EventManager();
    ~EventManager();
    
    void ClearSubscribers() { Subscribers.clear(); }
    
    /********************************************************************
     * Subscribe to an event type <T>
     * in our implementation a listener subscribes to an event
     * Example: EventManager->SubscribeToEvent<CollisionEvent>(this, &Game::OnCollision);
     ********************************************************************/
    template <typename TEvent, typename TOwner>
    void SubscribeToEvent(TOwner* OwnerInstance, void (TOwner::*CallbackFunction)(TEvent&));
    
    /********************************************************************
    * Broadcast an event of type <T>
    * in our implementation as soon as something emits an event we go and execute all listener callback functions
    * Example: EventManager->BroadcastEvent<CollisionEvent>(Player, Enemy);
    ********************************************************************/
    template <typename TEvent, typename ...TArgs>
    void BroadcastEvent(TArgs&&... InArgs);
    
private:
    std::unordered_map<std::type_index, std::unique_ptr<HandlerList>> Subscribers;
    
};

template <typename TEvent, typename TOwner>
void EventManager::SubscribeToEvent(TOwner* OwnerInstance, void(TOwner::* CallbackFunction)(TEvent&))
{
    if (!Subscribers[typeid(TEvent)].get())
    {
        Subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
    }
    auto Subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(OwnerInstance, CallbackFunction);
    Subscribers[typeid(TEvent)]->push_back(std::move(Subscriber));
}

template <typename TEvent, typename ...TArgs>
void EventManager::BroadcastEvent(TArgs&&... InArgs)
{
    std::list<std::unique_ptr<IEventCallback>>* Handlers = Subscribers[typeid(TEvent)].get();
    
    if (Handlers)
    {
        for (auto IT = Handlers->begin(); IT != Handlers->end(); ++IT)
        {
            IEventCallback* Handler = IT->get();
            TEvent Event(std::forward<TArgs>(InArgs)...);
            Handler->Execute(Event);
        }
    }
    
}
