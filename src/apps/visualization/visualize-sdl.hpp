// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef IPTSD_APPS_VISUALIZATION_VISUALIZE_SDL_HPP
#define IPTSD_APPS_VISUALIZATION_VISUALIZE_SDL_HPP

#include "visualize.hpp"

#include <SDL2/SDL.h>
#include <cairomm/cairomm.h>
#include <memory>

namespace iptsd::apps::visualization {

class VisualizeSDL : public Visualize {
private:
	SDL_Window *m_window = nullptr;
	SDL_Renderer *m_renderer = nullptr;

	SDL_Texture *m_rtex = nullptr;
	Cairo::RefPtr<Cairo::ImageSurface> m_tex {};

public:
	VisualizeSDL(const core::Config &config, const core::DeviceInfo &info,
		     std::optional<const ipts::Metadata> metadata)
		: Visualize(config, info, metadata)
	{
		SDL_Init(SDL_INIT_VIDEO);
	}

	void on_start() override
	{
		// Create an SDL window
		u32 flags = SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_ALLOW_HIGHDPI;
		SDL_CreateWindowAndRenderer(0, 0, flags, &m_window, &m_renderer);

		// Get the screen size.
		SDL_GetRendererOutputSize(m_renderer, &m_size.x(), &m_size.y());

		// Create a texture that will be rendered later
		m_rtex = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888,
					   SDL_TEXTUREACCESS_STREAMING, m_size.x(), m_size.y());

		// Create a texture for drawing.
		m_tex = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, m_size.x(), m_size.y());

		// Create context for issuing draw commands.
		m_cairo = Cairo::Context::create(m_tex);
	}

	void on_data(const gsl::span<u8> &data) override
	{
		Visualize::on_data(data);

		this->draw();

		void *pixels = nullptr;
		int pitch = 0;

		// Copy drawtex to rendertex
		SDL_LockTexture(m_rtex, nullptr, &pixels, &pitch);
		std::memcpy(pixels, m_tex->get_data(), m_size.prod() * 4L);
		SDL_UnlockTexture(m_rtex);

		// Display rendertex
		SDL_RenderClear(m_renderer);
		SDL_RenderCopy(m_renderer, m_rtex, nullptr, nullptr);
		SDL_RenderPresent(m_renderer);
	}

	void on_stop() override
	{
		SDL_DestroyTexture(m_rtex);
		SDL_DestroyRenderer(m_renderer);
		SDL_DestroyWindow(m_window);

		SDL_Quit();
	}
};

} // namespace iptsd::apps::visualization

#endif // IPTSD_APPS_VISUALIZATION_VISUALIZE_SDL_HPP
