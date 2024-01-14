#pragma once
#include "Component.h"
#include "Entity.h"
#include "../Lapis/LapisTypes.h"
using namespace Lapis;
struct Renderer : public Component
{
public:
	Color col = "ffffff";
	

};

struct Rigidbody : public Component
{
public:
	bool canCollide;
	bool isTrigger;
	Entity Raycast(Transform raycastStart, float dist, std::vector<Entity>& liveObjects);
	bool IsLineIntersecting(const Vec3& start, const Vec3& end, Transform& transformComponent);
};