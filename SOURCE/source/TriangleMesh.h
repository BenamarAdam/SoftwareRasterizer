#pragma once
#include <vector>
#include "Geometry.h"
#include "Structs.h"
#include "Texture.h"

enum class PrimitiveTopology
{
	TriangleList,
	TriangleStrip
};

class TriangleMesh final : public Geometry
{
public:
	TriangleMesh(const FPoint3& position, const std::vector<IVertex>& vertices, const std::vector<unsigned int>& indices, 
		PrimitiveTopology topology = PrimitiveTopology::TriangleList);
	~TriangleMesh() override = default;

	std::vector<Vertex> GetModelVerts() const override;

	void Project(std::vector<Vertex>& vertices) const override;
	bool Rasterize(std::vector<Vertex>& vertices, std::vector<float>& depthBuffer, std::vector<Vertex>& outVertices) const override;

private:
	std::vector<Vertex> m_ModelVertices;
	std::vector<Vertex> m_WorldVertices;
	std::vector<unsigned> m_Indices;

	PrimitiveTopology m_Topology;

	void CalcWorldVertices();
	void OnRecalculateTransform() override;

	bool RasterizeSingleTriangle(std::vector<Vertex>& triangleVertices, std::vector<float>& depthBuffer, std::vector<Vertex>& outVertices) const;

	std::vector<Vertex> GetTriangleVertices(unsigned int triangleNumber, const std::vector<Vertex>& vertices) const;
};
