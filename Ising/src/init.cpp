#include "Ising.hpp"

Ising::Ising(vec3<uIndx>& size, uIndx conc,
						 double temperature,
						 const vec3<BoundaryCondition>& b) {
	this->L = size;
	this->N = size.x * size.y * size.z;
	this->conc = conc;
	this->raw = size;
	this->raw.x /= WORD_SIZE;
	this->rawN = this->N / WORD_SIZE;
	this->T = temperature;
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

	this->initial = new uWord[this->rawN];

	// Tie the seed value to the size of the lattice. This ensures the same
	// initial lattice for every type of experiment.
	uint seed = this->N + (uint)(this->T * 1000. + 0.5);

	for (uSize i = 0; i < this->rawN; i+=1u)
		this->initial[i] = randIntP(this->conc, seed);

	this->lattice = new uWord[this->rawN];
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

