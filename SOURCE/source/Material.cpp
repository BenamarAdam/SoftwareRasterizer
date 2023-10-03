#include "pch.h"
#include "Material.h"

#include <sstream>

#include "SceneManager.h"

Material::Material(ID3D11Device* pDevice, const std::wstring& path)
	: m_pEffect(nullptr)
{
	m_pEffect = LoadEffect(pDevice, path);
	if (m_pEffect == nullptr || !m_pEffect->IsValid())
	{
		std::cout << "Couldn't load material";
		return;
	}

	m_pTechniques.push_back(LoadTechnique("DefaultTechnique"));
	m_pTechniques.push_back(LoadTechnique("LinearTechnique"));
	m_pTechniques.push_back(LoadTechnique("AnisotropicTechnique"));
}
Material::~Material()
{
	for (auto* tech : m_pTechniques)
	{
		tech->Release();
	}
	m_pEffect->Release();
}

ID3DX11Effect* Material::GetEffect() const
{
	return m_pEffect;
}
ID3DX11EffectTechnique* Material::GetTechnique() const
{
	return m_pTechniques[m_TechniqueIndex];
}

void Material::NextTechnique()
{
	++m_TechniqueIndex;
	m_TechniqueIndex %= m_pTechniques.size();
}

void Material::Update(Mesh* mesh)
{
	const Camera* pCamera = SceneManager::GetInstance().GetScene().GetCamera();
	SetEffectMatrix("gWorldViewProj", m_pWorldViewProjVariable, pCamera->GetLHProjection() * pCamera->GetLHWorldToView() * mesh->GetTransform());
	
}

ID3DX11Effect* Material::LoadEffect(ID3D11Device* pDevice, const std::wstring& path)
{
	HRESULT result = S_OK;
	ID3D10Blob* pErrorBlob = nullptr;
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(path.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			auto* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

			std::wstringstream ss;
			for (unsigned int i{ 0 }; i < pErrorBlob->GetBufferSize(); i++)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			std::wcout << "Effect failed to load: " << path << std::endl;
			return nullptr;
		}
	}

	std::cout << "Loaded material\n";

	return pEffect;
}
ID3DX11EffectTechnique* Material::LoadTechnique(const std::string& techniqueName) const
{
	ID3DX11EffectTechnique* technique = m_pEffect->GetTechniqueByName(techniqueName.c_str());
	if (technique == nullptr || !technique->IsValid())
	{
		std::cout << techniqueName << " couldn't be loaded";
		return nullptr;
	}

	return technique;
}

void Material::SetEffectMatrix(const std::string& matrixVariableName, ID3DX11EffectMatrixVariable* pMeshMatrix,
	const Elite::FMatrix4& pMeshMatrixValue)
{
	pMeshMatrix = m_pEffect->GetVariableByName(matrixVariableName.c_str())->AsMatrix();
	if (!pMeshMatrix->IsValid())
	{
		std::cout << matrixVariableName << " is not valid\n";
		return;
	}
	
	const HRESULT res = pMeshMatrix->SetMatrix(*pMeshMatrixValue.data);
	if (FAILED(res))
	{
		std::cout << "Matrix variable invalid: " << matrixVariableName << std::endl;
		return;
	}
}
void Material::SetEffectShaderResource(const std::string& shaderVariableName, ID3DX11EffectShaderResourceVariable* pMeshShaderResource,
	Texture* pMeshShaderResourceValue)
{
	pMeshShaderResource = m_pEffect->GetVariableByName(shaderVariableName.c_str())->AsShaderResource();
	if (!pMeshShaderResource->IsValid())
	{
		std::cout << shaderVariableName << " couldn't be loaded\n";
		return;
	}
	
	if (pMeshShaderResource->IsValid() && pMeshShaderResourceValue != nullptr)
	{
		pMeshShaderResource->SetResource(pMeshShaderResourceValue->GetTextureResourceView());
	}
}