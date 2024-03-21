#pragma once
#include "Component.h"
#include "Entity.h"
#include "../Lapis/LapisTypes.h"
#include <unordered_map>

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
	using CollisionCallback = std::function<void(std::shared_ptr<Entity>)>;
	static std::unordered_map<std::shared_ptr<Entity>, Rigidbody::CollisionCallback> collisionCallbacks;

	BoxCollider collider;
	Vec3 velocity;
	bool canCollide = true;
	bool isTrigger = false;
	std::shared_ptr<Entity> Raycast(Transform raycastStart, float dist, std::vector<std::shared_ptr<Entity>>& liveObjects);
	bool IsLineIntersecting(const Vec3& start, const Vec3& end, std::shared_ptr<Transform> transformComponent);
	static bool BoxIntersect(BoxCollider a, BoxCollider b);
	static void ProcessPhysics(std::vector<std::shared_ptr<Entity>>& liveObjects);
	void SetColliderBounds(const Vec3& min, const Vec3& max);

	void SetCollisionCallback(std::shared_ptr<Entity> entity, Rigidbody::CollisionCallback callback)
	{
		collisionCallbacks[entity] = callback;
	}





private:
	CollisionCallback collisionCallback; 
};

