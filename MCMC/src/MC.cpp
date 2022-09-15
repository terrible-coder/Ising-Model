#include "monte_carlo.hpp"

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

double Probability(double dE, const double BETA, Context *ctx) {
	switch(ctx->Transition) {
	case TransProb::BOLTZMANN:
		return Boltzmann(dE, BETA);
	case TransProb::SUZU_KUBO:
		return Suzuki_Kubo(dE, BETA);
	}
}

bool isAccepted(double dE, double temperature, Context *ctx) {
	if (dE < 0) return true;
	double BETA = 1 / (ctx->BoltzConstant * temperature);
	double prob = Probability(dE, BETA, ctx);
	double r = rProbability();
	return r < prob;
}

double singleFlipChange(Ising& c, int i, int j) {
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

void dynamics(Ising& config, Context* ctx) {
	int ri, rj;
	getRandomIndices(config.getWidth(), config.getHeight(), &ri, &rj);

	switch(ctx->SpinKinetics) {
	case Dynamics::FLIP:
		spin_flip(config, ri, rj, ctx);
		break;
	case Dynamics::EXCHANGE:
		// choose the pair to exchange with
		bool success = false;
		int pairI, pairJ;
		while (!success) {
			getRandomIndices(config.getWidth(), config.getHeight(), &pairI, &pairJ);
			// std::cout << "Attempt " << ri << ", " << rj << " ";
			// std::cout << "and " << pairI << ", " << pairJ;
			int i1 = std::min(ri, pairI);
			int j1 = std::min(rj, pairJ);
			int i2 = std::max(ri, pairI);
			int j2 = std::max(rj, pairJ);
			spin_exchange(config, i1, j1, i2, j2, ctx, &success);
			// if (success) {
			// 	std::cout << "Attempt " << ri << ", " << rj << " ";
			// 	std::cout << "and " << pairI << ", " << pairJ;
			// 	std::cout << "\taccepted\n";
			// }
			// else std::cout << std::endl;
		}
		break;
	}
}
