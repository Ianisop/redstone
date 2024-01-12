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
	float gravity = 9.81f;
	std::vector<Transform> physicsObjects; // list for the objects that will get physics



	//Transforms for instantiated obj
	Transform cube1;
	Transform cube2;
	Transform ground;
	bool flag = false; // debug
	static int checkerboardSize = 25;
	Color col;


	//PLAYER VARS
	float movementSpeed = 1;
	Vec3 playerVelocity;
	float jumpSpeed = -5;


	//MISC
	std::random_device rd;
	std::mt19937 gen(rd());

	void SpawnStuff()
	{
		if (flag == false)
		{
			//physicsObjects.push_back(mainCamera);
			cube2.pos.z = 2;
			cube2.pos.y = 2;
			flag = true;
		}

		for (int i = 0; i < checkerboardSize; i++) {
			for (int j = 0; j < checkerboardSize; j++) {
				ground = Transform(Vec3(i - checkerboardSize / 2, 0, j - checkerboardSize / 2), 0, 1);
				if (((i % 2) + j) % 2 == 1)
					col = "ffffff";
				else
					col = "000000";
				Draw::D3::Plane(ground, col);
			}
		}
		Draw::D3::Cube(Transform(cube2), Color(255, 1, 0, 1));


	}


	void Run()
	{
		SpawnStuff();
		MovePlayer();
		mainCamera.pos.y = 0;
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

		if (GetAsyncKeyState('A')) playerVelocity += mainCamera.Right() * movementSpeed;
		if (GetAsyncKeyState('D')) playerVelocity -= mainCamera.Right() * movementSpeed;
		if (GetAsyncKeyState('W')) playerVelocity -= mainCamera.Forward() * movementSpeed;
		if (GetAsyncKeyState('S')) playerVelocity += mainCamera.Forward() * movementSpeed;
		

		// Jumping control
		if (GetAsyncKeyState(VK_SPACE) && IsGrounded(mainCamera.pos)) {
			
			playerVelocity.y += jumpSpeed;
		}

	
		ApplyGravity(playerVelocity);
		
		

		mainCamera.pos += playerVelocity * deltaTime;

		//std::cout << std::format("{} , {}  {} \n",mainCamera.pos.y, ground.pos.y, IsGrounded(mainCamera.pos));
	}

	bool IsGrounded(Vec3& pos)
	{
		float closestGroundY = FLT_MIN;

		for (int i = 0; i < checkerboardSize; i++) {
			for (int j = 0; j < checkerboardSize; j++) {
				// Calculate current ground position based on the loop indices
				float currentGroundY = ground.pos.y + i;  // Adjust based on your grid configuration

				// Check if the current ground is closer to the player's y-position
				if (abs(pos.y - currentGroundY) < abs(pos.y - closestGroundY)) {
					closestGroundY = currentGroundY;
				}
			}
		}

		return pos.y <= closestGroundY;
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

}
