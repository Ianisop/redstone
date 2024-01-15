#include "Entity.h"

bool Entity::operator==(const Entity* other) const
{
	if (other->GetTag() == this->tag)
	{
		return true;
	}
	return false;
}
bool Entity::operator!=(const Entity* other) const
{
	if (other->GetTag() != this->tag)
	{
		return true;
	}
	return false;
}


