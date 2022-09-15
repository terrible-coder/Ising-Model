#include "transition.hpp"

double Boltzmann(double dE, double BETA) {
	return exp(-dE * BETA);
}

double Suzuki_Kubo(double dE, double BETA) {
	double p = Boltzmann(dE, BETA);
	return p / (1 + p);
}
