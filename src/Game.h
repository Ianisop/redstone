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
	void InitializeObjects();
	void Blip();
	void UpdateColliders();
	void Input();
	void DrawColliders();
	void SetupCollisionCallback();


}