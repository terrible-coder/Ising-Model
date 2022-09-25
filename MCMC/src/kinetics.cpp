#include "monte_carlo.hpp"

/**
 * @brief Generate and return random index values for 2D lattice.
 * 
 * @param w The width of the lattice to index.
 * @param h The height of the lattice to index.
 * @param i The row index.
 * @param j The column index.
 */
void getRandomIndices(int w, int h, int* i, int* j) {
	int idx = rIndex(w, h);
	idx1to2(idx, w, (uint*) i, (uint*) j);
}

void spin_flip(Ising& c, Context* ctx) {
	int ri, rj;
	getRandomIndices(c.getWidth(), c.getHeight(), &ri, &rj);
	// The change in energy, if the spin is flipped
	double dE = flipChange(c, ri, rj);
	if (isAccepted(dE, c.getTemp(), ctx))
		c.flip(ri, rj);
}

double picking(double dE) {
	// return 0.01 * exp(-dE);
	// return 12 - dE + 1;
	// return 0.5 * exp(-dE / 4) + exp(-dE*dE / 16);
	// return (dE < 0) ? 0.5 * exp(-dE/4) + 0.5 : 1;
	return (dE < 0) ? 0.5 * exp(-dE / 4) + 1.5 : -0.125 * dE + 2;
}

/**
 * @brief List of possible exchanges arranged in decreasing order of -ve
 * energy change.
 */
static ExchangeList exchanges;
static double _last_temp = -99.;

void spin_exchange(Ising& c, Context* ctx) {
	if (_last_temp != c.getTemp()) {
		_last_temp = c.getTemp();
		exchanges = ExchangeList(picking);
		exchanges.createList(c, ctx);
	}

	PotEx ex;
	exchanges.normalise();
	do {
		ex = exchanges.pickEx(rProbability());
	} while(!isAccepted(ex.delE, c.getTemp(), ctx));
	c.exchange(ex.i1, ex.j1, ex.i2, ex.j2);
	int i1 = ex.i1, j1 = ex.j1;
	int i2 = ex.i2, j2 = ex.j2;
	exchanges.remove_if([i1, j1, i2, j2](const PotEx& done) {
		return (done.i1 == i1 && done.j1 == j1) ||
					 (done.i2 == i2 && done.j2 == j2) ||
					 (done.i1 == i2 && done.j1 == j2) ||
					 (done.i2 == i1 && done.j2 == j1);
	});
	exchanges.update(c, ex, ctx);
}
