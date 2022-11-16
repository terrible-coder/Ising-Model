#include "Ising.hpp"

Ising::Ising(uIndx conc, ModelParams& params, float temperature) {
	this->conc = conc;
	this->p = params;
	this->T = temperature;
	this->raw = {params.L.x, params.L.y, params.L.z};
	this->raw.x /= WORD_SIZE;
	this->rawN = params.N / WORD_SIZE;
	this->is_generated = false;

	this->__M = (float)params.N + 10.f;

	std::cout << "Temperature of config: " << this->T << std::endl;
}

Ising::~Ising() {
	delete this->lattice;
	delete this->initial;
}

void Ising::generate(int r) {
	// Allow generation of initial configuration only once.
	if (this->is_generated) {
		std::cout << "Lattice already generated. Cannot be generated more than once." << std::endl;
		return;
	}
	this->is_generated = true;

	this->initial = new uWord[this->rawN];

	// Tie the seed value to the size of the lattice. This ensures the same
	// initial lattice for every type of experiment.
	uint seed = this->p.N + (uint)(this->T * 1000. + 0.5) * r;

	for (uSize i = 0; i < this->rawN; i+=1u)
		this->initial[i] = randIntP(this->conc, seed);

	this->lattice = new uWord[this->rawN];
}

void Ising::reinit() {
	if (!this->is_generated) {
		std::cout << "Initial state not generated yet. Try that first." << std::endl;
		return;
	}
	// copy initial data into lattice
	for (uint i = 0; i < this->rawN; i++)
		this->lattice[i] = this->initial[i];
}
