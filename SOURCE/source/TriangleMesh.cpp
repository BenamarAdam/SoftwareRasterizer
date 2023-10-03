#include "pch.h"
#include "TriangleMesh.h"
#include "SceneManager.h"
#include <tuple>
#include <array>

#include "MathFunctions.h"
#include "Triangle.h"

TriangleMesh::TriangleMesh(const FPoint3& position, const std::vector<IVertex>& vertices, const std::vector<unsigned>& indices, PrimitiveTopology topology)
	: Geometry(position)
	, m_Indices(indices)
	, m_Topology(topology)
{
	m_ModelVertices.reserve(vertices.size());
	for (const IVertex& iVertex : vertices)
	{
		m_ModelVertices.push_back(
			Vertex{
				iVertex.pos,
				RGBColor{1.f, 1.f, 1.f},
				FVector2{iVertex.uv.x, iVertex.uv.y},
				iVertex.normal,
				iVertex.tangent,
				0
			}
		);
	}

	m_WorldVertices.insert(m_WorldVertices.end(), m_ModelVertices.begin(), m_ModelVertices.end());
	CalcWorldVertices();
}

std::vector<Vertex> TriangleMesh::GetModelVerts() const
{
	return m_ModelVertices;
}

void TriangleMesh::Project(std::vector<Vertex>& vertices) const
{
	const Scene& activeScene{ SceneManager::GetInstance().GetScene() };
	const Camera* pCamera{ activeScene.GetCamera() };

	// Positions
	TransformVertexPos(pCamera->GetRHProjection() * pCamera->GetRHWorldToView() * GetTransform(), vertices);
	PerspectiveDivide(vertices);
	// Vertices now in NDC space

	// Normal & Tangent
	TransformVertexNormals(GetTransform(), vertices);
	TransformVertexTangents(GetTransform(), vertices);

	// Todo: View Direction 
}
bool TriangleMesh::Rasterize(std::vector<Vertex>& vertices, std::vector<float>& depthBuffer, std::vector<Vertex>& outVertices) const
{
	unsigned int maxIndex{};
	switch (m_Topology)
	{
	case PrimitiveTopology::TriangleList:
		maxIndex = static_cast<unsigned int>(m_Indices.size()) / 3;
		break;
	case PrimitiveTopology::TriangleStrip:
		maxIndex = static_cast<unsigned int>(m_Indices.size()) - 2;
		break;
	}

	for (unsigned int i{ 0 }; i < maxIndex; ++i)
	{
		std::vector<Vertex> triangleVertices{ GetTriangleVertices(i, vertices) };
		RasterizeSingleTriangle(triangleVertices, depthBuffer, outVertices);
	}

	return !outVertices.empty();
}

void TriangleMesh::CalcWorldVertices()
{
	for (unsigned int i{0}; i < m_ModelVertices.size(); ++i)
	{
		m_WorldVertices[i].pos = GetTransform() * m_ModelVertices[i].pos;
	}
}
void TriangleMesh::OnRecalculateTransform()
{
	CalcWorldVertices();
}

bool TriangleMesh::RasterizeSingleTriangle(std::vector<Vertex>& triangleVertices, std::vector<float>& depthBuffer, std::vector<Vertex>& outVertices) const
{
	for (const Vertex& vertex : triangleVertices)
	{
		if (!InRange(vertex.pos.z, 0.f, 1.f))
		{
			return false;
		}
	}
	
	const Camera* pCamera{ SceneManager::GetInstance().GetScene().GetCamera() };
	const int width{ pCamera->GetScreenWidth() };
	const int height{ pCamera->GetScreenHeight() };
	VertsToSS(width, height, triangleVertices);
	
	const std::tuple<FPoint2, FPoint2> points{ GetBoundingBox(static_cast<float>(width), static_cast<float>(height), triangleVertices) };
	const FPoint2 topLeft{ std::get<0>(points) };
	const FPoint2 bottomRight{ std::get<1>(points) };

	FPoint2 pixel{};
	for (auto row = static_cast<uint32_t>(std::ceilf(topLeft.y)); row < static_cast<uint32_t>(std::ceilf(bottomRight.y)); ++row)
	{
		pixel.y = static_cast<float>(row);

		for (auto col = static_cast<uint32_t>(std::ceilf(topLeft.x)); col < static_cast<uint32_t>(std::ceilf(bottomRight.x)); ++col)
		{
			pixel.x = static_cast<float>(col);
			if (Triangle::Hit(pixel, triangleVertices))
			{
				const float interpZ
				{
					1 /
					(
						1 / triangleVertices[0].pos.z * triangleVertices[0].weight +
						1 / triangleVertices[1].pos.z * triangleVertices[1].weight +
						1 / triangleVertices[2].pos.z * triangleVertices[2].weight
					)
				};

				if (interpZ > depthBuffer[PixelToBufferIndex(col, row, width)])

				depthBuffer[PixelToBufferIndex(col, row, width)] = interpZ;

				const float interpW
				{
					1 /
					(
						1 / triangleVertices[0].pos.w * triangleVertices[0].weight +
						1 / triangleVertices[1].pos.w * triangleVertices[1].weight +
						1 / triangleVertices[2].pos.w * triangleVertices[2].weight
					)

				};

				Vertex vertOut{};

				vertOut.pos.x = pixel.x;
				vertOut.pos.y = pixel.y;
				vertOut.pos.z = interpZ;
				vertOut.pos.w = interpW;
				
				const std::array<const Vertex*, 3> triangleVertexPointerArray{ &triangleVertices[0],&triangleVertices[1],&triangleVertices[2] };
				vertOut.uv = Interpolate
				(
					std::array<FVector2, 3>{triangleVertices[0].uv, triangleVertices[1].uv, triangleVertices[2].uv},
					triangleVertexPointerArray,
					interpW
				);
				vertOut.color = Interpolate
				(
					std::array<RGBColor, 3>{triangleVertices[0].color, triangleVertices[1].color, triangleVertices[2].color},
					triangleVertexPointerArray,
					interpW
				);
				vertOut.normal = Interpolate
				(
					std::array<FVector3, 3>{triangleVertices[0].normal, triangleVertices[1].normal, triangleVertices[2].normal},
					triangleVertexPointerArray,
					interpW
				);
				vertOut.tangent = Interpolate
				(
					std::array<FVector3, 3>{triangleVertices[0].tangent, triangleVertices[1].tangent, triangleVertices[2].tangent},
					triangleVertexPointerArray,
					interpW
				);

				outVertices.push_back(vertOut);
			}
		}
	}

	return !outVertices.empty();
}

std::vector<Vertex> TriangleMesh::GetTriangleVertices(unsigned triangleNumber, const std::vector<Vertex>& vertices) const
{
	std::vector<Vertex> out{};
	switch (m_Topology)
	{
		case PrimitiveTopology::TriangleList:
			{
				const unsigned int firstIndex{ triangleNumber * 3 };
				out.push_back(vertices[m_Indices[firstIndex]]);
				out.push_back(vertices[m_Indices[firstIndex + 1]]);
				out.push_back(vertices[m_Indices[firstIndex + 2]]);
			}
		break;
		case PrimitiveTopology::TriangleStrip:
			out.push_back(vertices[m_Indices[triangleNumber]]);
			if (triangleNumber % 2 == 0)
			{
				out.push_back(vertices[m_Indices[triangleNumber + 1]]);
				out.push_back(vertices[m_Indices[triangleNumber + 2]]);
			}
			else
			{
				out.push_back(vertices[m_Indices[triangleNumber + 2]]);
				out.push_back(vertices[m_Indices[triangleNumber + 1]]);
			}
		break;
	}
	return out;
}