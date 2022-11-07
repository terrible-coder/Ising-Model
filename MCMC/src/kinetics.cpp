#include "monte_carlo.hpp"

/**
 * @brief Generate and return random index values for 2D lattice.
 * 
 * @param w The width of the lattice to index.
 * @param h The height of the lattice to index.
 * @param i The row index.
 * @param j The column index.
 */
void getRandomIndices(vec3<uIndx> const& L, pos* i) {
	int idx = rIndex(L);
	idx1to3(idx, L, i);
}

void spin_flip(Ising& c, Context* ctx) {
	pos i;
	float dE;
	getRandomIndices(c.getVecSize(), &i);
	// The change in energy, if the spin is flipped
	dE = c.flipEnergyChange(i);
	if (!isAccepted(dE, c.getTemp(), ctx))
		c.flip(i);
}

float picking(float dE) {
	// return 0.01 * exp(-dE);
	// return 12 - dE + 1;
	// return 0.5 * exp(-dE / 4) + exp(-dE*dE / 16);
	// return (dE < 0) ? 0.5 * exp(-dE/4) + 0.5 : 1;
	return (dE < 0) ? 0.5 * exp(-dE / 4) + 1.5 : -0.125 * dE + 2;
}

void spin_exchange(Ising& c, Context* ctx) {
	pos i, j;
	float dE;
	do {
		getRandomIndices(c.getVecSize(), &i);
		getRandomIndices({1, 1, 1}, &j);
		j = j + i;
		if (c(i) == c(j)) continue;
		dE = c.exchangeEnergyChange(i, j);
	} while (!isAccepted(dE, c.getTemp(), ctx));
	c.exchange(i, j);
}
