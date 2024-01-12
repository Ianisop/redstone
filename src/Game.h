#pragma once
#include "Lapis/LapisTypes.h"
#include <chrono>
#include <thread>
#include <functional>

using namespace Lapis;
namespace Game
{
	void Run();
	void Move();
	void SpawnStuff();
	void MovePlayer();
	bool IsGrounded(Vec3& pos);
	void ApplyGravity(Vec3& pos);


}