#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <bit>

#include "context.hpp"
#include "io/prepare.hpp"

void readNext(std::ifstream& file, uWord* grid, uIndx rawSize) {
	uWord number;
	for (uIndx i = 0u; i < rawSize; i += 1u) {
		file.read((char*) &number, sizeof(uWord));
		grid[i] = number;
	}
}

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cout << "No file to analyse." << std::endl;
		return EXIT_FAILURE;
	}
	std::string filename = argv[1];
	std::ifstream evolution(filename, std::ios::binary);
	vec3<uIndx> L;
	evolution.read((char*) &(L.x), sizeof(uIndx));
	evolution.read((char*) &(L.y), sizeof(uIndx));
	evolution.read((char*) &(L.z), sizeof(uIndx));

	uIndx z = 0u;
	if (argc < 3)
		std::cout << "Z index not provided. Defaulting to z=0 layer." << std::endl;
	else
		z = (uIndx) std::stoi(argv[2]);

	uIndx rawSize = (L.x / WORD_SIZE) * L.y * L.z;
	uWord* lattice = new uWord[rawSize];
	uWord number;

	Context ctx;
	init_system("input.txt", &ctx);
	std::cout << "Runs: " << ctx.Run << std::endl;
	uIndx rawX = ctx.size.x / WORD_SIZE;
	std::vector<float> M;
	std::ofstream order("order.csv");

	std::cout << M.size() << std::endl;

	// y_end
	for (uint r = 0; r < ctx.Run; r++) {
		M.push_back(0);
		readNext(evolution, lattice, rawSize);
		for (uIndx i = rawSize - rawX; i < rawSize; i += 1u)
			M[r] += std::__popcount(lattice[i]);		// order 
	}
	for (uint r = 0; r < ctx.Run; r++)
		order << M[r] << ",";
	order << std::endl;

	// y_beg
	for (uint r = 0; r < ctx.Run; r++) {
		M.push_back(0);
		readNext(evolution, lattice, rawSize);
		for (uIndx i = 0; i < rawX; i += 1u)
			M[r] += std::__popcount(lattice[i]);		// order 
	}
	for (uint r = 0; r < ctx.Run; r++)
		order << M[r] << ",";
	order << std::endl;

	order.close();
	evolution.close();

	pos i;
}

