#include "MC.hpp"

double Probability(double dE, bool sigma, params p) {
	// Boltzmann
	return exp(-dE / (p.kB * p.T));
}

void spin_flip(bool** &grid, int i, int j, int w, int h, params p) {
	bool sigma = grid[i  ][j  ];
	bool north = grid[(i+h-1)%h][j  ];
	bool east  = grid[i  ][(j+w+1)%w];
	bool south = grid[(i+h+1)%h][j  ];
	bool west  = grid[i  ][(j+w-1)%w];

	double S = bool2spin(north + east + south + west, 4);
	double dE = 2 * p.H * bool2spin(sigma) + 2 * p.J * S;
	if (dE < 0) {grid[i][j] = !grid[i][j]; std::cout << "Changed" << std::endl;}
	else {
		double r = static_cast<double> (rand()) / static_cast<double> (RAND_MAX);
		if (r < Probability(dE, sigma, p))
			{grid[i][j] = !grid[i][j]; std::cout << "Changed" << std::endl;}
	}
}
