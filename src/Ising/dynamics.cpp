#include "Ising.hpp"

/**
 * @brief Flip the spin at the given index.
 * 
 * @param i The row index.
 * @param j The column index.
 */
void Ising::flip(uint i, uint j) {
	uint idx = idx2to1(i, j, this->Lx);
	flipBit(&(this->lattice[idx / WORD_SIZE]), idx % WORD_SIZE);
}

/**
 * @brief Exchange the spins at the given indices.
 * 
 * @param i1 Row index 1.
 * @param j1 Column index 1.
 * @param i2 Row index 2.
 * @param j2 Column index 2.
 */
void Ising::exchange(uint i1, uint j1, uint i2, uint j2) {
	int i1a, j1a;
	int i2a, j2a;
	imposeBC(this->Lx, this->Ly, i1, j1, &i1a, &j1a, this->boundary);
	imposeBC(this->Lx, this->Ly, i2, j2, &i2a, &j2a, this->boundary);

	uint idx1 = idx2to1(i1a, j1a, this->Lx); // row major index of spin 1
	uint idx2 = idx2to1(i1a, j1a, this->Lx); // row major index of spin 2
	uWord_t* n1 = &(this->lattice[idx1 / WORD_SIZE]); // the word where spin 1 is stored
	uWord_t* n2 = &(this->lattice[idx2 / WORD_SIZE]); // the word where spin 2 is stored
	bool b1 = bitFromBeg(*n1, idx1 % WORD_SIZE); // spin 1
	bool b2 = bitFromBeg(*n2, idx2 % WORD_SIZE); // spin 2
	if (b1 == b2) return; // spins are same, no need to do anything
	flipBit(n1, idx1 % WORD_SIZE);
	flipBit(n2, idx2 % WORD_SIZE);
}

/**
 * @brief Computes the Ising hamiltonian for a given configuration.
 * 
 * @return double 
 */
double Ising::Hamiltonian() {
	double E = 0.;
	// Single spin terms
	E = -Ising::getField() * this->Magnetisation();

	// Two spin interaction terms
	uint SS = 0;
	for (int i = 0; i < this->Ly; i++)
		for (int j = 0; j < this->Lx; j++) {
			bool sigma = (*this)(i  , j  );
			bool north = (*this)(i-1, j  );
			bool east  = (*this)(i  , j+1);

			SS += (sigma == north) + (sigma == east);
		}
	E -= Ising::getNNCoup() * bool2spin(SS, 2*this->N);
	return E;
}

/**
 * @brief Calculate the magnetisation of the configuration.
 * 
 * @return double 
 */
double Ising::Magnetisation() {
	uint M = 0;
	for (std::uint16_t i = 0; i < this->N / WORD_SIZE; i++)
		M += std::__popcount(this->lattice[i]);
	return bool2spin(M, this->N);
}
