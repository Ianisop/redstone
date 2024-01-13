#pragma once
#include "Component.h"
#include "Entity.h"
#include "../Lapis/LapisTypes.h"

struct Renderer : public Component
{
public:
	Lapis::Color col = "ffffff";
	

};
