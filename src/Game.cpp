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
    auto player = std::make_shared<Entity>("player");
    float movementSpeed = 2;

    // Other entities and components
    std::vector<std::shared_ptr<Entity>> physicsObjects;


    auto ground = std::make_shared<Entity>("");
    auto cube2 = std::make_shared<Entity>("cube2");
    auto cube1 = std::make_shared<Entity>("cube1");
    auto entityInSight = std::make_shared<Entity>("raycastHit");
    

    //collision callbacks
    //void OnPlayerCube1Collision(const CollisionEvent& callBack);
    //void GroundChecker(const CollisionEvent& callBack);

    



    //TIMERS
    //Timer spawner(2);

    // MISC
    std::random_device rd;
    std::mt19937 gen(rd());


    float RandomFloat(float min, float max) {
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }

    void InitializePlayer()
    {
        player->AddComponent<Transform>();
        player->AddComponent<Rigidbody>();
        player->GetComponent<Rigidbody>()->mass = 5;
        player->GetComponent<Transform>()->scale.x = 0.1;
        player->GetComponent<Transform>()->scale.z = 0.1;
        player->GetComponent<Transform>()->scale.y = 0.3;
    }


    //this is basically the start function
    void InitializeObjects()
    {
        if (flag != true)
        {

            //physx objects
            physicsObjects.push_back(player);
            physicsObjects.push_back(ground);
            physicsObjects.push_back(cube2);
            physicsObjects.push_back(cube1);
           // physicsObjects.push_back(ground);

           



            //this adds comps
            for (auto obj : physicsObjects)
            {
                if (obj != player)
                {
                    obj->AddComponent<Transform>();
                    obj->AddComponent<Renderer>();
                    obj->AddComponent<Rigidbody>();
                    obj->GetComponent<Rigidbody>()->mass = 1.0f;
                    
                }



                std::cout << std::format("Creating entity({}) \n", obj->GetTag());

            }

            physicsObjects.reserve(physicsObjects.size()); // Reserve space for efficiency
            std::cout << "PhysObjects: " << physicsObjects.size() << std::endl;
            //RegisterColliders();

            //SETUPS FOR OBJS

            ///////////////////////
            /////////?CUBE 1 ///////
            //////////////////////
            
             cube1->GetComponent<Transform>()->pos.y = 0.2;
             cube1->GetComponent<Transform>()->pos.x = 5;
             cube1->GetComponent<Renderer>()->col = "ffffff";

             ///////////////////////
             /////////?CUBE 2 ///////
             //////////////////////

             cube2->GetComponent<Transform>()->pos.y = 0;
             cube2->GetComponent<Transform>()->pos.x = 7;
             cube2->GetComponent<Transform>()->pos.z = 3;
             cube2->GetComponent<Renderer>()->col = "613513";
             


             ///////////////////////
             /////////?GROUND ///////
             //////////////////////

             ground->GetComponent<Transform>()->pos.y = -0.1;
             ground->GetComponent<Transform>()->scale.x = 100;
             ground->GetComponent<Transform>()->scale.z = 100;
             ground->GetComponent<Renderer>()->col = "ffffff";
             ground->GetComponent<Rigidbody>()->kinematic = true;
             for (auto obj : physicsObjects)
             {
                 auto rigidbody = obj->GetComponent<Rigidbody>();
                 auto transform = obj->GetComponent<Transform>();
                 if (rigidbody != nullptr && transform != nullptr)
                 {
                     rigidbody->position = transform->position;
                 }

             }




            SetupCollisionCallback();
            flag = true;
           
        }
        mainCamera.pos = player->GetComponent<Transform>()->pos;
        player->GetComponent<Transform>()->rot = mainCamera.rot;
    }

    void Blip()
    {
        //this draws all the objects(cubes rn cos im retarded)
        for (auto obj : physicsObjects)
        {
            auto objTransform = obj->GetComponent<Transform>();
            auto objRenderer = obj->GetComponent<Renderer>();

             //std::cout << "Rendering cube with tag: " << obj.tag << std::endl;
            if (objTransform && objRenderer)
            {
                if (obj != ground)Draw::D3::Cube(*objTransform.get(), objRenderer.get()->col);
                if (obj == ground)Draw::D3::Plane(*objTransform.get(), objRenderer.get()->col);
            }
        }

    }

    // UPDATE LOOP
    void Run()
    {
        InitializePlayer();
        InitializeObjects();
        Blip();
        UpdateColliders();
        Rigidbody::ProcessPhysics(5, physicsObjects);
        Input();

        if (debug)
        {
            DrawColliders();
        }
        
       // player->GetComponent<Transform>()->pos.y = 0.2;
    }

    void Input()
    {
        if (GetAsyncKeyState('F')&1)
        {
            debug = !debug;
            
        }
        if (GetAsyncKeyState('G') & 1)
        {
            cube1->GetComponent<Rigidbody>()->ApplyForce(Vec3(2,0,0));
        }
        if (GetAsyncKeyState('W'))
        {
            player->GetComponent<Rigidbody>()->ApplyForce(mainCamera.Forward() * movementSpeed);
        }
        if (GetAsyncKeyState('A'))
        {
            player->GetComponent<Rigidbody>()->ApplyForce(-mainCamera.Right() * movementSpeed);
        }
        if (GetAsyncKeyState('S'))
        {
            player->GetComponent<Rigidbody>()->ApplyForce(-mainCamera.Forward() * movementSpeed);
        }
        if (GetAsyncKeyState('D'))
        {
            player->GetComponent<Rigidbody>()->ApplyForce(mainCamera.Right() * movementSpeed);
        }


    }

    // takes care of setting up box colliders and collision callbacks
    void UpdateColliders()
    {
        for (int i = 0; i < physicsObjects.size();i++)
        {
            auto trans = physicsObjects[i]->GetComponent<Transform>();
            auto rigidbody = physicsObjects[i]->GetComponent<Rigidbody>();
            if (trans != nullptr && rigidbody != nullptr)
            {
               
                //std::cout << physicsObjects[i].GetTag() << std::endl;
                Vec3 minBounds = trans->pos - (trans->scale / 2);  // Calculate minimum bounds
                Vec3 maxBounds = trans->pos + (trans->scale / 2);  // Calculate maximum bounds
                rigidbody->SetColliderBounds(minBounds, maxBounds);  // Set collider bounds
                
            }

        }

        
      // std::cout << "Colliders Set up!\n";

    }

    void OnPlayerCube1Collision(const CollisionEvent callBack)
    {
        std::cout << "Collision between player and cube1 detected!" << std::endl;

    }
    //setups up callbacks
    void SetupCollisionCallback()
    {
        player->GetComponent<Rigidbody>()->AddCollisionCallback(OnPlayerCube1Collision);

    }



    void DrawColliders()
    {
        for (int i = 0; i < physicsObjects.size(); i++)
        {
            auto rigidbody = physicsObjects[i]->GetComponent<Rigidbody>();

            //std::cout << physicsObjects[i].GetTag() << "\n";
            if (!rigidbody) continue;
            Draw::D3::DrawWireCube(rigidbody->collider.minBounds, rigidbody->collider.maxBounds, "006400");
            

        }
    }




}
