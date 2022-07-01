#include "Ising.hpp"

params P;

void initConfig(params* p) {
	P = *p;
}
void initConfig(std::string file) {
	P = readInput(file);
}

/**
 * @brief Converts boolean representation of spin (`true` for up `false` for down)
 * to the spin representation in Ising model.
 * 
 * @param s The spin value: `true` for up and `false` for down.
 * @return double `+1.0` for up and `-1.0` for down.
 */
double bool2spin(bool s) {
	return 2.*s - 1.;
}
/**
 * @brief Converts sum of spins in boolean representation to integer representation.
 * 
 * @param S The sum of booleans.
 * @param n Number of spins that have been summed.
 * @return double 
 */
double bool2spin(int S, int n) {
	return 2.*S - n;
}

/**
 * @brief Generates a lattice of spins of given dimensions. The spins are
 * represented as boolean values (`true` for up and `false` for down).
 * 
 * @param w The width of the lattice of spins.
 * @param h The height of the lattice of spins.
 * @return bool** 
 */
bool** generate(int w, int h) {
	bool** grid = new bool*[h];
	for (int i = 0; i < h; i++)
		grid[i] = (bool*) malloc(w * sizeof(bool));

	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			grid[i][j] = rand() % 2 == 0;
	return grid;
}

/**
 * @brief Prints the lattice in text format into the console.
 * 
 * @param grid The lattice of spins.
 * @param w The width of the lattice.
 * @param h The height of the lattice.
 */
void printLattice(bool** grid, int w, int h) {
	for (int i = 0; i < h; i++) {
		for(int j = 0; j < w; j++)
			if (grid[i][j])	std::cout << "+" << "\t";
			else						std::cout << "-" << "\t";
		std::cout << std::endl;
	}
}

/**
 * @brief Computes the Ising hamiltonian for a given configuration. 
 * 
 * @param grid The 2D spin configuration.
 * @param w The width of the spin lattice.
 * @param h The height of the spin lattice.
 * @return float 
 */
double Hamiltonian(const bool** grid, int w, int h) {
	double E = 0.;
	int SS = 0;
	// Single spin terms
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			SS += grid[i][j];
	E = - P.H * bool2spin(SS, w*h);

	// Two spin interaction terms
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++) {
			bool north = grid[i-1][j  ];
			bool east  = grid[i  ][j+1];
			// bool south = grid[i+1][j  ];
			// bool west  = grid[i  ][j-1];
			SS = bool2spin(north + east, 2);
			E -= P.J * SS * bool2spin(grid[i][j]);
		}
	return E;
}

/**
 * @brief Calculate the magnetisation of the configuration.
 * 
 * @param grid The lattice of spins.
 * @param w The width of the lattice.
 * @param h The height of the lattice.
 * @return double 
 */
double Magnetisation(const bool** grid, int w, int h) {
	int M = 0;
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			M += grid[i][j];
	return bool2spin(M, w*h);
}
