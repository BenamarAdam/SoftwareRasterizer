#include "pch.h"
#include "VehicleMaterial.h"
#include "SceneManager.h"

VehicleMaterial::VehicleMaterial(ID3D11Device* pDevice, const std::wstring& path) // Todo: needs to set material specific variables
	: Material(pDevice, path)
	, m_pWorldMatrixVariable(nullptr)
	, m_pViewInverseMatrixVariable(nullptr)
	, m_pDiffuseMapVariable(nullptr)
	, m_pNormalMapVariable(nullptr)
	, m_pSpecularMapVariable(nullptr)
	, m_pGlossinessMapVariable(nullptr)
	, m_pDiffuse(nullptr)
	, m_pNormalMap(nullptr)
	, m_pSpecularMap(nullptr)
	, m_pGlossinessMap(nullptr)
{
}

VehicleMaterial::~VehicleMaterial()
{
	delete m_pDiffuse;
	delete m_pNormalMap;
	delete m_pSpecularMap;
	delete m_pGlossinessMap;
}

void VehicleMaterial::Update(Mesh* mesh)
{
	Material::Update(mesh);
	
	const Camera* pCamera = SceneManager::GetInstance().GetScene().GetCamera();
	SetEffectMatrix("gWorldMatrix", m_pWorldMatrixVariable, mesh->GetTransform());
	SetEffectMatrix("gViewInverseMatrix", m_pViewInverseMatrixVariable, pCamera->GetLHViewToWorld());

	SetEffectShaderResource("gDiffuseMap", m_pDiffuseMapVariable, m_pDiffuse);
	SetEffectShaderResource("gNormalMap", m_pNormalMapVariable, m_pNormalMap);
	SetEffectShaderResource("gSpecularMap", m_pSpecularMapVariable, m_pSpecularMap);
	SetEffectShaderResource("gGlossinessMap", m_pGlossinessMapVariable, m_pGlossinessMap);
}

void VehicleMaterial::SetDiffuseTexture(Texture* pDiffuseTexture)
{
	delete m_pDiffuse;
	m_pDiffuse = pDiffuseTexture;
}
void VehicleMaterial::SetNormalMap(Texture* pNormalMap)
{
	delete m_pNormalMap;
	m_pNormalMap = pNormalMap;
}
void VehicleMaterial::SetSpecularMap(Texture* pSpecularMap)
{
	delete m_pSpecularMap;
	m_pSpecularMap = pSpecularMap;
}
void VehicleMaterial::SetGlossinessMap(Texture* pGlossinessMap)
{
	delete m_pGlossinessMap;
	m_pGlossinessMap = pGlossinessMap;
}
