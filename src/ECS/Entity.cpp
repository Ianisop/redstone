#include "Entity.h"

bool Entity::operator==(const std::shared_ptr<Entity>& other)
{
    if (other->GetTag() == this->tag)
    {
        return true;
    }
    return false;
}

bool Entity::operator!=(const std::shared_ptr<Entity>& other)
{
    if (other->GetTag() != this->tag)
    {
        return true;
    }
    return false;
}



