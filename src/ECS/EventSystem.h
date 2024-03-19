#pragma once
#include <functional>
#include <vector>
#include "../ECS/Entity.h"
#include "../ECS/Components.h"

class EventSystem {
public:
    using CollisionHandler = std::function<void(Entity&)>;

    static EventSystem& Instance() {
        static EventSystem instance;
        return instance;
    }

    void SubscribeToCollision(Entity& entity, CollisionHandler handler);
    void NotifyCollision(Entity& entity);

private:
    std::vector<std::pair<Entity*, CollisionHandler>> collisionSubscribers;
};

