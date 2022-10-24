#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <exception>

#include "defaults.hpp"
#include "io/data_logger.hpp"
#include "io/save.hpp"
#include "io/prepare.hpp"
#include "monte_carlo.hpp"

static Context CTX;

/**
 * @brief Print the progress of a process. The function has been taken from
 * https://gist.github.com/juliusikkala/946f505656ed3c35f6c2741f29f26080
 * 
 * @param p 
 * @param total 
 * @param width 
 */
void print_progress(int p, int total, int width = 80) {
	std::string total_str = std::to_string(total);
	std::string p_str = std::to_string(p);
	int bar_width = width - total_str.size()*2 - 4;

	std::cout << '\r';
	if(bar_width > 1) {
		int fill_width = bar_width * p / total;
		std::cout << "[";
		for(int i = 0; i < bar_width; ++i) {
			char c = ' ';
			if(i < fill_width || p == total) c = '=';
			else if(i == fill_width) c = '>';

			std::cout << c;
		}
		std::cout << "] ";
	}
	std::cout << std::setfill(' ') << std::setw(total_str.size())
		<< "En " << p_str << "/" << total_str << std::flush;
	if(p == total) std::cout << std::endl;
}

int main(int argc, char** argv) {
	std::string filename;
	if (argc > 1) filename = argv[1];
	else          filename = INPUT_FILE;

	std::cout << "Initialising system..." << std::endl;
	// init_system(filename, &CTX);
	CTX.size = { 64u, 64u, 1u };
	const std::string parentDir = prepExperiment(&CTX);
	if (parentDir == "") return EXIT_FAILURE;

	std::cout << parentDir << std::endl;
	const uint BIN = CTX.size.x * CTX.size.y * CTX.size.z;

	std::cout << "Setting global values..." << std::endl;
	Ising::setCoupling(CTX.Coupling);
	Ising::setField(CTX.Field);

	for (auto tp = CTX.Temperature.begin(); tp != CTX.Temperature.end(); tp++) {
		double T = *tp;
		// new temperature
		openLogger(parentDir, T);
		std::cout << SEPARATOR;

		Ising* config = new Ising(
			CTX.size, CTX.Concentration,
			T, CTX.boundary);
		config->generate();
		saveInit(config, parentDir);

		for (int ensemble = 0; ensemble < CTX.Ensemble_Size; ensemble++) {
			// std::cout << "Ensemble member " << ensemble+1 << "\n";
			print_progress(ensemble+1, CTX.Ensemble_Size);
			config->reinit();
			open(config, ensemble+1, parentDir);

			for (int k = 0; k < RUN; k++) {
				for (int i = 0; i < BIN; i++)
					dynamics(*config, &CTX);
				logData(config->Hamiltonian() / config->getSize(),
								config->Magnetisation() / config->getSize());
				if (!snap(config))
					return EXIT_FAILURE;
			}
			// next ensemble
			nextEnsemble();
			close();
		}
		closeLogger();
		delete config;
	}

	return EXIT_SUCCESS;
}
