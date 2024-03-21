#pragma once
#include "Component.h"
#include "Entity.h"
#include "../Lapis/LapisTypes.h"
#include <unordered_map>

using namespace Lapis;

struct CollisionEvent 
{
	std::shared_ptr<Entity> entity1;
	std::shared_ptr<Entity> entity2;
	
};
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
	using CollisionCallback = std::function<void(const CollisionEvent)>;
	BoxCollider collider;
	Vec3 velocity;
	bool grounded;
	bool canCollide = true;
	bool isTrigger = false;
	std::shared_ptr<Entity> Raycast(Transform raycastStart, float dist, std::vector<std::shared_ptr<Entity>>& liveObjects);
	bool IsLineIntersecting(const Vec3& start, const Vec3& end, std::shared_ptr<Transform> transformComponent);
	static bool BoxIntersect(BoxCollider a, BoxCollider b);
	static void ProcessPhysics(std::vector<std::shared_ptr<Entity>>& liveObjects);
	void SetColliderBounds(const Vec3& min, const Vec3& max);

	// Add a collision callback listener
	void AddCollisionCallback(CollisionCallback callback) {
		collisionCallbacks.push_back(callback);
	}   
	void HandleCollision(std::shared_ptr<Entity> otherEntity)
	{
        // Create collision event
        CollisionEvent event;
		
		event.entity1 = this->GetParentEntity();
        event.entity2 = otherEntity;
        

        // Invoke collision callbacks
		if (event.entity1 && event.entity2)
		{
			for (const auto& callback : collisionCallbacks) {
				callback(event);
			}
		}

    }

	// Simulate collision detection and raise collision events
	void SimulateCollision(std::shared_ptr<Entity> otherEntity)
	{
		CollisionEvent event{ otherEntity };
		// Raise collision event for each registered callback
		for (const auto& callback : collisionCallbacks) {
			callback(event);
		}
	}


private:
	std::vector<CollisionCallback> collisionCallbacks;
};

