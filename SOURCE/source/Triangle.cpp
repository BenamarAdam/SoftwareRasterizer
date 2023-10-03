#include "pch.h"
#include "Triangle.h"

#include "MathFunctions.h"
#include "SceneManager.h"
#include "EMath.h"


bool Triangle::Hit(const FPoint2& pixel, std::vector<Vertex>& vertices)
{
	FVector2 pixelToVertex{ pixel - vertices[0].pos.xy };
	FVector3 edge{ vertices[1].pos.xyz - vertices[0].pos.xyz };
	const float crossA = Cross(edge.xy, pixelToVertex);
	if (crossA > 0.f)
	{
		return false;
	}

	pixelToVertex = pixel - vertices[1].pos.xy;
	edge = vertices[2].pos.xyz - vertices[1].pos.xyz;
	const float crossB = Cross(edge.xy, pixelToVertex);
	if (crossB > 0.f) 
	{
		return false;
	}

	pixelToVertex = pixel - vertices[2].pos.xy;
	edge = vertices[0].pos.xyz - vertices[2].pos.xyz;
	const float crossC = Cross(edge.xy, pixelToVertex);
	if (crossC > 0.f)
	{
		return false;
	}

	const float area{ Cross(FVector2{vertices[0].pos.xy - vertices[1].pos.xy}, FVector2{vertices[0].pos.xy - vertices[2].pos.xy}) };
	vertices[0].weight = crossB / area;
	vertices[1].weight = crossC / area;
	vertices[2].weight = crossA / area;

	return true;
}
