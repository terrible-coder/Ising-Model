#include <iostream>
#include <fstream>
#include <string>
#include <exception>

#include "defaults.hpp"
#include "io/save.hpp"
#include "io/prepare.hpp"
#include "MC.hpp"

static Context CTX;

int main(int argc, char** argv) {
	std::string filename;
	if (argc > 1) filename = argv[1];
	else          filename = INPUT_FILE;

	std::cout << "Initialising system..." << std::endl;
	init_system(filename, &CTX);
	const std::string parentDir = prepExperiment(&CTX);
	if (parentDir == "")
		return EXIT_FAILURE;

	std::cout << parentDir << std::endl;
	const int BIN = CTX.Lx * CTX.Ly;

	std::cout << "Setting global values..." << std::endl;
	Ising::setCoupling(CTX.Coupling);
	Ising::setField(CTX.Field);

	for (int tp = 0; tp < CTX._t_points; tp++) {
		double T = CTX.Temperature[tp];
		// new temperature

		std::cout << SEPARATOR;

		Ising config(CTX.Lx, CTX.Ly, T, BoundaryCondition::PERIODIC);
		config.generate();

		for (int ensemble = 0; ensemble < CTX.ENSEMBLE_SIZE; ensemble++) {
			std::cout << "Ensemble member " << ensemble+1 << "\n";
			config.reinit();
			open(&config, ensemble+1, parentDir);

			for (int k = 0; k < RUN; k++) {
				for (int i = 0; i < BIN; i++)
					dynamics(&config, &CTX);
				snap(&config);
			}
			// next ensemble
			close();
		}
	}

	return EXIT_SUCCESS;
}
