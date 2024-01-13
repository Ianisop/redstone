#pragma once
#include "Component.h"
#include "Entity.h"
#include "../Lapis/LapisTypes.h"

struct Renderer : public Component
{
public:
	std::unique_ptr<Lapis::Color> col = std::make_unique<Lapis::Color>("ffffff");
	

};
