#include "monte_carlo.hpp"

bool isAccepted(float dE, float temperature, Context *ctx) {
	if (dE < 0) return true;
	float BETA = 1 / (ctx->BoltzConstant * temperature);
	float prob = Probability(dE, BETA, ctx);
	float r = rProbability();
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

void MonteCarlo(Ising& c, Context* ctx, uIndx en) {
	uSize Bin = c.getSize();
	for (uint i = 0; i < ctx->Run; i += 1u) {
		for (uSize mc = 0; mc < Bin; mc += 1u) {
			std::cout << "run " << i << " " << mc << std::endl;
			dynamics(c, ctx);
		}
	}
}
