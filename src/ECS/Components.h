#pragma once
#include "Component.h"
#include "Entity.h"
#include "../Lapis/LapisTypes.h"
using namespace Lapis;

struct BoxCollider
{
	Vec3 minBounds;
	Vec3 maxBounds;
};


struct Renderer : public Component
{
public:
	Color col = "ffffff";
	

};

struct Rigidbody : public Component
{
public:
	BoxCollider collider;

	bool canCollide = true;
	bool isTrigger = false;
	std::shared_ptr<Entity> Raycast(Transform raycastStart, float dist, std::vector<std::shared_ptr<Entity>>& liveObjects);
	bool IsLineIntersecting(const Vec3& start, const Vec3& end, Transform& transformComponent);
	static bool BoxIntersect(BoxCollider a, BoxCollider b);
	static void ProcessPhysics(std::vector<std::shared_ptr<Entity>>& liveObjects);
	void SetColliderBounds(const Vec3& min, const Vec3& max);

};