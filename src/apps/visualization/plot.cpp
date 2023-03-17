// SPDX-License-Identifier: GPL-2.0-or-later

#include "visualize-png.hpp"

#include <common/signal.hpp>
#include <common/types.hpp>
#include <core/linux/file-runner.hpp>

#include <CLI/CLI.hpp>
#include <algorithm>
#include <gsl/gsl>
#include <spdlog/spdlog.h>

namespace iptsd::apps::visualization::plot {

static int main(gsl::span<char *> args)
{
	CLI::App app {};

	std::filesystem::path path;
	std::filesystem::path output;

	app.add_option("DATA", path, "The binary data file containing the data to plot.")
		->type_name("FILE")
		->required();

	app.add_option("OUTPUT", output, "The output directory containg plotted frames.")
		->type_name("DIR")
		->required();

	CLI11_PARSE(app, args.size(), args.data());

	// Create a plotting application that reads from a file.
	core::linux::FileRunner<VisualizePNG> visualize {path, output};

	auto const _sigterm = common::signal<SIGTERM>([&](int) { visualize.stop(); });
	auto const _sigint = common::signal<SIGINT>([&](int) { visualize.stop(); });

	if (!visualize.run())
		return EXIT_FAILURE;

	return 0;
}

} // namespace iptsd::apps::visualization::plot

int main(int argc, char **argv)
{
	spdlog::set_pattern("[%X.%e] [%^%l%$] %v");
	gsl::span<char *> args {argv, gsl::narrow<usize>(argc)};

	try {
		return iptsd::apps::visualization::plot::main(args);
	} catch (std::exception &e) {
		spdlog::error(e.what());
		return EXIT_FAILURE;
	}
}
