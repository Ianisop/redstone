#include "EventSystem.h"

void EventSystem::SubscribeToCollision(Entity& entity, CollisionHandler handler) {
    collisionSubscribers.emplace_back(&entity, handler);
}

void EventSystem::NotifyCollision(Entity& entity) {
    for (auto& subscriber : collisionSubscribers) {
        if (subscriber.first == &entity) {
            subscriber.second(entity);
        }
    }
}