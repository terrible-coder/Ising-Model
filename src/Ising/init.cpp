#include "Ising.hpp"

uint trimTrailingZeros(uint* n) {
	uint k = 6; // This is hard coded for now, needs to be changed later
	while (*n & 1 == 0) {
		*n >>= 1;
		k--;
	}
	return k;
}

/**
 * @brief Generate a random number (bit string). The probability of a bit being
 * `1` is given by p/2^(n).
 * 
 * This is the finite digit algorithm as described in the paper by
 * Watanabe et. al., J. Phys. Soc. Jpn., August 15, 2019.
 * 
 * @param p 
 * @param n 
 * @param dist The distribution to choose random numbers from.
 * @param rng The random number generator engine.
 * @return uint 
 */
uint randIntP(uint p, uint n, std::uniform_int_distribution<uWord_t>& dist, std::default_random_engine& rng) {
	uWord_t yk = dist(rng);
	uWord_t xk;
	for (uint j = 1; j < n; j++) {
		p = p >> 1;
		xk = dist(rng);
		if (p & 1 == 1)
			yk = yk | xk;
		else
		 yk = yk & xk;
	}
	return yk;
}

Ising::Ising(uint w, uint h, uint conc,
						 double temperature,
						 BoundaryCondition b) {
	this->Lx = w;
	this->Ly = h;
	this->N  = w * h;
	this->conc = conc;
	this->rawX = w / WORD_SIZE;
	this->rawY = h;
	this->rawN = this->N / WORD_SIZE;
	this->T  = temperature;
	this->boundary = b;
	this->is_generated = false;

	std::cout << "Temperature of config: " << this->T << std::endl;
}

Ising::~Ising() {
	delete this->lattice;
	delete this->initial;
}

/**
 * @brief Generates a lattice of spins of given dimensions. The spins are
 * represented as boolean values (`true` for up and `false` for down).
 * This is the initial configuration of the model. That is, all ensemble
 * members will start from this configuration.
 * 
 * Note: This function should not be called more than once.
 */
void Ising::generate() {
	// Allow generation of initial configuration only once.
	if (this->is_generated) {
		std::cout << "Lattice already generated. Cannot be generated more than once." << std::endl;
		return;
	}
	this->is_generated = true;

	this->initial = new uWord_t[this->rawN];

	// Tie the seed value to the size of the lattice. This ensures the same
	// initial lattice for every type of experiment.
	uint seed = this->N + (uint)(this->T * 1000. + 0.5);
	std::default_random_engine RNG(seed);
	std::uniform_int_distribution<uWord_t> dist(0, ~((uWord_t)0));

	uint p = this->conc;
	uint n = trimTrailingZeros(&p);

	for (uint i = 0; i < this->rawN; i++)
		this->initial[i] = randIntP(p, n, dist, RNG);

	this->lattice = new uWord_t[this->rawN];
}

/**
 * @brief Initialise the lattice to the initial configuration generated.
 */
void Ising::reinit() {
	if (!this->is_generated) {
		std::cout << "Initial state not generated yet. Try that first." << std::endl;
		return;
	}
	// copy initial data into lattice
	for (uint i = 0; i < this->rawN; i++)
		this->lattice[i] = this->initial[i];
}

