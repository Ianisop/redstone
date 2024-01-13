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

using namespace Lapis;

namespace Game {

    bool flag = false;

    // Physics
    float gravity = 1.0f;

    // Player entity and components
    Entity playerEntity;
    std::shared_ptr<Transform> playerTransform;
    float movementSpeed = 2;

    // Other entities and components
    std::vector<Entity> physicsObjects;
    std::vector<Entity> liveObjects;
    Entity ground;
    Entity cube2;
    Entity cube1;
    Entity entityInSight;

    // MISC
    std::random_device rd;
    std::mt19937 gen(rd());

    void InitializePlayer()
    {
        playerEntity = Entity();
        playerTransform = playerEntity.AddComponent<Transform>();
        *playerTransform = mainCamera;
    }


    //this is basically the start function
    void InitializeObjects()
    {
        if (flag != true)
        {
            // OBJECTS IN SCENE GO HERE
            liveObjects.push_back(cube2);
            liveObjects.push_back(ground);
            liveObjects.push_back(cube1);

            ground.tag = "ground";
            cube2.tag = "cube";

            std::cout << "Ground tag: " << ground.tag << std::endl;
            std::cout << "Cube2 tag: " << cube2.tag << std::endl;

            //this adds comps
            for (Entity& obj : liveObjects)
            {
                obj.AddComponent<Transform>();
                obj.AddComponent<Renderer>();
                std::string* k = &obj.tag;
                std::cout << std::format("Creating obj({}) \n", *k);

            }

            std::cout << "LiveObjects: " << liveObjects.size() << std::endl;
            flag = true;
        }
    }

    void SpawnStuff()
    {
        // Draw ground
        auto groundTransform = ground.GetComponent<Transform>();
        auto groundRenderer = ground.GetComponent<Renderer>();

        if (groundTransform && groundRenderer)
        {
            std::cout << "Rendering ground..." << std::endl;

            groundTransform->scale.x = 10;
            groundTransform->scale.z = 10;
            groundTransform->pos.y = 0;

            Draw::D3::Plane(*groundTransform, *groundRenderer->col);

            std::cout << "Ground rendered at y = " << &groundTransform->pos.y << std::endl;
            std::cout << "Main camera y = " << mainCamera.pos.y << std::endl;

            *groundRenderer->col = "798674";
        }


        //raycasting debugging
        auto cubeTransform = cube1.GetComponent<Transform>();
        auto cubeRenderer = cube1.GetComponent<Renderer>();
        //std::cout << std::format("renderer: {} \n ", groundRenderer->col.get()->r);

        if (cubeTransform && cubeRenderer)
        {
            cubeTransform->pos.x = 10;
            Draw::D3::Cube(*cubeTransform, *cubeRenderer->col);
            std::cout << mainCamera.pos.y << std::endl;
        }



        //this draws all the objects(cubes rn cos im retarded)
        for (Entity& obj : liveObjects)
        {
            auto objTransform = obj.GetComponent<Transform>();
            auto objRenderer = obj.GetComponent<Renderer>();

            if (objTransform && objRenderer && obj.tag != "cube")
            {
                //std::cout << "Rendering cube with tag: " << obj.tag << std::endl;

                Draw::D3::Cube(*objTransform, *objRenderer->col);
            }
        }

    }


    void Run()
    {
        InitializePlayer();
        InitializeObjects();
        SpawnStuff();
        MovePlayer();
        mainCamera.pos.y = 0.1;
        
    }

    void MovePlayer() {
        // Movement controls
        if (GetAsyncKeyState('A')) {
            mainCamera.pos -= mainCamera.Right() * movementSpeed * deltaTime;
        }
        if (GetAsyncKeyState('D')) {
            mainCamera.pos += mainCamera.Right() * movementSpeed * deltaTime;
        }
        if (GetAsyncKeyState('W')) {
            mainCamera.pos += mainCamera.Forward() * movementSpeed * deltaTime;
        }
        if (GetAsyncKeyState('S')) {
            mainCamera.pos -= mainCamera.Forward() * movementSpeed * deltaTime;
        }

        //std::cout << Vec3::Magnitude(mainCamera.pos) << std::endl;
    }

    bool IsLineColliding(const Vec3& start, const Vec3& end, Transform& transformComponent) {
        Vec3 minBounds = transformComponent.position - Vec3(transformComponent.scale.x / 2.0f, transformComponent.scale.y / 2.0f, transformComponent.scale.z / 2.0f);
        Vec3 maxBounds = transformComponent.position + Vec3(transformComponent.scale.x / 2.0f, transformComponent.scale.y / 2.0f, transformComponent.scale.z / 2.0f);

        // Check for intersection between the point and the AABB
        return (start.x >= minBounds.x && start.x <= maxBounds.x &&
            start.y >= minBounds.y && start.y <= maxBounds.y &&
            start.z >= minBounds.z && start.z <= maxBounds.z) ||
            (end.x >= minBounds.x && end.x <= maxBounds.x &&
                end.y >= minBounds.y && end.y <= maxBounds.y &&
                end.z >= minBounds.z && end.z <= maxBounds.z);
    }

    Vec3 Raycast(Transform raycastStart, float dist) {
        Vec3 raycastDirection = raycastStart.Forward();
        Vec3 raycastEnd = raycastStart.pos + raycastDirection * dist;

        for (Entity& object : liveObjects) {
            auto& transformComponent = *object.GetComponent<Transform>();
            if (IsLineColliding(raycastStart.pos, raycastDirection, transformComponent)) {
                return transformComponent.position;
            }
        }

        return Vec3();
    }
}
