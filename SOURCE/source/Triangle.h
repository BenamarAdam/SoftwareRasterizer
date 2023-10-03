#pragma once
#include "Geometry.h"
#include "Structs.h"

class Triangle : public Geometry
{
public:
	
	static bool Hit(const FPoint2& pixel, std::vector<Vertex>& vertices);
	
};

