#include "pch.h"
#include "Renderer.h"

Elite::Renderer::Renderer(SDL_Window* pWindow)
	: m_pWindow{ pWindow }
	, m_Width{}
	, m_Height{}
{
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);
}
