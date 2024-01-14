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


#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))


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
        entityInSight = Raycast(mainCamera, 2000);
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

    bool IsLineColliding(const Vec3& start, const Vec3& end, Transform& transformComponent) {
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

            tMin = max(tNear, tMin);
            tMax = min(tFar, tMax);

            if (tMin > tMax) {
                return false;
            }
        }

        // The line segment intersects with the AABB
        return true;
    }



    Entity Raycast(Transform raycastStart, float dist) {
        Vec3 raycastDirection = raycastStart.Forward();
        Vec3 raycastEnd = raycastStart.pos + raycastDirection * dist;

        for (Entity& object : liveObjects) {
            auto& transformComponent = *object.GetComponent<Transform>();
            if (IsLineColliding(raycastStart.pos, raycastEnd, transformComponent)) {
                return object;
            }
        }

        return Entity();
    }

}
