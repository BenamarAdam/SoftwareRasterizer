#pragma once
#include "Material.h"
class ExhaustMaterial final : public Material
{
public:
	ExhaustMaterial(ID3D11Device* pDevice, const std::wstring& path, Texture* pDiffuseTexture);
	ExhaustMaterial(ExhaustMaterial& other) = delete;
	ExhaustMaterial(ExhaustMaterial&& other) = delete;
	ExhaustMaterial operator=(ExhaustMaterial& other) = delete;
	ExhaustMaterial operator=(ExhaustMaterial&& other) = delete;
	~ExhaustMaterial() override;

	void Update(Mesh* mesh) override;

private:
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
	Texture* m_pDiffuse;
};

