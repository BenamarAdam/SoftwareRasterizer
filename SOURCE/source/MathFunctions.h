#pragma once
#include "EMath.h"
#include <array>
#include "Structs.h"
#include <vector>
#include <tuple>

inline float NDCToSSX(float x, float screenWidth)
{
	return (x + 1) / 2 * screenWidth;
}

inline float NDCToSSY(float y, float screenHeight)
{
	return (1 - y) / 2 * screenHeight;
}

inline Elite::FPoint3 NDCPointToSS(const Elite::FPoint3& NDCPoint, float screenWidth, float screenHeight)
{
	Elite::FPoint3 screenSpacePoint{ NDCPoint };
	screenSpacePoint.x = NDCToSSX(NDCPoint.x, screenWidth);
	screenSpacePoint.y = NDCToSSY(NDCPoint.y, screenHeight);
	return screenSpacePoint;
}

inline float CalcSSX(const float x, const int screenWidth) // x
{
	return (x + 1) / 2 * screenWidth;
}
inline float CalcSSY(const float y, const int screenHeight) // y
{
	return (1 - y) / 2 * screenHeight;
}

inline unsigned int PixelToBufferIndex(unsigned int x, unsigned int y, unsigned int width)
{
	return x + (y * width);
}

inline void PointToScreenBoundaries(Elite::FPoint2& point, float width, float height)
{
	if (point.x < 0)
	{
		point.x = 0;
	}
	if (point.x >= width)
	{
		point.x = width - 1.f;
	}

	if (point.y < 0)
	{
		point.y = 0;
	}
	if (point.y >= height)
	{
		point.y = height - 1.f;
	}
}

template<typename Type>
inline Type Interpolate(const std::array<Type, 3>& attributesToInterpolate,
	const std::array<const Vertex*, 3>& pOwningVertices,
	const float interpolatedLinearDepth)
{
	return
		(	attributesToInterpolate[0] / pOwningVertices[0]->pos.w * pOwningVertices[0]->weight +
			attributesToInterpolate[1] / pOwningVertices[1]->pos.w * pOwningVertices[1]->weight +
			attributesToInterpolate[2] / pOwningVertices[2]->pos.w * pOwningVertices[2]->weight
			) * interpolatedLinearDepth;
}

template<typename Type>
inline bool InRange(Type value, Type minRange, Type maxRange)
{
	return value >= minRange && value <= maxRange;
}

inline void TransformVertexPos(const Elite::FMatrix4& transform, std::vector<Vertex>& verticesToTransform)
{
	for (Vertex& vertex : verticesToTransform)
	{
		vertex.pos = transform * vertex.pos;
	}
}

inline void TransformVertexNormals(const Elite::FMatrix3& transform, std::vector<Vertex>& verticesToTransform)
{
	for (Vertex& vertex : verticesToTransform)
	{
		vertex.normal = transform * vertex.normal;
	}
}

inline void TransformVertexTangents(const Elite::FMatrix3& transform, std::vector<Vertex>& verticesToTransform)
{
	for (Vertex& vertex : verticesToTransform)
	{
		vertex.tangent = transform * vertex.tangent;
	}
}

inline void PerspectiveDivide(std::vector<Vertex>& vertices)
{
	for (Vertex& vertex : vertices)
	{
		vertex.pos.x /= vertex.pos.w;
		vertex.pos.y /= vertex.pos.w;
		vertex.pos.z /= vertex.pos.w;
	}
}

inline void VertsToSS(const int width, const int height, std::vector<Vertex>& vertices)
{
	for (Vertex& vertex : vertices)
	{
		vertex.pos.xy = Elite::FPoint2{ CalcSSX(vertex.pos.x, width), CalcSSY(vertex.pos.y, height) };
	}

}

inline std::tuple<Elite::FPoint2, Elite::FPoint2> GetBoundingBox(float width, float height, const std::vector<Vertex>& vertices)
{
	if (vertices.empty())
	{
		return std::make_tuple(Elite::FPoint2{}, Elite::FPoint2{});
	}

	Elite::FPoint2 topLeft{ vertices[0].pos.xy };
	Elite::FPoint2 bottomRight{ vertices[0].pos.xy };

	for (unsigned int i{ 1 }; i < vertices.size(); ++i)
	{
		topLeft.x = std::min(topLeft.x, vertices[i].pos.x);
		topLeft.y = std::min(topLeft.y, vertices[i].pos.y);

		bottomRight.x = std::max(bottomRight.x, vertices[i].pos.x);
		bottomRight.y = std::max(bottomRight.y, vertices[i].pos.y);
	}

	PointToScreenBoundaries(topLeft, width, height);
	PointToScreenBoundaries(bottomRight, width, height);

	return std::make_tuple(topLeft, bottomRight);
}
