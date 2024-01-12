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


using namespace Lapis;

namespace Game
{

	//Physics
	float gravity = 1.0f;
	std::vector<Transform> physicsObjects; // list for the objects that will get physics



	//Transforms for instantiated obj
	std::vector<Transform> liveObjects;
	Transform cube1;
	Transform cube2;
	Transform ground;
	Transform transformInSight;
	bool flag = false; // debug



	//PLAYER VARS
	float movementSpeed = 2.5;
	Vec3 playerVelocity;
	float jumpSpeed = 10;


	//MISC
	std::random_device rd;
	std::mt19937 gen(rd());

	void SpawnStuff()
	{
		if (flag == false)
		{
			//physicsObjects.push_back(mainCamera);
			liveObjects.push_back(cube1);
			liveObjects.push_back(cube2);
			liveObjects.push_back(ground);
			liveObjects.push_back(transformInSight);
			ground.scale.x = 10;
			ground.scale.z = 10;
			ground.pos.y = 0;
			cube2.pos.z = 2;
			cube2.pos.y = 2;
			flag = true;
		}

		Draw::D3::Plane(Transform(ground), "ffffff");
		Draw::D3::Cube(Transform(cube2), "ffff00");


	}


	void Run()
	{
		SpawnStuff();
		MovePlayer();
		Raycast(mainCamera, 20);
		mainCamera.pos.y = 2;
	}


	// use for 3D box collision
	bool IsBoxColliding(Vec3 a, Vec3 b)
	{
		if(Vec3::Distance(a,b) <= 1)
		{
			// Collide
			std::cout << "Colliding" << "\n";
			return true;
		}
		return false;
	}

	//test collision
	void Move()
	{
		if (!IsBoxColliding(cube1.pos, cube2.pos))
		{
			cube1.pos.x += 0.05;
			cube2.pos.x -= 0.05;
		}
		else if (IsBoxColliding(cube1.pos, cube2.pos) && GetAsyncKeyState('F'))
		{
			cube1.pos.x -= 1;
			cube2.pos.x += 1;
		}
		
	}

	void MovePlayer()
	{
		// Movement controls
		playerVelocity = Vec3();  // Reset velocity

		if (GetAsyncKeyState('A')) playerVelocity -= mainCamera.Right() * movementSpeed;
		if (GetAsyncKeyState('D')) playerVelocity += mainCamera.Right() * movementSpeed;
		if (GetAsyncKeyState('W')) playerVelocity += mainCamera.Forward() * movementSpeed;
		if (GetAsyncKeyState('S')) playerVelocity -= mainCamera.Forward() * movementSpeed;
		

		//std::cout << std::format("{} , {}  {} \n",mainCamera.pos.y, ground.pos.y, IsGrounded(mainCamera.pos));
	}

	bool IsGrounded(Vec3& pos)
	{
		return pos.y <= ground.pos.y;
	}
	
	// Processes physics for all the stuff thats included
	void ProcessPhysics()
	{
		for (int i = 0; i < physicsObjects.size(); i++)
		{
			ApplyGravity(physicsObjects[i].pos);
		}
	}

	void ApplyGravity(Vec3& pos)
	{
		if (!IsGrounded(pos))
		{
			pos.y += gravity * deltaTime;
		}
	}



	Vec3 Raycast(Transform raycastStart, float dist)
	{
		// Get the player's position and forward direction
		Vec3 raycastDirection = raycastStart.Forward();

		// Calculate the end point of the ray
		Vec3 raycastEnd = raycastStart.pos + raycastDirection * dist;  // Adjust the distance as needed

		// Draw the ray for visualization
		Draw::D3::Line(raycastStart.pos, raycastEnd, "ff0000");


		for (const Transform& object : liveObjects)
		{
			if (IsLineColliding(raycastStart.pos, raycastEnd, object))
			{
				std::cout << "Ray hit object!\n";
				return object.pos;  // Return the position of the collided object
			}
		}


		return Vec3();
	}

	
	bool IsLineColliding(const Vec3& start, const Vec3& end, const Transform& object)
	{
		// Assuming the object is a cube for simplicity, you may need a more complex check for other shapes
		Vec3 minBounds = object.pos - Vec3(object.scale.x / 2.0f, object.scale.y / 2.0f, object.scale.z / 2.0f);
		Vec3 maxBounds = object.pos + Vec3(object.scale.x / 2.0f, object.scale.y / 2.0f, object.scale.z / 2.0f);

		// Check for intersection between the line and the axis-aligned bounding box (AABB)
		// This is a basic AABB intersection test and might need modification based on your specific needs
		return (start.x >= minBounds.x && start.x <= maxBounds.x &&
			start.y >= minBounds.y && start.y <= maxBounds.y &&
			start.z >= minBounds.z && start.z <= maxBounds.z) ||
			(end.x >= minBounds.x && end.x <= maxBounds.x &&
				end.y >= minBounds.y && end.y <= maxBounds.y &&
				end.z >= minBounds.z && end.z <= maxBounds.z);
	}



}
