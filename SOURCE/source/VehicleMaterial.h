#pragma once
#include "Material.h"



class VehicleMaterial final : public Material
{
public:
	VehicleMaterial(ID3D11Device* pDevice, const std::wstring& path);
	VehicleMaterial(VehicleMaterial& other) = delete;
	VehicleMaterial(VehicleMaterial&& other) = delete;
	VehicleMaterial operator=(VehicleMaterial& other) = delete;
	VehicleMaterial operator=(VehicleMaterial&& other) = delete;
	~VehicleMaterial() override;

	void Update(Mesh* mesh) override;
	
	void SetDiffuseTexture(Texture* pDiffuseTexture);
	void SetNormalMap(Texture* pNormalMap);
	void SetSpecularMap(Texture* pSpecularMap);
	void SetGlossinessMap(Texture* pGlossinessMap);

private:
	ID3DX11EffectMatrixVariable* m_pWorldMatrixVariable;
	ID3DX11EffectMatrixVariable* m_pViewInverseMatrixVariable;

	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable;

	Texture* m_pDiffuse;
	Texture* m_pNormalMap;
	Texture* m_pSpecularMap;
	Texture* m_pGlossinessMap;
};

