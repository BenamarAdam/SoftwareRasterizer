#pragma once
#include <vector>

class Texture;
class Mesh;

class Material
{
public:
	Material(ID3D11Device* pDevice, const std::wstring& path);
	Material(Material& other) = delete;
	Material(Material&& other) = delete;
	Material operator=(Material& other) = delete;
	Material operator=(Material&& other) = delete;
	virtual ~Material();

	ID3DX11Effect* GetEffect() const;
	ID3DX11EffectTechnique* GetTechnique() const;

	void NextTechnique();

	virtual void Update(Mesh* mesh);

protected:
	ID3DX11EffectMatrixVariable* m_pWorldViewProjVariable;

	void SetEffectMatrix(const std::string& matrixVariableName, ID3DX11EffectMatrixVariable* pMeshMatrix, const Elite::FMatrix4& pMeshMatrixValue);
	void SetEffectShaderResource(const std::string& shaderVariableName, ID3DX11EffectShaderResourceVariable* pMeshShaderResource, Texture* pMeshShaderResourceValue);

private:
	ID3DX11Effect* m_pEffect;
	std::vector<ID3DX11EffectTechnique*> m_pTechniques{};
	unsigned int m_TechniqueIndex{ 0 };

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& path);
	ID3DX11EffectTechnique* LoadTechnique(const std::string& techniqueName) const;
};

