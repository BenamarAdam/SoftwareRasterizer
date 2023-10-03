#include "pch.h"
#include "Texture.h"

#include "MathFunctions.h"

Texture::Texture(const std::string& path, ID3D11Device* pDevice)
{
	// Load texture with SDL
	m_pSurface = IMG_Load(path.c_str());
	if (m_pSurface == nullptr)
	{
		std::cout << "Error creating SDL_Surface with path: " << path << std::endl;
		return;
	}

	// Make texture description
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = m_pSurface->w;
	desc.Height = m_pSurface->h;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = m_pSurface->pixels;
	initData.SysMemPitch = static_cast<UINT>(m_pSurface->pitch);
	initData.SysMemSlicePitch = static_cast<UINT>(m_pSurface->h * m_pSurface->pitch);

	HRESULT result = pDevice->CreateTexture2D(&desc, &initData, &m_pTexture);
	if (FAILED(result))
	{
		std::cout << "Error creating Texture with path: " << path << std::endl;
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	result = pDevice->CreateShaderResourceView(m_pTexture, &SRVDesc, &m_pTextureResourceView);
	if (FAILED(result))
	{
		std::cout << "Error creating TextureResourceView with path: " << path << std::endl;
		return;
	}

	
}

Texture::~Texture()
{
	if (m_pTextureResourceView != nullptr)
		m_pTextureResourceView->Release();

	if (m_pTexture != nullptr)
		m_pTexture->Release();

	SDL_FreeSurface(m_pSurface);
}

ID3D11ShaderResourceView* Texture::GetTextureResourceView() const
{
	return m_pTextureResourceView;
}

Elite::RGBColor Texture::Sample(const Elite::FVector2& uv) const
{
	Uint8 r, g, b;
	SDL_GetRGB(
		static_cast<Uint32*>(m_pSurface->pixels)
		[
			PixelToBufferIndex
			(
				static_cast<int>(uv.x * static_cast<float>(m_pSurface->w)),
				static_cast<int>(uv.y * static_cast<float>(m_pSurface->h)),
				m_pSurface->w)
		],
		m_pSurface->format,
		&r, &g, &b);
	return Elite::RGBColor{ static_cast<float>(r) / 255.f, static_cast<float>(g) / 255.f, static_cast<float>(b) / 255.f };
}
