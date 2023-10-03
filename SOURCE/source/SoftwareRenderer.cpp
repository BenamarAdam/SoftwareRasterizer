#include "pch.h"
#include "SoftwareRenderer.h"

//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "ERGBColor.h"
#include "SceneManager.h"
#include "MathFunctions.h"

SoftwareRenderer::SoftwareRenderer(SDL_Window* pWindow, Texture* pDiffuse, Texture* pNormal)
	: Renderer(pWindow)
	, m_pTexture(pDiffuse)
	, m_pNormalMap(pNormal)
{
	//Initialize
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = static_cast<uint32_t*>(m_pBackBuffer->pixels);

	m_DepthBuffer.resize(m_Width * m_Height, 1.0f);
}

SoftwareRenderer::~SoftwareRenderer()
{
	delete m_pTexture;
	delete m_pNormalMap;
}

void SoftwareRenderer::Render()
{
	SDL_LockSurface(m_pBackBuffer);

	const Scene& activeScene{ SceneManager::GetInstance().GetScene() };
	
	for (uint32_t row = 0; row < m_Height; ++row)
	{
		for (uint32_t col = 0; col < m_Width; ++col)
		{
			m_pBackBufferPixels[PixelToBufferIndex(col, row, m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
				static_cast<Uint8>(m_ClearColor.r * 255.f),
				static_cast<Uint8>(m_ClearColor.g * 255.f),
				static_cast<Uint8>(m_ClearColor.b * 255.f));
		}
	}

	for (const Geometry* geometry : activeScene.GetGeometries())
	{
		std::vector<Vertex> geometryVertices{ geometry->GetModelVerts() };
		geometry->Project(geometryVertices);

		std::vector<Vertex> outVertices{};
		geometry->Rasterize(geometryVertices, m_DepthBuffer, outVertices);

		for (const Vertex& vertex : outVertices)
		{
			RGBColor finalPixelColor;
			if (m_RenderDepthBuffer)
			{
				finalPixelColor = RGBColor{ vertex.pos.z, vertex.pos.z , vertex.pos.z };
			}
			else
			{
				finalPixelColor = m_pTexture->Sample(vertex.uv);
			}

			m_pBackBufferPixels
				[
					PixelToBufferIndex
					(
						static_cast<unsigned int>(roundf(vertex.pos.x)),
						static_cast<unsigned int>(roundf(vertex.pos.y)),
						m_Width
					)
				] = SDL_MapRGB(m_pBackBuffer->format,
					static_cast<Uint8>(finalPixelColor.r * 255.f),
					static_cast<Uint8>(finalPixelColor.g * 255.f),
					static_cast<Uint8>(finalPixelColor.b * 255.f));
		}
	}

	std::fill(m_DepthBuffer.begin(), m_DepthBuffer.end(), 1.0f);

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

bool SoftwareRenderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}

RGBColor SoftwareRenderer::ShadePixel(const Vertex& outVertex) const
{
	const FVector3 lightDirection{ .577f, -.577f, -.577f };
	const RGBColor lightColor{ 1.f, 1.f, 1.f };
	constexpr float intensity{ 2.f };


	const float observedArea{ Dot(-outVertex.normal, lightDirection) };

	const RGBColor diffuseColor{ m_pTexture->Sample(outVertex.uv) };

	return lightColor * intensity * diffuseColor * observedArea;
}

void SoftwareRenderer::ToggleRenderDepthBuffer()
{
	m_RenderDepthBuffer = !m_RenderDepthBuffer;
}
