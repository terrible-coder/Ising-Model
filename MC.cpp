#include "MC.hpp"

double Probability(double dE, bool sigma, params p) {
	const double BETA = 1 / (p.kB * p.T);
	// Boltzmann
	return exp(-dE * BETA);
}

void spin_flip(Ising* config, int i, int j) {
	Ising c = *config;
	bool sigma = c(i  , j  ); // grid[i  ][j  ];
	bool north = c(i-1, j  ); // grid[(i+h-1)%h][j  ];
	bool east  = c(i  , j+1); // grid[i  ][(j+w+1)%w];
	bool south = c(i+1, j  ); // grid[(i+h+1)%h][j  ];
	bool west  = c(i  , j-1); // grid[i  ][(j+w-1)%w];

	double S = bool2spin(north + east + south + west, 4);
	double s = bool2spin(sigma);

	double dE = 2 * c.getField() * s + 2 * c.getNNCoup() * s  * S;
	if (dE < 0)
		c.flip(i, j);
	else {
		double r = static_cast<double> (rand()) / static_cast<double> (RAND_MAX);
		if (r < Probability(dE, sigma, *c.getParams()))
			c.flip(i, j);
	}
}
