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
	Vec3 position;
	Vec3 velocity;
	Vec3 acceleration;
	float mass;
	BoxCollider collider;
	float restitution;
	float frictionCoeff; //used for collision
	float kineticFriction;
	float staticFriction;
	bool kinematic;
	bool canCollide = true;
	bool isTrigger = false;
	static constexpr Vec3 gravity = Vec3(0.0f, -9.81f, 0.0f); 
	void ApplyForce(const Vec3& force);
	void ApplyFriction();

	static void ProcessPhysics(float steps, std::vector<std::shared_ptr<Entity>>& liveObjects);
	void SetColliderBounds(const Vec3& min, const Vec3& max);
	static void ResolveCollision(Rigidbody& rb1, Rigidbody& rb2);
	static bool BoundingBoxCollision(const BoxCollider& box1, const BoxCollider& box2);
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
	static void Step(std::vector<std::shared_ptr<Entity>> liveObjects);

};

