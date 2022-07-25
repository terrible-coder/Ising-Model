#include "Ising.hpp"

Ising::Ising(int w, int h,
						 double temperature,
						 BoundaryCondition b) {
	this->Lx = w;
	this->Ly = h;
	this->N  = w * h;
	this->T  = temperature;
	this->boundary = b;
	this->is_generated = false;

	std::cout << "Temperature of config: " << this->T << std::endl;
}

Ising::~Ising() {
	// free(this->lattice);
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

	this->initial = new bool*[this->Ly];
	for (int i = 0; i < this->Ly; i++)
		this->initial[i] = (bool*) malloc(this->Lx * sizeof(bool));

	// Tie the seed value to the size of the lattice. This ensures the same
	// initial lattice for every type of experiment.
	int seed = this->N + (int)(this->T * 1000. + 0.5);
	std::default_random_engine RNG(seed);
	std::uniform_int_distribution<int> dist(0, 1);

	for (int i = 0; i < this->Ly; i++)
		for (int j = 0; j < this->Lx; j++)
			this->initial[i][j] = (bool) dist(RNG);

	this->lattice = new bool*[this->Ly];
	for (int i = 0; i < this->Ly; i++)
		this->lattice[i] = (bool*) malloc(this->Lx * sizeof(bool));
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
	for (int i = 0; i < this->Ly; i++)
		for (int j = 0; j < this->Lx; j++)
			this->lattice[i][j] = this->initial[i][j];
}
