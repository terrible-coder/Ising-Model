#include "monte_carlo.hpp"

/**
 * @brief Generate and return random index values for 2D lattice.
 * 
 * @param w The width of the lattice to index.
 * @param h The height of the lattice to index.
 * @param i The row index.
 * @param j The column index.
 */
void getRandomIndices(vec3<uIndx> const& L, pos* i, bool noInit) {
	int idx = rIndex(L, noInit);
	idx1to3(idx, L, i);
}

void spin_flip(Ising& c, Context* ctx) {
	pos i;
	float dE;
	getRandomIndices(c.getVecSize(), &i, false);
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
	vec3<uIndx> L = c.getVecSize();
	uIndx q;
	while (true) {
		getRandomIndices(L, &i, false);
		std::vector<vec3<int>> neighbours;
		c.getNeighbours(i, &neighbours);
		q = neighbours.size();
		getRandomIndices({q, 1u, 1u}, &j, true);
		j = toTVec<uIndx>(neighbours[j.x]);

		if (c(i) == c(j)) continue;
		dE = c.exchangeEnergyChange(i, j);
		if (isAccepted(dE, c.getTemp(), ctx)) break;
	}
	std::cout << " nn:" << q << " ";
	std::cout << "\tpos i:" << i.x << "," << i.y << "," << i.z << "\t";
	std::cout << "pos j:" << j.x << "," << j.y << "," << j.z << "\t";
	std::cout << c(i) << " " << c(j) << "\t";
	c.exchange(i, j);
	std::cout << c(i) << " " << c(j) << std::endl;
}
