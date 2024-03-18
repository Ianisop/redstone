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
#include "Timer.h"





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
    Entity ground("");
    Entity cube2("cube2");
    Entity cube1("cube1");
    Entity entityInSight("raycastHit");
    Entity player("player");
    bool debug = false;



    //TIMERS
    //Timer spawner(2);

    // MISC
    std::random_device rd;
    std::mt19937 gen(rd());

    void MoveEntity(Entity& entity, const Vec3& direction) {
        auto transformComponent = entity.GetComponent<Transform>();
        auto rigidbodyComponent = entity.GetComponent<Rigidbody>();

        if (transformComponent && rigidbodyComponent)
        {
          transformComponent->pos += direction * movementSpeed * deltaTime;
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


            //this adds comps
            for (Entity &obj : liveObjects)
            {
                obj.AddComponent<Transform>();
                obj.AddComponent<Renderer>();

                std::cout << std::format("Creating entity({}) \n", obj.GetTag());

            }
            for (Entity& obj : physicsObjects)
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

    // UPDATE LOOP
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

    void MovePlayer()
    {
        // Movement controls
        if (GetAsyncKeyState('A')) 
        {
                mainCamera.pos -= mainCamera.Right() * movementSpeed * deltaTime;
            
        }
        if (GetAsyncKeyState('D')) 
        {
                mainCamera.pos += mainCamera.Right() * movementSpeed * deltaTime;
           
        }
        if (GetAsyncKeyState('W')) 
        {
                mainCamera.pos += mainCamera.Forward() * movementSpeed * deltaTime;
            
        }
        if (GetAsyncKeyState('S')) 
        {
                mainCamera.pos -= mainCamera.Forward() * movementSpeed * deltaTime;
            
        }
        if (GetAsyncKeyState('F'))
        {
            debug = !debug;
            
        }

        // Debug 
        auto playerTransform = playerEntity.GetComponent<Transform>();
        if (playerTransform)
        {
            std::cout << "mainCamera Position: " << mainCamera.pos << " playerEntity Transform Position: " << playerTransform->pos << std::endl;

        }
    }

    // takes care of setting up box colliders for now
    void RegisterColliders()
    {
        for (int i = 0; i < physicsObjects.size();i++)
        {
            auto trans = physicsObjects[i].GetComponent<Transform>();
            auto rigidbody = physicsObjects[i].GetComponent<Rigidbody>();
            if (trans != nullptr && rigidbody != nullptr)
            {
               //std::string tag = physicsObjects[i].GetTag();
               //sprint(tag);
               rigidbody->SetColliderBounds(((trans->pos - trans->scale) / 2), (trans->pos + trans->scale / 2));
                
            }

        }
        
        std::cout << "Colliders Set up!\n";

    }

    void DrawColliders()
    {
        for (int i = 0; i < physicsObjects.size(); i++)
        {
            auto transform = physicsObjects[i].GetComponent<Transform>();
            Draw::D3::Cube(*transform, "ff0000");

        }
    }
}
