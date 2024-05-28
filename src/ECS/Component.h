#pragma once

#include <memory>  

class Entity;

enum class ComponentType {
    Transform,
    Renderer,
    Rigidbody

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