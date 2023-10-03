#include "pch.h"
#include "Mesh.h"
#include "SceneManager.h"

Mesh::Mesh(ID3D11Device* pDevice, const std::vector<IVertex>& verts, const std::vector<unsigned int>& indices, Material* pMaterial,
	const Elite::FPoint3& position, const Elite::FVector3& forward)
	: m_pMaterial(pMaterial)
	, m_Pos(position)
	, m_Forward(forward)
{
	CalcTransform();
	
	HRESULT result = S_OK;
	static constexpr uint32_t numElements{ 4 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "NORMAL";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 20;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TANGENT";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 32;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	
	D3D11_BUFFER_DESC buffDesc{};
	buffDesc.Usage = D3D11_USAGE_IMMUTABLE;
	buffDesc.ByteWidth = sizeof(IVertex) * static_cast<uint32_t>(verts.size());
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.CPUAccessFlags = 0;
	buffDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subData{nullptr};
	subData.pSysMem = verts.data();

	result = pDevice->CreateBuffer(&buffDesc, &subData, &m_pVertexBuffer);
	if (FAILED(result))
		{
		return;
		}

	D3DX11_PASS_DESC passDesc{};
	m_pMaterial->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
	result = pDevice->CreateInputLayout(vertexDesc, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pVertexLayout);
	if (FAILED(result))
	{
		return;
	}
	
	m_IndicesAmt = static_cast<uint32_t>(indices.size());
	buffDesc.Usage = D3D11_USAGE_IMMUTABLE;
	buffDesc.ByteWidth = sizeof(uint32_t) * m_IndicesAmt;
	buffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	buffDesc.CPUAccessFlags = 0;
	buffDesc.MiscFlags = 0;
	subData.pSysMem = indices.data();
	result = pDevice->CreateBuffer(&buffDesc, &subData, &m_pIndexBuffer);
	if (FAILED(result))
	{
		return;
	}

	std::cout << "Loaded Mesh\n";
}

Mesh::~Mesh()
{
	m_pIndexBuffer->Release();
	m_pVertexLayout->Release();
	m_pVertexBuffer->Release();
	delete m_pMaterial;
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext)
{
	if (!m_Active)
	{
		return;
	}
	
	UINT stride = sizeof(IVertex);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetInputLayout(m_pVertexLayout);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pMaterial->Update(this);
	D3DX11_TECHNIQUE_DESC techDesc{}; // Todo: edited this
	m_pMaterial->GetTechnique()->GetDesc(&techDesc);
	for (UINT passIndex = 0; passIndex < techDesc.Passes; ++passIndex)
	{
		m_pMaterial->GetTechnique()->GetPassByIndex(0)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_IndicesAmt, 0, 0);
	}
}

Material* Mesh::GetMaterial()
{
	return m_pMaterial;
}

void Mesh::SetIsActive(bool active)
{
	m_Active = active;
}

bool Mesh::GetIsActive() const
{
	return m_Active;
}

Elite::FPoint3 Mesh::GetPosition() const
{
	return m_Pos;
}
void Mesh::SetPosition(const Elite::FPoint3& pos)
{
	m_Pos = pos;
	CalcTransform();
}

Elite::FVector3 Mesh::GetForward() const
{
	return m_Forward;
}
void Mesh::SetForward(const Elite::FVector3& forward)
{
	m_Forward = forward;
	CalcTransform();
}

Elite::FMatrix4 Mesh::GetTransform() const
{
	return m_Transform;
}

void Mesh::CalcTransform()
{
	const FVector3 worldup{ 0,1,0 };

	const FVector3 right{ Cross(worldup, m_Forward) };
	const FVector3 up{ Cross(m_Forward, right) };

	m_Transform[0] = FVector4{ right, 0 };
	m_Transform[1] = FVector4{ up, 0 };
	m_Transform[2] = FVector4{ m_Forward, 0 };
	m_Transform[3] = FVector4{ m_Pos.x, m_Pos.y, m_Pos.z, 1.f };
}
