#include "transition.hpp"

double Boltzmann(double dE, double BETA) {
	return exp(-dE * BETA);
}

double Suzuki_Kubo(double dE, double BETA) {
	double p = Boltzmann(dE, BETA);
	return p / (1 + p);
}

double Probability(double dE, const double BETA, Context *ctx) {
	switch(ctx->Transition) {
	case TransProb::BOLTZMANN:
		return Boltzmann(dE, BETA);
	case TransProb::SUZU_KUBO:
		return Suzuki_Kubo(dE, BETA);
	}
}
