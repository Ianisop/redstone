#pragma once
#include "Lapis/LapisTypes.h"
<<<<<<< Updated upstream
#include <chrono>
#include <thread>
#include <functional>
=======
>>>>>>> Stashed changes

using namespace Lapis;
namespace Game
{
	void Test();
	void Run();
	void Move();
<<<<<<< Updated upstream
	void SpawnStuff();
	void MovePlayer();
	bool IsGrounded(Vec3& pos);
	void ApplyGravity(Vec3& pos);


=======
	bool IsBoxColliding(Vec3 a,Vec3 b);
>>>>>>> Stashed changes
}