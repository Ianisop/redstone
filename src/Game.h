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
	bool IsCollisionInDirection(const Vec3& direction);
	void RegisterColliders();
	Vec3 CalculatePushbackVector(const Vec3& newPosition, const Vec3& minBounds, const Vec3& maxBounds);
	bool IsPointInBounds(const Vec3& point, const Vec3& minBounds, const Vec3& maxBounds);

}