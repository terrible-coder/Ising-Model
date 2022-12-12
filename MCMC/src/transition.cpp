#include "transition.hpp"

float Boltzmann(float dE, float BETA) {
	return exp(-dE * BETA);
}

float Suzuki_Kubo(float dE, float BETA) {
	float p = Boltzmann(dE, BETA);
	return p / (1 + p);
}

float Probability(float dE, const float BETA, Context *ctx) {
	switch(ctx->Transition) {
	case TransProb::BOLTZMANN:
		return Boltzmann(dE, BETA);
	case TransProb::SUZU_KUBO:
		return Suzuki_Kubo(dE, BETA);
	}
}
