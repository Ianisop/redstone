#pragma once
#include "Lapis/LapisTypes.h"
#include <chrono>
#include <thread>
#include <functional>
#include "ECS/Entity.h"

using namespace Lapis;
namespace Game
{
	void Run();
	void Move();
	void SpawnStuff();
	void MovePlayer();
	bool IsGrounded(Vec3& pos);
	void ApplyGravity(Vec3& pos);
	Entity Raycast(Transform raycastStart, float dist);
	bool IsLineColliding(const Vec3& start, const Vec3& end, Transform& transformComponent);

}