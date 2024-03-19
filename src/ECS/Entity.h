#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "../Lapis/LapisTypes.h"
#include "Component.h"
#include <functional>
#include <vector>

class Entity {
public:
    Entity() {}
    Entity(std::string val) : tag(val) {}
    std::string GetTag() const { return this->tag; }

    template <typename T, typename... Args>
    std::shared_ptr<T> AddComponent(Args&&... args) const {
        const char* typeName = typeid(T).name();
        auto it = components.find(typeName);

        if (it == components.end())
        {
            // Component doesn't exist, add a new one
            std::shared_ptr<T> newComponent = std::make_shared<T>(std::forward<Args>(args)...);
            components[typeName] = newComponent;
            return newComponent;
        }

        // Component already exists, return the existing one
        return std::static_pointer_cast<T>(it->second);
    }

    template <typename T>
    void RemoveComponent() const
    {
        const char* typeName = typeid(T).name();
        auto it = components.find(typeName);

        if (it != components.end()) {
            // Component exists, remove it
            components.erase(it);
        }
    }

    template <typename T>
    std::shared_ptr<T> GetComponent() const {
        const char* typeName = typeid(T).name();
        auto it = components.find(typeName);

        if (it != components.end() && it->second) {
            return std::dynamic_pointer_cast<T>(it->second);
        }

        // If the component is not found or is a nullptr, return nullptr
        return nullptr;
    }

    using CollisionHandlerFunction = std::function<void(const Entity&)>;

    // Function to handle collision with another entity
    void OnCollision(const Entity& otherEntity) const {
        // Call each registered collision handling function
        for (const auto& collisionHandler : collisionHandlers) {
            collisionHandler(otherEntity);
        }
    }

    // Register a collision handling function
    void RegisterCollisionHandler(CollisionHandlerFunction handler) const {
        collisionHandlers.push_back(handler);
    }

    bool operator==(const Entity* other) const;
    bool operator!=(const Entity* other) const;

    explicit Entity(const std::shared_ptr<Entity>& other)
        : tag(other->tag), components(other->components) {
        
    }
private:
    mutable std::unordered_map<std::string, std::shared_ptr<Component>> components;
    std::string tag;
    mutable std::vector<CollisionHandlerFunction> collisionHandlers;
    
};
