#include "pch.h"

//Project includes
#include "EDirectxRenderer.h"

// DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dx11effect.h>


#include "SceneManager.h"

Elite::DirectxRenderer::DirectxRenderer(SDL_Window * pWindow)
	: Renderer(pWindow)
	, m_IsInitialized{ false }
{
	const HRESULT initResult = InitialiseDirectX();

	if (!FAILED(initResult))
	{
		m_IsInitialized = true;
		std::cout << "DirectX is ready\n";
	}
}

Elite::DirectxRenderer::~DirectxRenderer()
{
	m_pRenderTargetView->Release();
	m_pRenderTargetBuffer->Release();

	m_pDepthStencilView->Release();
	m_pDepthStencilBuffer->Release();

	m_pSwapChain->Release();
	
	if (m_pDeviceContext)
	{
		m_pDeviceContext->ClearState();
		m_pDeviceContext->Flush();
		m_pDeviceContext->Release();
	}

	m_pDevice->Release();
	m_pDXGIFactory->Release();
}

void Elite::DirectxRenderer::Render()
{
	if (!m_IsInitialized)
	{
		return;
	}

	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &m_ClearColor.r);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	for (Mesh* mesh : SceneManager::GetInstance().GetScene().GetMeshes())
	{
		mesh->Render(m_pDeviceContext);
	}

	m_pSwapChain->Present(0, 0);
}

ID3D11Device* Elite::DirectxRenderer::GetDevice() const
{
	return m_pDevice;
}

HRESULT Elite::DirectxRenderer::InitialiseDirectX()
{
	HRESULT result;
	
	{
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
		uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		result = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, &m_pDevice, &featureLevel, &m_pDeviceContext);
		if (FAILED(result))
			return result;
		
		result = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&m_pDXGIFactory));
		if (FAILED(result))
			return result;
	}

	
	{
		DXGI_SWAP_CHAIN_DESC chainDesc{};
		chainDesc.BufferDesc.Width = m_Width;
		chainDesc.BufferDesc.Height = m_Height;
		chainDesc.BufferDesc.RefreshRate.Numerator = 1;
		chainDesc.BufferDesc.RefreshRate.Denominator = 60;
		chainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		chainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		chainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		chainDesc.SampleDesc.Count = 1;
		chainDesc.SampleDesc.Quality = 0;
		chainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		chainDesc.BufferCount = 1;
		chainDesc.Windowed = true;
		chainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		chainDesc.Flags = 0;
		
		SDL_SysWMinfo sysWMinfo{};
		SDL_VERSION(&sysWMinfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMinfo);
		chainDesc.OutputWindow = sysWMinfo.info.win.window;
		
		result = m_pDXGIFactory->CreateSwapChain(m_pDevice, &chainDesc, &m_pSwapChain);
		
		if (FAILED(result))
		{
			return result;
		}
	}

	
	{
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &m_pDepthStencilBuffer);
		if (FAILED(result))
		{
			return result;
		}
		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result))
		{
			return result;
		}
	}

	
	{
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result))
		{
			return result;
		}

		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, 0, &m_pRenderTargetView);
		if (FAILED(result))
		{
			return result;
		}
	}
	
	
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	D3D11_VIEWPORT viewport{};
	viewport.Width = static_cast<float>(m_Width);
	viewport.Height = static_cast<float>(m_Height);
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_pDeviceContext->RSSetViewports(1, &viewport);

	return S_OK;
}


