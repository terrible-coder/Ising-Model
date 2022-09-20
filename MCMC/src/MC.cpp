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
		// choose the pair to exchange with
		// int pairI, pairJ;
		// bool flag = true;
		// while (flag) {
		// 	bool sigma = config(ri  ,rj  );
		// 	if (config(ri-1, rj  ) ^ sigma) { pairI = ri-1; pairJ = rj  ; flag=false; } else
		// 	if (config(ri  , rj+1) ^ sigma) { pairI = ri  ; pairJ = rj+1; flag=false; } else
		// 	if (config(ri+1, rj  ) ^ sigma) { pairI = ri+1; pairJ = rj  ; flag=false; } else
		// 	if (config(ri  , rj-1) ^ sigma) { pairI = ri  ; pairJ = rj-1; flag=false; }
		// 	else
		// 		getRandomIndices(config.getWidth(), config.getHeight(), &ri, &rj);
		// }
		// int i1 = std::min(ri, pairI);
		// int j1 = std::min(rj, pairJ);
		// int i2 = std::max(ri, pairI);
		// int j2 = std::max(rj, pairJ);
		spin_exchange(config, ctx);
		break;
	}
}
