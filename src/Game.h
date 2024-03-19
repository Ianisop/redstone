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
	void Blip();
	void MovePlayer();
	void UpdateColliders();

	void DrawColliders();


}