/*=============================================================================*/
// Copyright 2017-2019 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ERenderer.h: class that holds the surface to render too + DirectX initialization.
/*=============================================================================*/
#ifndef ELITE_RAYTRACING_RENDERER_DIRECTX
#define	ELITE_RAYTRACING_RENDERER_DIRECTX
#include "Renderer.h"
#include <cstdint>

struct SDL_Window;
struct SDL_Surface;

namespace Elite
{
	class DirectxRenderer final : public Renderer
	{
	public:
		DirectxRenderer(SDL_Window* pWindow);
		~DirectxRenderer() override;

		DirectxRenderer(const DirectxRenderer&) = delete;
		DirectxRenderer(DirectxRenderer&&) noexcept = delete;
		DirectxRenderer& operator=(const DirectxRenderer&) = delete;
		DirectxRenderer& operator=(DirectxRenderer&&) noexcept = delete;

		void Render() override;

		ID3D11Device* GetDevice() const;

	private:
		bool m_IsInitialized;

		// DirectX Resources
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGIFactory* m_pDXGIFactory;
		IDXGISwapChain* m_pSwapChain;
		
		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11DepthStencilView* m_pDepthStencilView;
		
		ID3D11Texture2D* m_pRenderTargetBuffer;
		ID3D11RenderTargetView* m_pRenderTargetView;

		HRESULT InitialiseDirectX();
	};
}

#endif