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


    //TIMERS
    Timer spawner(2);

    // MISC
    std::random_device rd;
    std::mt19937 gen(rd());



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
            liveObjects.push_back(cube1);


            //this adds comps
            for (Entity &obj : liveObjects)
            {
                obj.AddComponent<Transform>();
                obj.AddComponent<Renderer>();

                std::cout << std::format("Creating obj({}) \n", obj.GetTag());

            }

            std::cout << "LiveObjects: " << liveObjects.size() << std::endl;
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
        MovePlayer();
        entityInSight = player.GetComponent<Rigidbody>()->Raycast(mainCamera, 2000, liveObjects);
        //debug for raycasting
        if (entityInSight.GetTag() != "")
        {
            std::cout << entityInSight.GetTag() << std::endl;
        }

        mainCamera.pos.y = 0.2;
        
        
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


}
