#include "pch.h"
#include "Scene.h"

Scene::~Scene()
{
	for (Geometry* geometry : m_pGeos)
	{
		delete geometry;
		geometry = nullptr;
	}

	for (Mesh* mesh : m_pMeshes)
	{
		delete mesh;
		mesh = nullptr;
	}

	delete m_pCamera;
	m_pCamera = nullptr;
}

void Scene::AddGeometry(Geometry* geometry)
{
	m_pGeos.push_back(geometry);
}
const std::vector<Geometry*>& Scene::GetGeometries() const
{
	return m_pGeos;
}

void Scene::AddMesh(Mesh* geometry)
{
	m_pMeshes.push_back(geometry);
}
const std::vector<Mesh*>& Scene::GetMeshes() const
{
	return m_pMeshes;
}

void Scene::SetCamera(Camera* pCamera)
{
	if (m_pCamera != pCamera)
	{
		delete m_pCamera;
	}
	m_pCamera = pCamera;
}
Camera* Scene::GetCamera() const
{
	return m_pCamera;
}
