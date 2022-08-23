#include "MC.hpp"

/**
 * @brief Seed value for generation of random values in range [0, 1).
 */
const int pSEED = 20;
/**
 * @brief Uniform probability distribution to sample from for accept-reject purposes.
 */
static std::uniform_real_distribution<double> pDist(0, 1);
/**
 * @brief Random number generator which generates values in range [0, 1).
 */
static std::default_random_engine pRNG(pSEED);

/**
 * @brief Uniform distribution to sample from for selecting random indices in
 * lattice.
 */
static std::uniform_int_distribution<int> iDist;
/**
 * @brief Random number generator which generates random index value.
 * 
 * Note: The generator assumes a row major storage of lattice data. The row
 * and column indices can be calculated from a single value returned by this
 * generator.
 * 
 */
static std::default_random_engine iRNG;

/**
 * @brief Keep track of last width of lattice. This guards accidental
 * re-initialisation (with same seed value).
 */
static int _last_w = -1;
/**
 * @brief Keep track of last height of lattice. This guards accidental
 * re-initialisation (with same seed value).
 */
static int _last_h = -1;

/**
 * @brief Seed the random number generator for the indices.
 * 
 * @param w The width of the lattice to index.
 * @param h The height of the lattice to index.
 */
void init_iRNG(int w, int h) {
	iDist = std::uniform_int_distribution<int>(0, w*h - 1);
	iRNG = std::default_random_engine(w * h);
	_last_w = w;
	_last_h = h;
}

/**
 * @brief A random number in range [0, 1) for probability purposes.
 * 
 * @return double 
 */
double rProbability() {
	return pDist(pRNG);
}

/**
 * @brief A random integer in range `[0, w*h)` for indexing purposes.
 * 
 * @param w The width of the lattice to index.
 * @param h The height of the lattice to index.
 * @return int 
 */
int rIndex(int w, int h) {
	if (w != _last_w || h != _last_h)
		init_iRNG(w, h);
	return iDist(iRNG);
}

/**
 * @brief Generate and return random index values for 2D lattice.
 * 
 * @param w The width of the lattice to index.
 * @param h The height of the lattice to index.
 * @param i The row index.
 * @param j The column index.
 */
void getRandomIndices(int w, int h, int* i, int* j) {
	int idx = rIndex(w, h);
	idx1to2(idx, w, (uint*) i, (uint*) j);
}

/**
 * @brief The Boltzmann probability distribution.
 * 
 * @param dE The energy change.
 * @param BETA 1/(kT)
 * @return double 
 */
double pBoltz(double dE, double BETA) {
	return exp(-dE * BETA);
}

/**
 * @brief The Glauber probability distribution.
 * 
 * @param dE The energy change.
 * @param BETA 1/(kT)
 * @return double 
 */
double pGlauber(double dE, double BETA) {
	return 0.5 * (1 - tanh(BETA * dE));
}

/**
 * @brief The Suzuki-Kubo probability distribution.
 * 
 * @param dE The energy change.
 * @param BETA 1/(kT)
 * @return double 
 */
double pSuKu(double dE, double BETA) {
	return 0.5 * (1 - tanh(0.5 * BETA * dE));
}

/**
 * @brief The probability value for accept-reject algorithm. The type of
 * probability distribution to choose from is read from the system settings.
 * 
 * @param dE The energy change.
 * @param BETA 1/(kT)
 * @param ctx The system settings.
 * @return double 
 */
double Probability(double dE, const double BETA, Context *ctx) {
	switch(ctx->Transition) {
	case TransProb::BOLTZMANN:
		return pBoltz(dE, BETA);
	case TransProb::GLAUBER:
		return pGlauber(dE, BETA);
	case TransProb::SUZU_KUBO:
		return pSuKu(dE, BETA);
	}
}

/**
 * @brief Performs the accept-reject algorithm.
 * 
 * @param dE The energy change.
 * @param temperature The temperature.
 * @param ctx The system settings.
 * @return `true` if accepted, `false` if rejected.
 */
bool acceptance(double dE, double temperature, Context *ctx) {
	if (dE < 0) return true;
	double BETA = 1 / (ctx->BoltzConstant * temperature);
	double prob = Probability(dE, BETA, ctx);
	double r = rProbability();
	return r < prob;
}

/**
 * @brief Perform the dynamics of the system according to the spin-flip and
 * spin-exchange algorithm. The type of dynamics to perform (flip or exchange)
 * is read from the system settings.
 * 
 * @param config The Ising model configuration.
 * @param ctx The system settings.
 */
void dynamics(Ising* config, Context* ctx) {
	int ri, rj;
	getRandomIndices(config->getWidth(), config->getHeight(), &ri, &rj);

	switch(ctx->SpinKinetics) {
	case Dynamics::FLIP:
		spin_flip(config, ri, rj, ctx);
		break;
	case Dynamics::EXCHANGE:
		// choose the pair to exchange with
		// north, south, east, west all have equal chance of being picked
		int pairI, pairJ;
		double r = rProbability();
		if (r < 0.25) {pairI = ri-1; pairJ = rj  ;} else
		if (r < 0.50) {pairI = ri  ; pairJ = rj+1;} else
		if (r < 0.75) {pairI = ri+1; pairJ = rj  ;} else
		              {pairI = ri  ; pairJ = rj-1;}

		int i1 = std::min(ri, pairI);
		int j1 = std::min(rj, pairJ);
		int i2 = std::max(ri, pairI);
		int j2 = std::max(rj, pairJ);
		spin_exchange(config, i1, j1, i2, j2, ctx);
		break;
	}
}

/**
 * @brief Perform the spin-flip dynamics algorithm.
 * 
 * @param config The Ising model configuration.
 * @param i The random row index to check.
 * @param j The random column index to check.
 * @param specs The system settings.
 */
void spin_flip(Ising* config, int i, int j, Context* ctx) {
	Ising c = *config;
	bool sigma = c(i  , j  ); // We check if this spin should be flip
	bool north = c(i-1, j  );
	bool east  = c(i  , j+1);
	bool south = c(i+1, j  );
	bool west  = c(i  , j-1);

	double S = bool2spin(north + east + south + west, 4);
	double s = bool2spin(sigma);

	// The change in energy, if the spin is flipped
	double dE = 2 * Ising::getField() * s + 2 * Ising::getNNCoup() * s * S;
	if (acceptance(dE, c.getTemp(), ctx))
		c.flip(i, j);
}

/**
 * @brief Perform the spin-exchange dynamics algorithm.
 * The function assumes `i1 <= i2` and `j1 <= j2`.
 * 
 * @param config The Ising model configuration.
 * @param i1 The random row index of spin to exchange.
 * @param j1 The random column index of spin to exchange.
 * @param i2 The random row index of spin to exchange with.
 * @param j2 The random column index of spin to exchange with.
 */
void spin_exchange(Ising *config, int i1, int j1, int i2, int j2, Context* ctx) {
	Ising c = *config;
	uint S1, S2;
	uint sig1 = c(i1, j1);
	uint sig2 = c(i2, j2);
	if (sig1 == sig2) // the spins are the same, no need to do anything
		return;

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
	} else {
		std::cout << "Unexpected order of indices." << std::endl;
		return;
	}
	int dSig = sig1? S1 - S2 : S2 - S1;
	double dE = Ising::getNNCoup() * dSig * 4.;
	if (acceptance(dE, c.getTemp(), ctx))
		c.exchange(i1, j1, i2, j2);
}
