#pragma once
#include <SDL_image.h>
#include <string>
#include "ERGBColor.h"
#include "EMath.h"

class Texture final
{
public:
	explicit Texture(const std::string& path, ID3D11Device* pDevice);
	~Texture();

	ID3D11ShaderResourceView* GetTextureResourceView() const;
	Elite::RGBColor Sample(const Elite::FVector2& uv) const;

private:
	SDL_Surface* m_pSurface{ nullptr };
	ID3D11Texture2D* m_pTexture{ nullptr };
	ID3D11ShaderResourceView* m_pTextureResourceView{ nullptr };
};