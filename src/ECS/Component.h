#pragma once

#include <memory>  // For std::shared_ptr

class Entity;  // Forward declaration

enum class ComponentType {
    Transform,
    Renderer,
    // Add more component types as needed
};

class Component {
public:
    virtual ~Component() = default;

    ComponentType GetType() const {
        return type;
    }

    std::shared_ptr<Entity> GetParentEntity() const {
        return parentEntity;
    }

protected:
    Component(ComponentType componentType, std::shared_ptr<Entity> parentEntity)
        : type(componentType), parentEntity(parentEntity) {}

    // Default constructor
    Component() : type(ComponentType::Transform), parentEntity(nullptr) {}

private:
    ComponentType type;
    std::shared_ptr<Entity> parentEntity;
};
