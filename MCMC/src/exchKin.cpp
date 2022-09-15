#include "monte_carlo.hpp"

void spin_exchange(Ising& c, int i1, int j1, int i2, int j2, Context* ctx, bool* success) {
	uint S1, S2;
	uint sig1 = c(i1, j1);
	uint sig2 = c(i2, j2);
	if (sig1 == sig2) { // the spins are the same, no need to do anything
		*success = false;
		return;
	}

	double dE;

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
		dE = Ising::getNNCoup() * dSig * 4.;
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
		dE = Ising::getNNCoup() * dSig * 4.;
	} else
		dE = singleFlipChange(c, i1, j1) + singleFlipChange(c, i2, j2);
	if (!isAccepted(dE, c.getTemp(), ctx)) {
		*success = false;
		return;
	}
	c.exchange(i1, j1, i2, j2);
	*success = true;
}

