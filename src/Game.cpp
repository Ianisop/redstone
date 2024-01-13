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


    // MISC
    std::random_device rd;
    std::mt19937 gen(rd());

    void InitializePlayer() {
        playerEntity = Entity();
        playerTransform = playerEntity.AddComponent<Transform>();
        // Set initial player properties
        playerTransform->position = Vec3(0.0f, 0.0f, 0.0f);

    }

    void InitializeObjects() {
        if (flag != true)
        {
            /* actual init function
            for (Entity& obj : liveObjects)
            {
                obj.AddComponent<Transform>();                      // add Transforms
                obj.AddComponent<Rigidbody>();                      // add Phyics component
                obj.AddComponent<Renderer>();                       // add Rendering component
            
            }*/



            // debuggg
            auto groundTransform = ground.AddComponent<Transform>();

            groundTransform->position = Vec3(0.0f, -2.0f, 0.0f);

            auto cube2Transform = cube2.AddComponent<Transform>();

            cube2Transform->position = Vec3(10.0f, 2.0f, 2.0f);
            liveObjects.push_back(cube2);
            flag = true;
        }

    }

    void SpawnStuff()
    {
        //Draw ground
        Draw::D3::Plane(*ground.GetComponent<Transform>(), ground.GetComponent<Renderer>()->col);

        for (Entity& obj : liveObjects) {
            Draw::D3::Cube(*obj.GetComponent<Transform>(), obj.GetComponent<Renderer>()->col);
        }
    }

    void Run() {
        InitializePlayer();
        InitializeObjects();
        SpawnStuff();
        MovePlayer();

    }

    void MovePlayer() {
        // Movement controls
        if (GetAsyncKeyState('A')) {
            playerTransform->position -= playerTransform->Right() * movementSpeed;
        }
        if (GetAsyncKeyState('D')) {
            playerTransform->position += playerTransform->Right() * movementSpeed;
        }
        if (GetAsyncKeyState('W')) {
            playerTransform->position += playerTransform->Forward() * movementSpeed;
        }
        if (GetAsyncKeyState('S')) {
            playerTransform->position -= playerTransform->Forward() * movementSpeed;
        }

        // Apply gravity
        //playerPhysics->ApplyGravity(playerTransform->position);
    }

	bool IsGrounded(Vec3& pos)
	{
		return pos.y <= ground.GetComponent<Transform>()->pos.y;
	}
	
	// Processes physics for all the stuff thats included
	void ProcessPhysics()
	{
		for (int i = 0; i < physicsObjects.size(); i++)
		{
			ApplyGravity(physicsObjects[i].GetComponent<Transform>()->pos);
		}
	}

	void ApplyGravity(Vec3& pos)
	{
		if (!IsGrounded(pos))
		{
			pos.y += gravity * deltaTime;
		}
	}



    bool IsLineColliding(const Vec3& start, const Vec3& end, Transform& transformComponent)
    {
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



    Vec3 Raycast(Transform raycastStart, float dist)
    {
        Vec3 raycastDirection = raycastStart.Forward();
        Vec3 raycastEnd = raycastStart.pos + raycastDirection * dist;


        for (Entity& object : liveObjects)
        {
            auto transformComponent = *object.GetComponent<Transform>();
            if (IsLineColliding(raycastStart.pos, raycastDirection, transformComponent))
            {
                return transformComponent.position;
            }
        }

        return Vec3();
    }

}



