#include "pch.h"
#include "ExhaustMaterial.h"
#include "Texture.h"

ExhaustMaterial::ExhaustMaterial(ID3D11Device* pDevice, const std::wstring& path, Texture* pDiffuseTexture)
	: Material(pDevice, path)
	, m_pDiffuseMapVariable(nullptr)
	, m_pDiffuse(pDiffuseTexture)
{
}

ExhaustMaterial::~ExhaustMaterial()
{
	delete m_pDiffuse;
}

void ExhaustMaterial::Update(Mesh* mesh)
{
	Material::Update(mesh);

	SetEffectShaderResource("gDiffuseMap", m_pDiffuseMapVariable, m_pDiffuse);
}
