#include "Components.h"
#include "../Lapis/LapisTypes.h"
#include "../Lapis/Engine.h"

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
        if (rigidbody != nullptr && !rigidbody->kinematic) {
            rigidbody->acceleration += gravity;
            rigidbody->ApplyFriction();
            rigidbody->velocity += rigidbody->acceleration * deltaTime;
            rigidbody->ApplyDamping();  // Apply damping
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
                //std::cout << liveObjects[i]->GetTag() << " + " << liveObjects[k]->GetTag() << "\n";
                //if (liveObjects[i]->GetTag() == "ground") std::cout << liveObjects[i]->GetComponent<Rigidbody>()->position << "\n";
                ResolveCollision(*r1, *r2);
            }
        }
    }
}


void Rigidbody::ResolveCollision(Rigidbody& rb1, Rigidbody& rb2) {
    Vec3 collisionNormal = (rb2.position - rb1.position);
    collisionNormal.Normalize();
    Vec3 relativeVelocity = rb2.velocity - rb1.velocity;
    float relativeVelocityNormal = Vec3::Dot(relativeVelocity, collisionNormal);

    if (relativeVelocityNormal > 0) {
        return; // They are moving apart
    }

    float e = std::min(rb1.restitution, rb2.restitution);
    float impulseNormal = -(1 + e) * relativeVelocityNormal / (1 / rb1.mass + 1 / rb2.mass);

    rb1.velocity -= collisionNormal * impulseNormal / rb1.mass;
    rb2.velocity += collisionNormal * impulseNormal / rb2.mass;

    Vec3 minBounds1 = rb1.collider.minBounds;
    Vec3 maxBounds1 = rb1.collider.maxBounds;
    Vec3 minBounds2 = rb2.collider.minBounds;
    Vec3 maxBounds2 = rb2.collider.maxBounds;

    float penetrationDepth = std::max(0.0f, Vec3::Dot(rb2.position - rb1.position, collisionNormal) -
        (maxBounds1 - minBounds1).Length() / 2 -
        (maxBounds2 - minBounds2).Length() / 2);

    // Cap the maximum penetration correction
    float maxPenetrationCorrection = 0.1f;  // Adjust as needed
    penetrationDepth = std::min(penetrationDepth, maxPenetrationCorrection);

    Vec3 correction = collisionNormal * (penetrationDepth / (rb1.mass + rb2.mass));
    rb1.position -= correction * rb2.mass / (rb1.mass + rb2.mass);
    rb2.position += correction * rb1.mass / (rb1.mass + rb2.mass);

    // Reduce velocity based on penetration depth
    float penetrationVelocityReduction = 0.8f;  // Adjust to control bounce reduction
    rb1.velocity = rb1.velocity * penetrationVelocityReduction;
    rb2.velocity = rb2.velocity * penetrationVelocityReduction;
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

    // Add a threshold to stop the object
    float stopThreshold = 0.1f;  // Adjust as needed
    if (velocity.Length() < stopThreshold) {
        velocity = Vec3(0, 0, 0);
    }
}




bool Rigidbody::BoundingBoxCollision(const BoxCollider& box1, const BoxCollider& box2) {
    // Check for overlap between bounding boxes
    return (box1.maxBounds.x > box2.minBounds.x && box1.minBounds.x < box2.maxBounds.x &&
        box1.maxBounds.y > box2.minBounds.y && box1.minBounds.y < box2.maxBounds.y &&
        box1.maxBounds.z > box2.minBounds.z && box1.minBounds.z < box2.maxBounds.z);
}
