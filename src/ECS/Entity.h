#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "src/Lapis/LapisTypes.h"
#include "Component.h"
#include <string.h>
#include <functional>
#include <vector>

class Entity {
public:
    Entity() {}
    Entity(std::string val) : tag(val) {}
    std::string GetTag() { return tag; }

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

        if (it != components.end() && it->second) {
            return std::static_pointer_cast<T>(it->second);
        }

        // If the component is not found or is a nullptr, return nullptr
        return nullptr;
    }




    bool operator==(const std::shared_ptr<Entity>& other);
    bool operator!=(const std::shared_ptr<Entity>& other);

private:
    std::unordered_map<std::string, std::shared_ptr<Component>> components;
    std::string tag;



};