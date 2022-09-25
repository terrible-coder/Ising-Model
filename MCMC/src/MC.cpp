#include "monte_carlo.hpp"

bool isAccepted(double dE, double temperature, Context *ctx) {
	if (dE < 0) return true;
	double BETA = 1 / (ctx->BoltzConstant * temperature);
	double prob = Probability(dE, BETA, ctx);
	double r = rProbability();
	return r < prob;
}

void dynamics(Ising& config, Context* ctx) {
	switch(ctx->SpinKinetics) {
	case Dynamics::FLIP:
		spin_flip(config, ctx);
		break;
	case Dynamics::EXCHANGE:
		spin_exchange(config, ctx);
		break;
	}
}
