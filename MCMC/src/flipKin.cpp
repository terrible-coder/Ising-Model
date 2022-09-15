#include "monte_carlo.hpp"

void spin_flip(Ising& c, int i, int j, Context* ctx) {
	// bool sigma = c(i  , j  ); // We check if this spin should be flip
	// bool north = c(i-1, j  );
	// bool east  = c(i  , j+1);
	// bool south = c(i+1, j  );
	// bool west  = c(i  , j-1);

	// double S = bool2spin(north + east + south + west, 4);
	// double s = bool2spin(sigma);

	// The change in energy, if the spin is flipped
	double dE = singleFlipChange(c, i, j); // 2 * Ising::getField() * s + 2 * Ising::getNNCoup() * s * S;
	if (isAccepted(dE, c.getTemp(), ctx))
		c.flip(i, j);
}


