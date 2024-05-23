#include "Components.h"
#include "../Lapis/LapisTypes.h"
#include "../Lapis/Engine.h"

void Rigidbody::SetColliderBounds(const Vec3& min, const Vec3& max) {
    collider.minBounds = min;
    collider.maxBounds = max;
}

void Rigidbody::ApplyForce(const Vec3& force) {
    // F = ma, so a = F/m
    Vec3 forceAccel = force * (1.0f / mass);
    acceleration += forceAccel;
}

void Rigidbody::ProcessPhysics(float steps, std::vector<std::shared_ptr<Entity>>& liveObjects) {
    for (int i = 0; i < steps; i++) {
        Step(liveObjects);
    }
}

void Rigidbody::Step(std::vector<std::shared_ptr<Entity>> liveObjects) {
    // Update velocity and position
    for (auto& obj : liveObjects) {
        auto rigidbody = obj->GetComponent<Rigidbody>();
        if (rigidbody != nullptr && !rigidbody->kinematic)
        {
            rigidbody->acceleration += gravity;
            rigidbody->ApplyFriction();
            rigidbody->velocity += rigidbody->acceleration * deltaTime;
            rigidbody->position += rigidbody->velocity * deltaTime;
            rigidbody->acceleration = Vec3(0, 0, 0);
        }

        auto transform = obj->GetComponent<Transform>();
        if (transform != nullptr) {
            transform->position = rigidbody->position;
        }
    }

    // Perform collision detection and resolution
    for (int i = 0; i < liveObjects.size(); i++) {
        for (int k = i + 1; k < liveObjects.size(); k++) {
            auto r1 = liveObjects[i]->GetComponent<Rigidbody>();
            auto r2 = liveObjects[k]->GetComponent<Rigidbody>();

            if (r1 && r2 && r1->canCollide && r2->canCollide && BoundingBoxCollision(r1->collider, r2->collider)) {
                ResolveCollision(*r1, *r2);
            }
        }
    }
}

void Rigidbody::ResolveCollision(Rigidbody& rb1, Rigidbody& rb2) {
    // Calculate collision normal
    Vec3 collisionNormal = (rb2.position - rb1.position);
    collisionNormal.Normalize();

    // Calculate relative velocity
    Vec3 relativeVelocity = rb2.velocity - rb1.velocity;

    // Calculate relative velocity along the normal
    float relativeVelocityNormal = Vec3::Dot(relativeVelocity, collisionNormal);

    // Calculate impulse for restitution
    float impulseNormal = -(1 + rb1.restitution) * relativeVelocityNormal /
        (1 / rb1.mass + 1 / rb2.mass);

    // Apply impulse to change velocities
    rb1.velocity -= collisionNormal * impulseNormal / rb1.mass;
    rb2.velocity += collisionNormal * impulseNormal / rb2.mass;
}

void Rigidbody::ApplyFriction() {
    // Calculate normal force
    Vec3 normalForce = gravity * mass;

    // Calculate kinetic friction force
    if (velocity.Length() > 0) {
        // Calculate relative velocity
        Vec3 relativeVelocity = -velocity; // Reverse velocity for friction calculation

        // Calculate the direction opposite to motion
        Vec3 frictionDirection = relativeVelocity;
        relativeVelocity.Normalize();

        // Calculate the magnitude of the friction force
        float frictionForceMagnitude = kineticFriction * normalForce.Length();

        // Apply friction force as acceleration
        acceleration += frictionDirection * frictionForceMagnitude / mass;
    }
}


bool Rigidbody::BoundingBoxCollision(const BoxCollider& box1, const BoxCollider& box2) {
    // Check for overlap between bounding boxes
    return (box1.maxBounds.x > box2.minBounds.x && box1.minBounds.x < box2.maxBounds.x &&
        box1.maxBounds.y > box2.minBounds.y && box1.minBounds.y < box2.maxBounds.y &&
        box1.maxBounds.z > box2.minBounds.z && box1.minBounds.z < box2.maxBounds.z);
}
