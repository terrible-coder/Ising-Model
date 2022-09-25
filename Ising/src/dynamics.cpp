#include "Ising.hpp"

void Ising::flip(uint i, uint j) {
	uint idx = idx2to1(i, j, this->Lx);
	flipBit(&(this->lattice[idx / WORD_SIZE]), idx % WORD_SIZE);
}

void Ising::exchange(uint i1, uint j1, uint i2, uint j2) {
	int i1a = i1, j1a = j1;
	int i2a = i2, j2a = j2;
	imposeBC(this->Lx, this->Ly, i1, j1, &i1a, &j1a, this->boundary);
	imposeBC(this->Lx, this->Ly, i2, j2, &i2a, &j2a, this->boundary);

	uint idx1 = idx2to1(i1a, j1a, this->Lx); // row major index of spin 1
	uint idx2 = idx2to1(i2a, j2a, this->Lx); // row major index of spin 2
	uWord_t* n1 = &(this->lattice[idx1 / WORD_SIZE]); // the word where spin 1 is stored
	uWord_t* n2 = &(this->lattice[idx2 / WORD_SIZE]); // the word where spin 2 is stored
	flipBit(n1, idx1 % WORD_SIZE); // this function gets called only if the
	flipBit(n2, idx2 % WORD_SIZE); // two spins are not the same
}

double Ising::Hamiltonian() {
	double E = 0.;
	// Single spin terms
	E = -Ising::getField() * this->Magnetisation();

	// Two spin interaction terms
	uint SS = 0;
	uWord_t* shift = new uWord_t[this->rawN];

	/* Multiplication of spin values (+1 or -1) can be mapped directly to
	 * multiplication of boolean values.
	 * A_spin * B_spin \equiv A XNOR B
	 * The actual value can be achieved using the very useful bool2spin.
	 */
	this->__leftShift(shift); // east
	for (int i = 0; i < this->rawN; i++)
		SS += std::__popcount(~(this->lattice[i] ^ shift[i]));

	this->__downShift(shift); // north
	for (int i = 0; i < this->rawN; i++)
		SS += std::__popcount(~(this->lattice[i] ^ shift[i]));

	free(shift);

	E -= Ising::getNNCoup() * bool2spin(SS, 2*this->N);
	return E;
}

double Ising::Magnetisation() {
	uint M = 0;
	for (std::uint16_t i = 0; i < this->rawN; i++)
		M += std::__popcount(this->lattice[i]);
	return bool2spin(M, this->N);
}
