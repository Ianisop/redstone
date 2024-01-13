#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "../Lapis/LapisTypes.h"
#include "Component.h"

class Entity {
private:
    std::unordered_map<std::string, std::shared_ptr<Component>> components;

public:
    template <typename T, typename... Args>
    std::shared_ptr<T> AddComponent(Args&&... args) {
        const char* typeName = typeid(T).name();
        auto it = components.find(typeName);

        if (it == components.end()) {
            // Component doesn't exist, add a new one
            std::shared_ptr<T> newComponent = std::make_shared<T>(std::forward<Args>(args)...);
            components[typeName] = newComponent;
            return newComponent;
        }

        // Component already exists, return the existing one
        return std::static_pointer_cast<T>(it->second);
    }

    template <typename T>
    void RemoveComponent() {
        const char* typeName = typeid(T).name();
        auto it = components.find(typeName);

        if (it != components.end()) {
            // Component exists, remove it
            components.erase(it);
        }
    }

    template <typename T>
    std::shared_ptr<T> GetComponent() {
        const char* typeName = typeid(T).name();
        auto it = components.find(typeName);

        if (it != components.end()) {
            return std::static_pointer_cast<T>(it->second);
        }

        // Handle case when component is not found.
        // This may throw an exception or return a default-constructed shared_ptr.
       // throw std::runtime_error("Component not found");
    }
};
