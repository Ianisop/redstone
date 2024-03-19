#include <iostream>
#include "Components.h"
#include "../Game.h"


#define maximum(a, b) ((a) > (b) ? (a) : (b))
#define minimum(a, b) ((a) < (b) ? (a) : (b))


bool Rigidbody::IsLineIntersecting(const Vec3& start, const Vec3& end, Transform& transformComponent) {
    Vec3 minBounds = transformComponent.pos - Vec3(transformComponent.scale.x / 2.0f, transformComponent.scale.y / 2.0f, transformComponent.scale.z / 2.0f);
    Vec3 maxBounds = transformComponent.pos + Vec3(transformComponent.scale.x / 2.0f, transformComponent.scale.y / 2.0f, transformComponent.scale.z / 2.0f);

    float tMin = 0.0f;
    float tMax = std::numeric_limits<float>::infinity();

    for (int i = 0; i < 3; ++i) {
        float invDirection = 1.0f / (end[i] - start[i]);
        float tNear = (minBounds[i] - start[i]) * invDirection;
        float tFar = (maxBounds[i] - start[i]) * invDirection;

        if (invDirection < 0.0f) {
            std::swap(tNear, tFar);
        }

        tMin = maximum(tNear, tMin);
        tMax = minimum(tFar, tMax);

        if (tMin > tMax) {
            return false;
        }
    }

    // The line segment intersects with the AABB
    return true;
}

Entity Rigidbody::Raycast(Transform raycastStart, float dist, std::vector<Entity>& liveObjects) {
    Vec3 raycastDirection = raycastStart.Forward();
    Vec3 raycastEnd = raycastStart.pos + raycastDirection * dist;

    for (Entity& object : liveObjects) {
        auto& transformComponent = *object.GetComponent<Transform>();
        if (IsLineIntersecting(raycastStart.pos, raycastEnd, transformComponent)) {
            return object;
        }
    }

    return Entity();
}

bool Rigidbody::BoxIntersect(BoxCollider a, BoxCollider b)
{
    return (
        a.minBounds.x <= b.maxBounds.x &&
        a.maxBounds.x >= b.minBounds.x &&
        a.minBounds.y <= b.maxBounds.y &&
        a.maxBounds.y >= b.minBounds.y &&
        a.minBounds.z <= b.maxBounds.z &&
        a.maxBounds.z >= b.minBounds.z
        );
}

void Rigidbody::SetColliderBounds(const Vec3& min, const Vec3& max)
{
    collider.minBounds = min;
    collider.maxBounds = max;
}



void Rigidbody::ProcessPhysics(std::vector<std::shared_ptr<Entity>>& liveObjects)
{
    std::shared_ptr<Entity> player; // Initialize player as a shared pointer

    // Find the player entity in the liveObjects vector
    for (const auto& entity : liveObjects) {
        if (entity->GetTag() == "player") {
            player = entity;
            break; // Stop searching once player is found
        }
    }

    if (!player) {
        // Player not found, handle error or return
        std::cerr << "Error: Player entity not found." << std::endl;
        return;
    }

    for (size_t i = 0; i < liveObjects.size(); ++i) {
        for (size_t j = i + 1; j < liveObjects.size(); ++j) {
            Entity* entityA = liveObjects[i].get();
            Entity* entityB = liveObjects[j].get();

            // Print entity tags for debugging
            //std::cout << "Processing entities: " << entityA->GetTag() << " and " << entityB->GetTag() << std::endl;

            // Ensure that at least one of the entities is not the player entity
            if ((entityA != player.get() || entityB != player.get())) {
                auto rigidbodyA = entityA->GetComponent<Rigidbody>();
                auto rigidbodyB = entityB->GetComponent<Rigidbody>();

                if (rigidbodyA && rigidbodyB && rigidbodyA->canCollide && rigidbodyB->canCollide) {
                    auto transformA = rigidbodyA->collider;
                    auto transformB = rigidbodyB->collider;

                    //if collide, then do something
                    if (BoxIntersect(rigidbodyA->collider, rigidbodyB->collider))
                    {

                        std::cout << "Entities are colliding!: " << entityA->GetTag() << " and " << entityB->GetTag() << std::endl;
                    }
                }
            }
        }
    }

}


