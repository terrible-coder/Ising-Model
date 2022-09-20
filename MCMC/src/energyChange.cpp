#include "energyChange.hpp"

double flipChange(Ising& c, uint i, uint j) {
	static double neighbour2change[] = {
		2 * Ising::getField() - 8 * Ising::getNNCoup(),
		2 * Ising::getField() - 4 * Ising::getNNCoup(),
		2 * Ising::getField(),
		2 * Ising::getField() + 4 * Ising::getNNCoup(),
		2 * Ising::getField() + 8 * Ising::getNNCoup(),
	};

	double sigma = bool2spin(c(i, j));
	int S = c(i, j+1) + c(i, j-1) + c(i+1, j) + c(i-1, j);
	return sigma * neighbour2change[S];
}

double exchangeChange(Ising& c, int i1, int j1, int i2, int j2) {
	uint S1, S2;
	uint sig1 = c(i1, j1);
	uint sig2 = c(i2, j2);
	if (sig1 == sig2) return 0.;
	/*
		The spins marked by "x" are the ones which are to be exchanged. The change
		in energy is due to the spins marked in "*" only.
		The dotted lines help visualise which spins get coupled with whom in the
		change.
	*/

	if (i1 == i2) {		// The pair is in the same row
		/*
				.	*	:	*	.
				*	x	:	x	*
				.	*	:	*	.
		*/
		S1 = c(i1-1, j1) + c(i1+1, j1) + c(i1, j1-1);
		S2 = c(i2-1, j2) + c(i2+1, j2) + c(i2, j2+1);
		int dSig = sig1? S1 - S2 : S2 - S1;
		return Ising::getNNCoup() * dSig * 4.;
	} else if (j1 == j2) {		// The pair is in the same column
		/*
				.	*	.
				*	x	*
				......
				*	x	*
				.	*	.
		*/
		S1 = c(i1, j1-1) + c(i1, j1+1) + c(i1-1, j1);
		S2 = c(i2, j2-1) + c(i2, j2+1) + c(i2+1, j2);
		int dSig = sig1? S1 - S2 : S2 - S1;
		return Ising::getNNCoup() * dSig * 4.;
	}
	return 0.;
}

PotEx* createPotEx(Ising& config, int i1, int j1, int i2, int j2, Context* ctx) {
	const double BETA = 1 / (ctx->BoltzConstant * config.getTemp());
	PotEx* ptr = new PotEx;
	ptr->i1 = i1; ptr->j1 = j1;
	ptr->i2 = i2; ptr->j2 = j2;
	ptr->delE = exchangeChange(config, i1, j1, i2, j2);
	ptr->transition = Probability(ptr->delE, BETA, ctx);
	ptr->normP = 0.;
	return ptr;
}
