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
	vec3<uIndx> L = c.getVecSize();
	uIndx q, idx;
	const uIndx BAIL = c.getQ() << 2;
	uIndx iter = 0u;
	while (true) {
		iter += 1u;
		getRandomIndices(L, &i);
		std::vector<vec3<int>> neighbours;
		c.getNeighbours(i, &neighbours);
		q = neighbours.size();
		idx = rIndex(q);
		j = c.equiv(neighbours[idx]);

		if (c(i) != c(j)) {
	// std::cout << "\tpos i:" << i.x << "," << i.y << "," << i.z << "\t";
	// std::cout << "\tpos j:" << j.x << "," << j.y << "," << j.z << "\t";
	// std::cout << c(i) << " " << c(j) << "\t";
			dE = c.exchangeEnergyChange(i, j);
			if (isAccepted(dE, c.getTemp(), ctx)) break;
		}
		if (iter == BAIL) return;
	// std::cout << c(i) << " " << c(j) << "\t";
		// if (c(i) != c(j)) break;
	}
	c.exchange(i, j);
	// std::cout << " nn:" << q << " ";
	// std::cout << "\t" << idx << std::endl;
	// c.exchange(i, j);
}
