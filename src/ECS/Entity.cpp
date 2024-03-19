#include "Entity.h"

bool Entity::operator==(Entity* other)
{
	if (other->GetTag() == this->tag)
	{
		return true;
	}
	return false;
}
bool Entity::operator!=(Entity* other)
{
	if (other->GetTag() != this->tag)
	{
		return true;
	}
	return false;
}
