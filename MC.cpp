#include "MC.hpp"

double pBoltz(double dE, double BETA) {
	return exp(-dE * BETA);
}
double pGlauber(double dE, double BETA) {
	return 0.5 * (1 - tanh(BETA * dE));
}

double Probability(double dE, params p) {
	const double BETA = 1 / (p.kB * p.T);
	// Boltzmann
	if (p.probability == "boltzmann")
		return pBoltz(dE, BETA);
	// if (p.probability == "glauber")
	return pGlauber(dE, BETA);
}

bool acceptance(double dE, params p) {
	if (dE < 0) return true;
	double prob = Probability(dE, p);
	double r = static_cast<double> (rand()) / static_cast<double> (RAND_MAX);
	return r < prob;
}

void dynamics(Ising* config) {
	if (config->getParams()->kinetics == "flip") {
		int ri = rand() % config->getParams()->height;
		int rj = rand() % config->getParams()->width;
		spin_flip(config, ri, rj);
	} else if (config->getParams()->kinetics == "exchange") {
		int ri = rand() % config->getParams()->height;
		int rj = rand() % config->getParams()->width;
		int pairi, pairj;
		double r = static_cast<double> (rand()) / static_cast<double> (RAND_MAX);
		if (r < 0.25) {pairi = ri-1; pairj = rj;}
		else if (r < 0.50) {pairi = ri; pairj = rj+1;}
		else if (r < 0.75) {pairi = ri+1; pairj = rj;}
		else {pairi = ri; pairj = rj-1;}
		int i1 = (ri < pairi) ? ri : pairi;
		int j1 = (rj < pairj) ? rj : pairj;
		int i2 = (ri >= pairi) ? ri : pairi;
		int j2 = (rj >= pairj) ? rj : pairj;
		spin_exchange(config, i1, j1, i2, j2);
	}
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
	if (acceptance(dE, *c.getParams()))
		c.flip(i, j);
}

/**
 * @brief i1 <= i2 j1 <= j2
 * 
 * @param config 
 * @param i1 
 * @param j1 
 * @param i2 
 * @param j2 
 */
void spin_exchange(Ising *config, int i1, int j1, int i2, int j2) {
	double S1, S2;
	Ising c = *config;
	if (i1 == i2) {
		int s;
		s = c(i1-1, j1) + c(i1+1, j1) + c(i1, j1-1);
		S1 = bool2spin(s, 3);
		s = c(i2-1, j2) + c(i2+1, j2) + c(i2, j2+1);
		S2 = bool2spin(s, 3);
	} else if (j1 == j2) {
		int s;
		s = c(i1, j1-1) + c(i1, j1+1) + c(i1-1, j1);
		S1 = bool2spin(s, 3);
		s = c(i2, j2-1) + c(i2, j2+1) + c(i2+1, j2);
		S2 = bool2spin(s, 3);
	} else {std::cout << "Something went wrong." << std::endl; return;}
	double Si = bool2spin(c(i1, j1)) * S1 + bool2spin(c(i2, j2)) * S2;
	double Sf = bool2spin(c(i1, j1)) * S2 + bool2spin(c(i2, j2)) * S1;
	double dE = c.getParams()->J * (Sf - Si);
	if (acceptance(dE, *c.getParams()))
		c.exchange(i1, j1, i1, j2);
}
