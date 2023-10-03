#pragma once
#include <vector>
#include "Mesh.h"
#include "ECamera.h"
#include "Geometry.h"

using namespace Elite;

class Scene final
{
public:
	// Todo: rule of 5
	~Scene();

	void AddGeometry(Geometry* geometry);
	void AddMesh(Mesh* geometry);

	const std::vector<Geometry*>& GetGeometries() const;
	const std::vector<Mesh*>& GetMeshes() const;
	Camera* GetCamera() const;

	void SetCamera(Camera* pCamera);

private:
	std::vector<Geometry*> m_pGeos{};
	std::vector<Mesh*> m_pMeshes{};
	Camera* m_pCamera{ nullptr };
};