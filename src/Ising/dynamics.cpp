#include "Ising.hpp"

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
 * @brief Flip the spin at the given index.
 * 
 * @param i The row index.
 * @param j The column index.
 */
void Ising::flip(int i, int j) {
	this->lattice[i][j] = !this->lattice[i][j];
}

/**
 * @brief Exchange the spins at the given indices.
 * 
 * @param i1 Row index 1.
 * @param j1 Column index 1.
 * @param i2 Row index 2.
 * @param j2 Column index 2.
 */
void Ising::exchange(int i1, int j1, int i2, int j2) {
	int i1a, j1a;
	int i2a, j2a;
	this->BC(i1, j1, &i1a, &j1a);
	this->BC(i2, j2, &i2a, &j2a);

	bool temp = this->lattice[i1a][j1a];
	this->lattice[i1a][j1a] = this->lattice[i2a][j2a];
	this->lattice[i2a][j2a] = temp;
}

/**
 * @brief Computes the Ising hamiltonian for a given configuration.
 * 
 * @return double 
 */
double Ising::Hamiltonian() {
	int w = this->Lx, h = this->Ly;
	double E = 0.;
	int SS = 0;

	// Single spin terms
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			SS += this->lattice[i][j];
	E = -Ising::getField() * bool2spin(SS, this->N);

	// Two spin interaction terms
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++) {
			bool north = (*this)(i-1, j  );
			bool east  = (*this)(i  , j+1);

			SS = bool2spin(north + east, 2);
			E -= Ising::getNNCoup() * SS * bool2spin((*this)(i, j));
		}
	return E;
}

/**
 * @brief Calculate the magnetisation of the configuration.
 * 
 * @return double 
 */
double Ising::Magnetisation() {
	int M = 0;
	for (int i = 0; i < this->Ly; i++)
		for (int j = 0; j < this->Lx; j++)
			M += this->lattice[i][j];
	return bool2spin(M, this->N);
}
