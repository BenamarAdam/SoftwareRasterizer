#pragma once
struct IVertex
{
	Elite::FPoint3 pos;
	Elite::FPoint2 uv;
	Elite::FVector3 normal;
	Elite::FVector3 tangent;
};

struct Vertex
{
	Elite::FPoint4 pos{};
	Elite::RGBColor color{ 1,1,1 };
	Elite::FVector2 uv{};
	Elite::FVector3 normal{};
	Elite::FVector3 tangent{};
	float weight{};
};