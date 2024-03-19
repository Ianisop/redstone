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

    bool flag = false; // used to run the start function
    bool debug = false;

    // Physics
    float gravity = 1.0f;

    // Player entity stuff
    Entity player("player");
    float movementSpeed = 2;

    // Other entities and components
    std::vector<Entity> physicsObjects;
    std::vector<Entity> liveObjects;
    std::vector<std::shared_ptr<Entity>> sharedPhysicsObjects;
    Entity ground("");
    Entity cube2("cube2");
    Entity cube1("cube1");
    Entity entityInSight("raycastHit");
    
    



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
        player.AddComponent<Transform>();
        *player.GetComponent<Transform>() = mainCamera;
    }


    //this is basically the start function
    void InitializeObjects()
    {
        if (flag != true)
        {

            // OBJECTS IN SCENE GO HERE
            liveObjects.push_back(cube2);
            liveObjects.push_back(cube1);
            liveObjects.push_back(ground);




            //physx objects
            physicsObjects.push_back(player);
            physicsObjects.push_back(cube2);
            physicsObjects.push_back(cube1);

           



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
            sharedPhysicsObjects.reserve(physicsObjects.size()); // Reserve space for efficiency
            for (auto& entity : physicsObjects) {
                sharedPhysicsObjects.push_back(std::make_shared<Entity>(entity));
            }

            std::cout << "LiveObjects: " << liveObjects.size() << std::endl;
            std::cout << "PhysObjects: " << physicsObjects.size() << std::endl;
            //RegisterColliders();
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

            if (obj == ground)
            {
                objTransform->pos.y = 0;
                objTransform->scale.x = 100;
                objTransform->scale.z = 100;
                objRenderer->col = "ffffff";
                Draw::D3::Plane(*objTransform.get(), objRenderer.get()->col);
            }

            if (obj == cube1)
            {
                objTransform->pos.y = 1;
                objTransform->pos.x = 3;
                objTransform->pos.z = 2;

                objRenderer->col = "ffffff";

            }

            if (obj == cube2)
            {
                objTransform->pos.x = 3;
                objTransform->pos.y = 2;
                objRenderer->col = "ff0000";
                auto rigidbody = cube2.GetComponent<Rigidbody>();
                
                if (!rigidbody && objTransform)
                {
                    cube2.AddComponent<Rigidbody>();
                    auto rb = cube2.GetComponent<Rigidbody>();
                   // std::cout << "No rigidbody on cube2 \n";
                    Vec3 minBounds = objTransform->pos - (objTransform->scale / 2);  // Calculate minimum bounds
                    Vec3 maxBounds = objTransform->pos + (objTransform->scale / 2);  // Calculate maximum bounds
                    rb->SetColliderBounds(minBounds, maxBounds);  // Set collider bounds
                    //std::cout << rb->collider.minBounds << " " << rb->collider.maxBounds << "\n";
                    
                }
            }


             //std::cout << "Rendering cube with tag: " << obj.tag << std::endl;
             if(obj != ground) Draw::D3::Cube(*objTransform.get(), objRenderer.get()->col);
                
            
        }

    }

    // UPDATE LOOP
    void Run()
    {
        InitializePlayer();
        InitializeObjects();
        SpawnStuff();
        UpdateColliders();
        Rigidbody::ProcessPhysics(sharedPhysicsObjects);
        MovePlayer();
        DrawColliders();
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


    }

    // takes care of setting up box colliders for now
    void UpdateColliders()
    {
        for (int i = 0; i < physicsObjects.size();i++)
        {
            auto trans = physicsObjects[i].GetComponent<Transform>();
            auto rigidbody = physicsObjects[i].GetComponent<Rigidbody>();
            if (trans != nullptr && rigidbody != nullptr)
            {
               
                //std::cout << physicsObjects[i].GetTag() << std::endl;

                Vec3 minBounds = trans->pos - (trans->scale / 140);  // Calculate minimum bounds
                Vec3 maxBounds = trans->pos + (trans->scale / 140);  // Calculate maximum bounds
                rigidbody->SetColliderBounds(minBounds, maxBounds);  // Set collider bounds
                
            }

        }
        
      // std::cout << "Colliders Set up!\n";

    }


    void DrawColliders()
    {
        for (int i = 0; i < physicsObjects.size(); i++)
        {
            auto rigidbody = physicsObjects[i].GetComponent<Rigidbody>();

            //std::cout << physicsObjects[i].GetTag() << "\n";
            Draw::D3::DrawWireCube(rigidbody->collider.minBounds, rigidbody->collider.maxBounds, "006400");
            

        }
    }
}
