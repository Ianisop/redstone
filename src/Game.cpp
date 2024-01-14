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
    std::vector<Entity> physicsObjects;
    std::vector<Entity> liveObjects;
    Entity ground("");
    Entity cube2("cube2");
    Entity cube1("cube1");
    Entity entityInSight("raycastHit");



    //TIMERS
    Timer spawner(2);

    // MISC
    std::random_device rd;
    std::mt19937 gen(rd());

    void MoveEntity(Entity& entity, const Vec3& direction) {
        auto transformComponent = entity.GetComponent<Transform>();
        auto rigidbodyComponent = entity.GetComponent<Rigidbody>();

        if (transformComponent && rigidbodyComponent) {
            if (!IsCollisionInDirection(entity.GetComponent<Transform>()->Forward()))
            {
                transformComponent->pos += direction * movementSpeed * deltaTime;
            }
        }
    }

    float RandomFloat(float min, float max) {
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }

    void InitializePlayer()
    {
        playerTransform = player.AddComponent<Transform>();
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
            //liveObjects.push_back(cube1);



            //physx objects 
           //physicsObjects.push_back(cube1);
            physicsObjects.push_back(cube2);
           physicsObjects.push_back(player);


            //this adds comps
            for (Entity &obj : liveObjects)
            {
                obj.AddComponent<Transform>();
                obj.AddComponent<Renderer>();

                std::cout << std::format("Creating entity({}) \n", obj.GetTag());

            }
            for (Entity &obj : physicsObjects)
            {
                obj.AddComponent<Rigidbody>();

                std::cout << std::format("Creating Phys body({}) \n", obj.GetTag());

            }
            std::cout << "LiveObjects: " << liveObjects.size() << std::endl;
            std::cout << "PhysObjects: " << physicsObjects.size() << std::endl;
            RegisterColliders();
            flag = true;
        }

    }

    auto OnCollision2 = [](Entity& other) {
        std::cout << "Collision detected with " << other.GetTag() << std::endl;

        };

    void RegisterColliders()
    {
        for (Entity& entity : physicsObjects)
        {
            auto trans = entity.GetComponent<Transform>();
            auto rigidbody = entity.GetComponent<Rigidbody>();

           if (trans && rigidbody)
           {
               std::cout << "setting up colls \n";
               Vec3 minBounds = ((trans->pos - trans->scale) / 2);
               Vec3 maxBounds = (trans->pos + trans->scale / 2);
               rigidbody->SetColliderBounds(minBounds, maxBounds);
               //Subscribe to collision events
               EventSystem::Instance().SubscribeToCollision(entity, OnCollision2);
            }
        }
    }
    void SpawnStuff()
    {
        //this draws all the objects(cubes rn cos im retarded)
        for (Entity& obj : liveObjects)
        {
            auto objTransform = obj.GetComponent<Transform>();
            auto objRenderer = obj.GetComponent<Renderer>();

            if (obj == &ground)
            {
                objTransform->pos.y = 0;
                objTransform->scale.x = 100;
                objTransform->scale.z = 100;
                objRenderer->col = "ff0000";
                Draw::D3::Plane(*objTransform.get(), objRenderer.get()->col);
            }
            if (obj == &cube2)
            {
                objTransform->pos.x = 3;
                objTransform->pos.y = 0.2;
            }

            if (obj == &cube1)
            {
                objTransform->pos.x = 0;
                objTransform->pos.z = 10;
                objTransform->pos.y = 0.2;
                objRenderer->col = "F698BD";
            }

            else
            {
                //std::cout << "Rendering cube with tag: " << obj.tag << std::endl;
                if(obj != &ground) Draw::D3::Cube(*objTransform.get(), objRenderer.get()->col);
                
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
           // if(coll)std::cout << entityInSight.GetTag() << std::endl;
            
        }

        mainCamera.pos.y = 0.2;
        
        
    }

    void MovePlayer() {
        // Movement controls
        if (GetAsyncKeyState('A')) {
            if (!IsCollisionInDirection(mainCamera.Right())) {
                mainCamera.pos -= mainCamera.Right() * movementSpeed * deltaTime;
            }
        }
        if (GetAsyncKeyState('D')) {
            if (!IsCollisionInDirection(-mainCamera.Right())) {
                mainCamera.pos += mainCamera.Right() * movementSpeed * deltaTime;
            }
        }
        if (GetAsyncKeyState('W')) {
            if (!IsCollisionInDirection(mainCamera.Forward())) {
                mainCamera.pos += mainCamera.Forward() * movementSpeed * deltaTime;
            }
        }
        if (GetAsyncKeyState('S')) {
            if (!IsCollisionInDirection(-mainCamera.Forward())) {
                mainCamera.pos -= mainCamera.Forward() * movementSpeed * deltaTime;
            }
        }

        // Debug 
        auto playerTransform = player.GetComponent<Transform>();
        if (playerTransform)
        {
            //std::cout << "mainCamera Position: " << mainCamera.pos << " playerEntity Transform Position: " << playerTransform->pos << std::endl;

        }
    }


    bool IsCollisionInDirection(const Vec3& direction)
    {
        // Calculate the potential new position after movement
        Vec3 newPosition = mainCamera.pos + direction * movementSpeed * deltaTime;

        // Check for collisions
        for (Entity object : liveObjects) {
            auto transformComponent = object.GetComponent<Transform>();
            auto rigidbodyComponent = object.GetComponent<Rigidbody>();
            
            if (transformComponent && rigidbodyComponent)
            {
                //std::cout << "comps found \n";
                if (rigidbodyComponent->IsLineIntersecting(mainCamera.pos, newPosition, *transformComponent)) {
                    std::cout << "Collision detected with object: " << object.GetTag() << std::endl;
                    std::cout << "Current Position: (" << mainCamera.pos.x << ", " << mainCamera.pos.y << ", " << mainCamera.pos.z << ")" << std::endl;
                    std::cout << "New Position: (" << newPosition.x << ", " << newPosition.y << ", " << newPosition.z << ")" << std::endl;
                    std::cout << "Object Position: (" << transformComponent->pos.x << ", " << transformComponent->pos.y << ", " << transformComponent->pos.z << ")" << std::endl;
                    return true;
                }
                std::cout << "no hit \n";
            }
        }
        return false;
    }


}
