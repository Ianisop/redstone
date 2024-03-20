#include <iostream>
#include "Components.h"
#include "../Game.h"
#include "../Lapis/Engine.h"

#undef max
#define maximum(a, b) ((a) > (b) ? (a) : (b))
#define minimum(a, b) ((a) < (b) ? (a) : (b))
#define movementSpeed 2
#define friction 0.2f

bool Rigidbody::IsLineIntersecting(const Vec3& start, const Vec3& end, std::shared_ptr<Transform> transformComponent)
{
    Vec3 minBounds = transformComponent->pos - Vec3(transformComponent->scale.x / 2.0f, transformComponent->scale.y / 2.0f, transformComponent->scale.z / 2.0f);
    Vec3 maxBounds = transformComponent->pos + Vec3(transformComponent->scale.x / 2.0f, transformComponent->scale.y / 2.0f, transformComponent->scale.z / 2.0f);

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

std::shared_ptr<Entity> Rigidbody::Raycast(Transform raycastStart, float dist, std::vector<std::shared_ptr<Entity>>& liveObjects) {
    Vec3 raycastDirection = raycastStart.Forward();
    Vec3 raycastEnd = raycastStart.pos + raycastDirection * dist;

    for (auto& object : liveObjects) {
        auto transformComponent = object->GetComponent<Transform>();
        if (IsLineIntersecting(raycastStart.pos, raycastEnd, transformComponent)) {
            return object;
        }
    }

    return nullptr;
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





void Rigidbody::ProcessPhysics(std::vector<std::shared_ptr<Entity>>& liveObjects) {
    // Find the player entity in the liveObjects vector
    std::shared_ptr<Entity> player;
    for (const auto& entity : liveObjects) {
        if (entity->GetTag() == "player") {
            player = entity;
            break; // Stop searching once player is found
        }
    }

    // Check if player entity was found
    if (!player) {
        std::cerr << "Error: Player entity not found." << std::endl;
        return;
    }

    // Movement controls
    if (GetAsyncKeyState('A'))
        player->GetComponent<Rigidbody>()->velocity -= mainCamera.Right() * movementSpeed * deltaTime;
    if (GetAsyncKeyState('D'))
        player->GetComponent<Rigidbody>()->velocity += mainCamera.Right() * movementSpeed * deltaTime;
    if (GetAsyncKeyState('W'))
        player->GetComponent<Rigidbody>()->velocity += mainCamera.Forward() * movementSpeed * deltaTime;
    if (GetAsyncKeyState('S'))
        player->GetComponent<Rigidbody>()->velocity -= mainCamera.Forward() * movementSpeed * deltaTime;

    // Handle collisions and apply pushback forces
    for (size_t i = 0; i < liveObjects.size(); ++i) {
        for (size_t j = i + 1; j < liveObjects.size(); ++j) {
            Entity* entityA = liveObjects[i].get();
            Entity* entityB = liveObjects[j].get();

            // Ensure that at least one of the entities is not the player entity
            if ((entityA != player.get() || entityB != player.get())) {
                auto rigidbodyA = entityA->GetComponent<Rigidbody>();
                auto rigidbodyB = entityB->GetComponent<Rigidbody>();

                // Check if both entities have rigidbody components and can collide
                if (rigidbodyA && rigidbodyB && rigidbodyA->canCollide && rigidbodyB->canCollide) {
                    auto transformA = rigidbodyA->collider;
                    auto transformB = rigidbodyB->collider;

                    // Check for collision
                    if (BoxIntersect(rigidbodyA->collider, rigidbodyB->collider)) {
                        // Calculate collision normal
                        Vec3 collisionNormal = (player->GetComponent<Rigidbody>()->velocity - transformA.minBounds);
                        collisionNormal.Normalize();

                        // Calculate magnitude of pushback force
                        float forceMagnitude = Vec3::Dot(-player->GetComponent<Rigidbody>()->velocity, collisionNormal);

                        // Calculate pushback force using cross product
                        Vec3 pushForce = Vec3::Cross(-player->GetComponent<Rigidbody>()->velocity, collisionNormal);

                        // Apply pushback force to player's velocity
                        player->GetComponent<Rigidbody>()->velocity += pushForce * deltaTime;
                    }
                }
            }
        }
    }

    // Update player position based on velocity
    player->GetComponent<Transform>()->pos += player->GetComponent<Rigidbody>()->velocity * deltaTime;
}