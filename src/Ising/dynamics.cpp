#include "Ising.hpp"

/**
 * @brief Flip the spin at the given index.
 * 
 * @param i The row index.
 * @param j The column index.
 */
void Ising::flip(uint i, uint j) {
	uint idx = i * this->Lx + j;
	this->lattice[idx / WORD_SIZE] ^= ((uWord_t)1) << (WORD_SIZE - idx%WORD_SIZE - 1);
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

	uint idx1 = i1a * this->Lx + j1a; // row major index of spin 1
	uint idx2 = i2a * this->Lx + j2a; // row major index of spin 2
	uWord_t n1 = this->lattice[idx1 / WORD_SIZE]; // the word where spin 1 is stored
	uWord_t n2 = this->lattice[idx2 / WORD_SIZE]; // the word where spin 2 is stored
	uint a1 = WORD_SIZE - idx1%WORD_SIZE - 1; // shift length
	uint a2 = WORD_SIZE - idx2%WORD_SIZE - 1; // shift length
	uWord_t b1 = (n1 >> a1) & 1; // spin 1
	uWord_t b2 = (n2 >> a2) & 1; // spin 2
	// swap b1 and b2
	b1 = b1 ^ b2;
	b2 = b1 ^ b2;
	b1 = b1 ^ b2;
	// put them back in correct place
	b1 <<= a1;
	b2 <<= a2;
	n1 = (n1 & ~b1) | b1; // change the bit in n1
	n2 = (n2 & ~b2) | b2; // change the bit in n2

	this->lattice[idx1] = n1;
	this->lattice[idx2] = n2;
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
