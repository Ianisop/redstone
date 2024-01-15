#include "Game.h"
#include "Lapis/LapisTypes.h"
#include "Lapis/Engine.h"
#include <iostream>
#include <format>
#include <coroutine>
#include <chrono>
#include <thread>
#include <functional>
#include <random>
#include <algorithm>
#include "ECS/Entity.h"
#include "ECS/Component.h"
#include "ECS/Components.h"
#include "ECS/EventSystem.h"
#include "Timer.h"



#ifndef minimum
#define minimum(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef maximum
#define maximum(a, b) (((a) > (b)) ? (a) : (b))
#endif




using namespace Lapis;

namespace Game {

    bool flag = false;

    // Physics
    float gravity = 1.0f;

    // Player entity and components
    Entity player("player");
    std::shared_ptr<Transform> playerTransform;
    float movementSpeed = 2;

    // Other entities and components
    std::vector<std::shared_ptr<Entity>> physicsObjects;
    std::vector<std::shared_ptr<Entity>> liveObjects;

    Entity ground("");
    Entity cube2("cube2");
    Entity cube1("cube1");
    Entity entityInSight("raycastHit");



    //TIMERS
    Timer spawner(2);

    // MISC
    std::random_device rd;
    std::mt19937 gen(rd());

    void MoveEntity(Entity& entity, const Vec3& direction)
    {
        auto transformComponent = entity.GetComponent<Transform>();
        auto rigidbodyComponent = entity.GetComponent<Rigidbody>();

        if (transformComponent && rigidbodyComponent) {
            if (!IsCollisionInDirection(entity.GetComponent<Transform>()->Forward()))
            {
                transformComponent->pos += direction * movementSpeed * deltaTime;
            }
        }
    }

    float RandomFloat(float min, float max)
    {
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }

    void InitializePlayer()
    {
        playerTransform = player.AddComponent<Transform>();
        *playerTransform = mainCamera;
        playerTransform->scale.x = 0.3;
        playerTransform->scale.z = 0.3;
    }


    //this is basically the start function
    void InitializeObjects()
    {
        if (flag != true)
        {
            // OBJECTS IN SCENE GO HERE
            auto cube2Ptr = std::make_shared<Entity>(cube2);
            auto groundPtr = std::make_shared<Entity>(ground);

            // live objects
            liveObjects.push_back(cube2Ptr);
            liveObjects.push_back(std::make_shared<Entity>(ground));

            // physx objects
            physicsObjects.push_back(cube2Ptr);  
            physicsObjects.push_back(std::make_shared<Entity>(player));

            // Add comps for all objects
            for (const auto& obj : liveObjects)
            {
                obj->AddComponent<Transform>();
                obj->AddComponent<Renderer>();
                std::cout << std::format("Creating entity({}) \n", obj->GetTag());
            }

            // Add Rigidbody comp for physicsObjects
            for (const auto& obj : physicsObjects)
            {
                obj->AddComponent<Rigidbody>();
                std::cout << std::format("Creating Phys body({}) \n", obj->GetTag());
            }

            std::cout << "LiveObjects: " << liveObjects.size() << std::endl;
            std::cout << "PhysObjects: " << physicsObjects.size() << std::endl;
            flag = true;
        }

        RegisterColliders();
    }


    auto OnCollision2 = [](Entity& other) {
        std::cout << "Collision detected with " << other.GetTag() << std::endl;

        };

    void RegisterColliders()
    {
       // std::cout << " PHYSICS OBJECTS LOADED" << physicsObjects.size() << std::endl;
        for (const std::shared_ptr<Entity>& entity : physicsObjects)
        {
            auto trans = entity->GetComponent<Transform>();
            auto rigidbody = entity->GetComponent<Rigidbody>();
           // std::cout << trans << rigidbody << entity->GetTag() << std::endl;
            if (trans && rigidbody)
            {
                //std::cout << "setting up colls " << entity->GetTag() << "\n";

                // Debug prints
                //std::cout << "Transform Component: " << trans->pos << " " << trans->scale << std::endl;

                // Calculate bounds based on the scale of the entity
                Vec3 minBounds = trans->pos - (trans->scale / 2.0f);
                Vec3 maxBounds = trans->pos + (trans->scale / 2.0f);

                //std::cout << "Calculated Min Bounds: " << minBounds << std::endl;
               // std::cout << "Calculated Max Bounds: " << maxBounds << std::endl;

                rigidbody->SetColliderBounds(minBounds, maxBounds);
            }
        }
    }

    void SpawnStuff()
    {
        for (const auto& obj : liveObjects)
        {
            auto objTransform = obj->GetComponent<Transform>();
            auto objRenderer = obj->GetComponent<Renderer>();

            if (*obj.get() == &ground)
            {
                objTransform->pos.y = 0;
                objTransform->scale.x = 100;
                objTransform->scale.z = 100;
                objRenderer->col = "ff0000";
                Draw::D3::Plane(*objTransform.get(), objRenderer.get()->col);
            }
            else if (*obj.get() == &cube2)
            {
                objTransform->pos.x = 3;
                objTransform->pos.y = 0.2;
                objRenderer->col = "F698BD";
                Draw::D3::Cube(*objTransform.get(), objRenderer.get()->col);

                // Draw wireframe cube for collider
                auto collider = obj->GetComponent<Rigidbody>()->collider;
                if (&collider)
                {
                    Draw::D3::DrawWireCube(collider.minBounds, collider.maxBounds, "008000");
                }
            }
            else
            {
                Draw::D3::Cube(*objTransform.get(), objRenderer.get()->col);
            }
        }
    }


    void Run()
    {
        InitializePlayer();
        InitializeObjects();
        SpawnStuff();
        Rigidbody::ProcessPhysics(physicsObjects);
        MovePlayer();
        
        entityInSight = player.GetComponent<Rigidbody>()->Raycast(mainCamera, 2000, liveObjects);
        //debug for raycasting
        if (entityInSight.GetTag() != "")
        {
            //BoxCollider* coll = &entityInSight.GetComponent<Rigidbody>()->collider;
            //if(coll)std::cout << entityInSight.GetTag() << std::endl;
            
        }

        mainCamera.pos.y = 0.2;
        
        
    }

    Vec3 Normalize(const Vec3& v) {
        float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        if (length != 0.0f) {
            return Vec3(v.x / length, v.y / length, v.z / length);
        }
        return Vec3(0.0f, 0.0f, 0.0f);
    }

    void MovePlayer() {
        // Movement controls
        Vec3 movementDirection;

        if (GetAsyncKeyState('A')) {
            movementDirection -= mainCamera.Right();
        }
        if (GetAsyncKeyState('D')) {
            movementDirection += mainCamera.Right();
        }
        if (GetAsyncKeyState('W')) {
            movementDirection += mainCamera.Forward();
        }
        if (GetAsyncKeyState('S')) {
            movementDirection -= mainCamera.Forward();
        }

        // Normalize the movement direction to ensure consistent speed in all directions
        movementDirection = Normalize(movementDirection);

        // Calculate the potential new position after movement
        Vec3 newPosition = player.GetComponent<Transform>()->pos + movementDirection * movementSpeed * deltaTime;

        // Check for collisions
        for (const std::shared_ptr<Entity>& object : liveObjects) {
            auto transformComponent = object->GetComponent<Transform>();
            auto rigidbodyComponent = object->GetComponent<Rigidbody>();

            if (transformComponent && rigidbodyComponent) {
                // Calculate the collider bounds
                auto collider = rigidbodyComponent->collider;
                Vec3 minBounds = transformComponent->pos + collider.minBounds;
                Vec3 maxBounds = transformComponent->pos + collider.maxBounds;

                // Check if the new position collides with the object
                if (IsPointInBounds(newPosition, minBounds, maxBounds)) {
                    // Collision detected, calculate the pushback vector
                    Vec3 pushback = CalculatePushbackVector(newPosition, minBounds, maxBounds);

                    // Adjust the new position to be just outside the collider bounds
                    newPosition += pushback;

                    // Update the player position
                    mainCamera.pos = newPosition;
                }
            }
        }
    }


    bool IsCollisionInDirection(const Vec3& direction)
    {
        // Calculate the potential new position after movement
        Vec3 newPosition = player.GetComponent<Transform>()->pos + direction * movementSpeed;

        // Check for collisions
        for (const std::shared_ptr<Entity>& object : liveObjects)
        {
            auto transformComponent = object->GetComponent<Transform>();
            auto rigidbodyComponent = object->GetComponent<Rigidbody>();

            if (transformComponent && rigidbodyComponent)
            {
                // Debug prints
                std::cout << "Checking collision with object: " << object->GetTag() << std::endl;
                std::cout << "Current Position: (" << player.GetComponent<Transform>()->pos.x << ", " << player.GetComponent<Transform>()->pos.y << ", " << player.GetComponent<Transform>()->pos.z << ")" << std::endl;
                std::cout << "New Position: (" << newPosition.x << ", " << newPosition.y << ", " << newPosition.z << ")" << std::endl;
                std::cout << "Object Position: (" << transformComponent->pos.x << ", " << transformComponent->pos.y << ", " << transformComponent->pos.z << ")" << std::endl;

                auto collider = rigidbodyComponent->collider;
                std::cout << "Collider Min Bounds: " << collider.minBounds << std::endl;
                std::cout << "Collider Max Bounds: " << collider.maxBounds << std::endl;

                if (rigidbodyComponent->IsLineIntersecting(mainCamera.pos, newPosition, *transformComponent))
                {
                    std::cout << "Collision detected with object: " << object->GetTag() << std::endl;
                    return true;
                }
            }
        }
        return false;
    }

    bool IsPointInBounds(const Vec3& point, const Vec3& minBounds, const Vec3& maxBounds) {
        return (point.x >= minBounds.x && point.x <= maxBounds.x &&
            point.y >= minBounds.y && point.y <= maxBounds.y &&
            point.z >= minBounds.z && point.z <= maxBounds.z);
    }

    // Calculate the pushback vector to resolve a collision
    Vec3 CalculatePushbackVector(const Vec3& newPosition, const Vec3& minBounds, const Vec3& maxBounds) {
        Vec3 pushback(0.0f, 0.0f, 0.0f);

        // Calculate the overlap along each axis
        float overlapX = maximum(0.0f, minimum(maxBounds.x, newPosition.x) - maximum(minBounds.x, newPosition.x));
        float overlapY = minimum(0.0f, minimum(maxBounds.y, newPosition.y) - maximum(minBounds.y, newPosition.y));
        float overlapZ = maximum(0.0f, minimum(maxBounds.z, newPosition.z) - maximum(minBounds.z, newPosition.z));

        // Determine the axis with the smallest overlap
        if (overlapX <= overlapY && overlapX <= overlapZ) {
            pushback.x = (newPosition.x < minBounds.x) ? -overlapX : overlapX;
        }
        else if (overlapY <= overlapX && overlapY <= overlapZ) {
            pushback.y = (newPosition.y < minBounds.y) ? -overlapY : overlapY;
        }
        else {
            pushback.z = (newPosition.z < minBounds.z) ? -overlapZ : overlapZ;
        }

        return pushback;
    }




}
