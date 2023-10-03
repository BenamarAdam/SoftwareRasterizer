#pragma once
#ifndef ELITE_RAYTRACING_RENDERER_BASE
#define	ELITE_RAYTRACING_RENDERER_BASE

#include <cstdint>

struct SDL_Window;
struct SDL_Surface;

namespace Elite
{
	class Renderer
	{
	public:
		explicit Renderer(SDL_Window* pWindow);
		virtual ~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		virtual void Render() = 0;

	protected:
		SDL_Window* m_pWindow;
		uint32_t m_Width;
		uint32_t m_Height;
		RGBColor m_ClearColor{ 0.1f, 0.1f, 0.1f };
	};
}
#endif