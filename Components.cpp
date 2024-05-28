#include "src/ECS/Components.h"
#include "src/Lapis/LapisTypes.h"
#include "src/Lapis/Engine.h"


#undef min
#undef max

bool firstFrame = true;
void Rigidbody::SetColliderBounds(const Vec3& min, const Vec3& max) {
    collider.minBounds = min;
    collider.maxBounds = max;
}

void Rigidbody::ApplyForce(const Vec3& force) {
    // F = ma, so a = F/m
    Vec3 forceAccel = force * (1.0f / mass);
    acceleration += forceAccel;
}

void Rigidbody::ApplyDamping() {
    float dampingFactor = 0.98f;  // Damping factor to reduce velocity over time
    velocity = velocity * dampingFactor;
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
        if (rigidbody != nullptr && !rigidbody->kinematic && !rigidbody->isSleeping) {
            rigidbody->acceleration += gravity;
            rigidbody->ApplyFriction();
            rigidbody->velocity += rigidbody->acceleration * deltaTime;
            rigidbody->ApplyDamping();  // Apply damping
            rigidbody->position += rigidbody->velocity * deltaTime;
            rigidbody->acceleration = Vec3(0, 0, 0);

            // Check if the object should go to sleep
            //rigidbody->CheckSleep();
        }

        auto transform = obj->GetComponent<Transform>();
        if (transform != nullptr && rigidbody != nullptr) {
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
    // Calculate relative velocity
    Vec3 relativeVelocity = rb2.velocity - rb1.velocity;

    // Calculate the direction vector from rb1 to rb2
    Vec3 direction = rb2.position - rb1.position;

    // Normalize the direction vector to get the collision normal
    Vec3 collisionNormal = direction;
    collisionNormal.Normalize();
    PositionalCorrection(rb1, rb2, collisionNormal);


}

void Rigidbody::PositionalCorrection(Rigidbody& rb1, Rigidbody& rb2, const Vec3& collisionNormal) {
    const float percent = 0.2f; // Usually between 20% to 80%
    const float slop = 0.01f; // Usually around 0.01 to 0.1

    // Calculate penetration depth using the collision normal
    float penetrationDepth = CalculatePenetrationDepth(rb1.collider, rb2.collider, collisionNormal);

    // Calculate the correction magnitude
    float correctionMagnitude = std::max(penetrationDepth - slop, 0.0f) / (1.0f / rb1.mass + 1.0f / rb2.mass);

    // Scale the correction based on the percent factor
    Vec3 correction = collisionNormal * (correctionMagnitude * percent);

    // Apply correction to the positions of the rigid bodies
    if (!rb1.kinematic) rb1.position -= correction / rb1.mass;
    if (!rb2.kinematic) rb2.position += correction / rb2.mass;
}




float Rigidbody::CalculatePenetrationDepth(const BoxCollider& box1, const BoxCollider& box2, const Vec3& normal) {
    Vec3 halfExtents1 = (box1.maxBounds - box1.minBounds) * 0.5f;
    Vec3 halfExtents2 = (box2.maxBounds - box2.minBounds) * 0.5f;

    Vec3 center1 = (box1.maxBounds + box1.minBounds) * 0.5f;
    Vec3 center2 = (box2.maxBounds + box2.minBounds) * 0.5f;

    Vec3 distance = center2 - center1;
    Vec3 overlap = halfExtents1 + halfExtents2 - Vec3(std::abs(distance.x), std::abs(distance.y), std::abs(distance.z));

    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        return overlap.x;
    }
    else if (overlap.y < overlap.x && overlap.y < overlap.z) {
        return overlap.y;
    }
    else {
        return overlap.z;
    }
}


void Rigidbody::CheckSleep() {
    float sleepVelocityThreshold = 0.1f; // Threshold below which the object is considered at rest
    float sleepTimeThreshold = 1.0f;     // Time in seconds before object is put to sleep
    static float sleepTimeAccumulator = 0.0f;

    if (velocity.Length() < sleepVelocityThreshold && acceleration.Length() == 0.0f) {
        sleepTimeAccumulator += deltaTime;
        if (sleepTimeAccumulator >= sleepTimeThreshold) {
            isSleeping = true;
            velocity = Vec3(0, 0, 0);
            acceleration = Vec3(0, 0, 0);
        }
    }
    else {
        sleepTimeAccumulator = 0.0f;
        isSleeping = false;
    }
}


void Rigidbody::ApplyFriction() {
    // Calculate normal force
    Vec3 normalForce = gravity * mass;

    // Calculate kinetic friction force
    if (true) {
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
    bool collision = (box1.maxBounds.x > box2.minBounds.x && box1.minBounds.x < box2.maxBounds.x &&
        box1.maxBounds.y > box2.minBounds.y && box1.minBounds.y < box2.maxBounds.y &&
        box1.maxBounds.z > box2.minBounds.z && box1.minBounds.z < box2.maxBounds.z);

    return collision;
}
