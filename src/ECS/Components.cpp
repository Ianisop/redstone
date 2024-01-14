#include <iostream>
#include "Components.h"
#include "../Game.h"


#define maximum(a, b) ((a) > (b) ? (a) : (b))
#define minimum(a, b) ((a) < (b) ? (a) : (b))


bool Rigidbody::IsLineIntersecting(const Vec3& start, const Vec3& end, Transform& transformComponent) {
    Vec3 minBounds = transformComponent.pos - Vec3(transformComponent.scale.x / 2.0f, transformComponent.scale.y / 2.0f, transformComponent.scale.z / 2.0f);
    Vec3 maxBounds = transformComponent.pos + Vec3(transformComponent.scale.x / 2.0f, transformComponent.scale.y / 2.0f, transformComponent.scale.z / 2.0f);


    float tMin = 0.0f;
    float tMax = std::numeric_limits<float>::infinity();

    for (int i = 0; i < 3; ++i) {
        float invDirection = 1.0f / (end[i] - start[i]);
        float tNear = (minBounds[i] - start[i]) * invDirection;
        float tFar = (maxBounds[i] - start[i]) * invDirection;

        if (invDirection < 0.0f) {
            std::swap(tNear, tFar);
        }

        tMin = maximum(tNear, tMin);
        tMax = minimum(tFar, tMax);

        if (tMin > tMax) {
            return false;
        }
    }

    // The line segment intersects with the AABB
    return true;
}



Entity Rigidbody::Raycast(Transform raycastStart, float dist, std::vector<Entity>& liveObjects) {
    Vec3 raycastDirection = raycastStart.Forward();
    Vec3 raycastEnd = raycastStart.pos + raycastDirection * dist;

    for (Entity& object : liveObjects) {
        auto& transformComponent = *object.GetComponent<Transform>();
        if (IsLineIntersecting(raycastStart.pos, raycastEnd, transformComponent)) {
            return object;
        }
    }

    return Entity();
}


