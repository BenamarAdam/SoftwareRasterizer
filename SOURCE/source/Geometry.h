#pragma once
#include "EMath.h"
#include <vector>

#include "Structs.h"

using namespace Elite;

class Geometry
{
public:
	explicit Geometry(FPoint3 position, FVector3 forward = FVector3{0,0,1});
	virtual ~Geometry() = default;

	const FPoint3& GetPosition() const;
	void SetPosition(const FPoint3& position);

	const FVector3& GetForward() const;
	void SetForward(const FVector3& forward);

	const FMatrix4& GetTransform() const;

	virtual std::vector<Vertex> GetModelVerts() const = 0;

	virtual void Project(std::vector<Vertex>& vertices) const = 0;
	virtual bool Rasterize(std::vector<Vertex>& vertices, std::vector<float>& depthBuffer, std::vector<Vertex>& outVertices) const = 0;

protected:
	virtual void OnRecalculateTransform(){};
	void CalcTransform();

private:
	FPoint3 m_Pos;
	FVector3 m_Forward;
	FMatrix4 m_Transform;
};

