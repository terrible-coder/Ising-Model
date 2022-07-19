#include "MC.hpp"

const int pSEED = 20;
static std::uniform_real_distribution<double> pDist(0, 1);
static std::default_random_engine pRNG(pSEED);

static std::uniform_int_distribution<int> iDist;
static std::default_random_engine iRNG;

static int _last_w = -1;
static int _last_h = -1;

void init_iRNG(int w, int h) {
	iDist = std::uniform_int_distribution<int>(0, w*h - 1);
	iRNG = std::default_random_engine(w * h);
	_last_w = w;
	_last_h = h;
}

double rProbability() {
	return pDist(pRNG);
}

int rIndex(int w, int h) {
	if (w != _last_w || h != _last_h)
		init_iRNG(w, h);
	return iDist(iRNG);
}

void getRandomIndices(int w, int h, int* i, int* j) {
	int idx = rIndex(w, h);
	*j = idx % w;
	*i = (idx - *j) / h;
}

double pBoltz(double dE, double BETA) {
	return exp(-dE * BETA);
}
double pGlauber(double dE, double BETA) {
	return 0.5 * (1 - tanh(BETA * dE));
}

double Probability(double dE, const double BETA, Specifications *S) {
	switch(S->Transition) {
	case TransProb::BOLTZMANN:
		return pBoltz(dE, BETA);
	case TransProb::GLAUBER:
		return pGlauber(dE, BETA);
	}
}

bool acceptance(double dE, double temperature, Specifications *S) {
	if (dE < 0) return true;
	double BETA = 1 / (S->BoltzConstant * temperature);
	double prob = Probability(dE, BETA, S);
	double r = rProbability();
	return r < prob;
}

void dynamics(Ising* config, Specifications* S) {
	int ri, rj;
	getRandomIndices(config->getWidth(), config->getHeight(), &ri, &rj);

	switch(S->SpinKinetics) {
	case Dynamics::FLIP:
		spin_flip(config, ri, rj, S);
		break;
	case Dynamics::EXCHANGE:
		int pairI, pairJ;
		double r = pDist(pRNG);
		if (r < 0.25) {pairI = ri-1; pairJ = rj  ;} else
		if (r < 0.50) {pairI = ri  ; pairJ = rj+1;} else
		if (r < 0.75) {pairI = ri+1; pairJ = rj  ;}
		else          {pairI = ri  ; pairJ = rj-1;}
		int i1 = (ri <  pairI) ? ri : pairI;
		int j1 = (rj <  pairJ) ? rj : pairJ;
		int i2 = (ri >= pairI) ? ri : pairI;
		int j2 = (rj >= pairJ) ? rj : pairJ;
		spin_exchange(config, i1, j1, i2, j2, S);
		break;
	}
}

void spin_flip(Ising* config, int i, int j, Specifications* specs) {
	Ising c = *config;
	bool sigma = c(i  , j  );
	bool north = c(i-1, j  );
	bool east  = c(i  , j+1);
	bool south = c(i+1, j  );
	bool west  = c(i  , j-1);

	double S = bool2spin(north + east + south + west, 4);
	double s = bool2spin(sigma);

	double dE = 2 * Ising::getField() * s + 2 * Ising::getNNCoup() * s  * S;
	if (acceptance(dE, c.getTemp(), specs))
		c.flip(i, j);
}

/**
 * @brief i1 <= i2 j1 <= j2
 * 
 * @param config 
 * @param i1 
 * @param j1 
 * @param i2 
 * @param j2 
 */
void spin_exchange(Ising *config, int i1, int j1, int i2, int j2, Specifications* specs) {
	double S1, S2;
	Ising c = *config;
	if (i1 == i2) {
		int s;
		s = c(i1-1, j1) + c(i1+1, j1) + c(i1, j1-1);
		S1 = bool2spin(s, 3);
		s = c(i2-1, j2) + c(i2+1, j2) + c(i2, j2+1);
		S2 = bool2spin(s, 3);
	} else if (j1 == j2) {
		int s;
		s = c(i1, j1-1) + c(i1, j1+1) + c(i1-1, j1);
		S1 = bool2spin(s, 3);
		s = c(i2, j2-1) + c(i2, j2+1) + c(i2+1, j2);
		S2 = bool2spin(s, 3);
	} else {std::cout << "Something went wrong." << std::endl; return;}
	double Si = bool2spin(c(i1, j1)) * S1 + bool2spin(c(i2, j2)) * S2;
	double Sf = bool2spin(c(i1, j1)) * S2 + bool2spin(c(i2, j2)) * S1;
	double dE = Ising::getNNCoup() * (Sf - Si);
	if (acceptance(dE, c.getTemp(), specs))
		c.exchange(i1, j1, i1, j2);
}
