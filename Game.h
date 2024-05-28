#pragma once
#include "src/Lapis/LapisTypes.h"
#include <chrono>
#include <thread>
#include <functional>
#include "src/ECS/Entity.h"

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