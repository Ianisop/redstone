#pragma once

enum class ComponentType {
    Transform,
    Renderer,
    Physics,
    Input,

};

class Component {
public:
    virtual ~Component() = default;


    ComponentType GetType() const {
        return type;
    }

protected:
    ComponentType type;
};