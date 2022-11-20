#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <exception>
#include <mpi.h>

#include "defaults.hpp"
#include "io/data_logger.hpp"
#include "io/save.hpp"
#include "io/prepare.hpp"
#include "Ising.hpp"
#include "monte_carlo.hpp"


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

	Context CTX;
	std::cout << "Initialising system..." << std::endl;
	if (init_system(filename, &CTX))
		std::cout << "Parsing input file success." << std::endl;
	else {
		std::cout << "Could not parse input file." << std::endl;
		return EXIT_FAILURE;
	}
	if (!prepExperiment(&CTX))
		return EXIT_FAILURE;
	logOnly(CTX.DataLogs);

	std::cout << "Box size: " << CTX.size.x << "x" << CTX.size.y << "x" << CTX.size.z << "\n";
	std::cout << "Number of surfaces: " << CTX.surfLocs.size() << "\n";
	std::cout << "Number of temperature points: " << CTX.Temperature.size() << "\n";
	std::cout << "Temperature range: " << *(CTX.Temperature.begin()) << "..." << *(CTX.Temperature.end()-1) << "\n";
	std::cout << "Logging " << CTX.DataLogs.size() << "\n"; 
	std::cout << std::endl;

	ModelParams parameters(CTX.size, CTX.boundary);
	parameters.setInteractions(CTX.interact.x, CTX.interact.y, CTX.interact.z);
	if (CTX.surfLocs.size() != CTX.surfInts.size()) {
		std::cout << "Number of surfaces do not match ";
		std::cout << "number of surface interactions." << std::endl;
		return EXIT_FAILURE;
	}
	if (CTX.surfLocs.size() > 0) {
		Edge e;
		for (uint i = 0; i < CTX.surfInts.size(); i += 1u) {
			std::string loc = CTX.surfLocs[i];
			vec3<float> interact = CTX.surfInts[i];
			if (loc == "x_beg") e = Edge::X_BEG; else
			if (loc == "x_end") e = Edge::X_END; else
			if (loc == "y_beg") e = Edge::Y_BEG; else
			if (loc == "y_end") e = Edge::Y_END; else
			if (loc == "z_beg") e = Edge::Z_BEG; else
			if (loc == "z_end") e = Edge::Z_END;
			else {
				std::cout << "Edge location \"" << loc << "\" not recognised." << std::endl;
				return EXIT_FAILURE;
			}
			parameters.create_surface(e, interact.x, interact.y, interact.z);
		}
	}

	// Preparing MPI

	int rank, n_ranks, numbers_per_rank;
	int rEn_beg, rEn_end;
	int numbers = CTX.EnsembleSize;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
	numbers_per_rank = floor(numbers / n_ranks);
	if (numbers% n_ranks > 0)
		numbers_per_rank++;
	rEn_beg = rank * numbers_per_rank;
	rEn_end = rEn_beg + numbers_per_rank;

	float T = *(CTX.Temperature.begin()); // temperature
	Ising config(CTX.Concentration, parameters, T);
	config.generate(rank+1);

	openLogger(CTX.saveDir, T, rank+1);
	saveInit(config, CTX.saveDir);
	for (uIndx en = rEn_beg; en < rEn_end; en += 1u) {
		openSnap(config, en+1, CTX.saveDir);
		MonteCarlo(config, &CTX, en+1);
		closeSnap();
		nextEnsemble();
	}
	closeLogger();

	// config.reinit();
	// saveInit(config, CTX.saveDir);
	// pos i{0u, 0u, 0u};
	// uSize n = 0;
	// std::vector<vec3<int>> neighbours;
	// config.getNeighbours(i, &neighbours);
	// std::cout << "point:\t" << i.x << "," << i.y << "," << i.z << "\t" << config(i) << std::endl;
	// for (auto it = neighbours.begin(); it != neighbours.end(); it++) {
	// 	pos j = config.equiv(*it);
	// 	uSize m = 0;
	// 	std::cout << j.x << "," << j.y << "," << j.z << "\t";
	// 	std::cout << config(j) << "\t";
	// 	std::cout << "nn_sum, except i: ";
	// 	std::cout << config.sumNeighbours(j, {1u, 1u, 1u}, i, &m);
	// 	std::cout << "(" << m << ")" << "\t";
	// 	std::cout << config.exchangeEnergyChange(i, j);
	// 	std::cout << std::endl;
	// }
	// uIndx s = config.sumNeighbours(i, &n);
	// std::cout << "\n" << n << "\t" << s << std::endl; 

	return MPI_Finalize();
}
