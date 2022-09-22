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

PotEx createPotEx(Ising& config, int i1, int j1, int i2, int j2, Context* ctx) {
	const double BETA = 1 / (ctx->BoltzConstant * config.getTemp());
	PotEx ptr;
	config.equiv(&i1, &j1);
	config.equiv(&i2, &j2);
	ptr.i1 = i1; ptr.j1 = j1;
	ptr.i2 = i2; ptr.j2 = j2;
	ptr.delE = exchangeChange(config, i1, j1, i2, j2);
	ptr.transition = Probability(ptr.delE, BETA, ctx);
	ptr.normP = 0.;
	return ptr;
}

ExchangeList::ExchangeList() {}

ExchangeList::ExchangeList(std::function<double(double)> pick) {
	this->isCreated = false;
	double a = 4. * Ising::getNNCoup();
	for (int i = 0; i < 7; i++)
		this->pickingProb[i] = pick((i-3) * a);
}

void ExchangeList::push_back(const PotEx& exchange) {
	int dE; // in units of J
	if (exchange.delE > 0) dE = (int) (exchange.delE / Ising::getNNCoup() + 0.5);
	else dE = (int) (exchange.delE / Ising::getNNCoup() - 0.5);

	if (dE == -12) this->dEn12.push_back(exchange); else
	if (dE == - 8) this->dEn08.push_back(exchange); else
	if (dE == - 4) this->dEn04.push_back(exchange); else
	if (dE ==   0) this->dE000.push_back(exchange); else
	if (dE == + 4) this->dEp04.push_back(exchange); else
	if (dE == + 8) this->dEp08.push_back(exchange); else
//if (delE == +12)
		this->dEp12.push_back(exchange);
}

void ExchangeList::createList(Ising& config, Context* ctx) {
	if (this->isCreated) return;
	this->isCreated = true;
	for (uint i = 0; i < config.getHeight(); i++)
		for (uint j = 0; j < config.getWidth(); j++) {
			bool sigma = config(i, j);
			if (sigma != config(i-1, j  ))	// north
				this->push_back(createPotEx(config, i-1, j  , i  , j  , ctx));
			if (sigma != config(i  , j+1))	// east
				this->push_back(createPotEx(config, i  , j  , i  , j+1, ctx));
		}
}

void ExchangeList::remove_if(const std::function<bool(const PotEx&)>& f) {
	this->dEn12.remove_if(f); this->dEn08.remove_if(f); this->dEn04.remove_if(f);
	this->dE000.remove_if(f);
	this->dEp04.remove_if(f); this->dEp08.remove_if(f); this->dEp12.remove_if(f);
}

void ExchangeList::normalise() {
	double probSum = 0.;
	if (this->dEn12.size() > 0) probSum += this->pickingProb[0];
	if (this->dEn08.size() > 0) probSum += this->pickingProb[1];
	if (this->dEn04.size() > 0) probSum += this->pickingProb[2];
	if (this->dE000.size() > 0) probSum += this->pickingProb[3];
	if (this->dEp04.size() > 0) probSum += this->pickingProb[4];
	if (this->dEp08.size() > 0) probSum += this->pickingProb[5];
	if (this->dEp12.size() > 0) probSum += this->pickingProb[6];

	double A;
	std::list<PotEx>::iterator it;

	A = probSum * this->dEn12.size();
	for (it = this->dEn12.begin(); it != this->dEn12.end(); it++)
		it->normP = pickingProb[0] / A;

	A = probSum * this->dEn08.size();
	for (it = this->dEn08.begin(); it != this->dEn08.end(); it++)
		it->normP = pickingProb[1] / A;

	A = probSum * this->dEn04.size();
	for (it = this->dEn04.begin(); it != this->dEn04.end(); it++)
		it->normP = pickingProb[2] / A;

	A = probSum * this->dE000.size();
	for (it = this->dE000.begin(); it != this->dE000.end(); it++)
		it->normP = pickingProb[3] / A;

	A = probSum * this->dEp04.size();
	for (it = this->dEp04.begin(); it != this->dEp04.end(); it++)
		it->normP = pickingProb[4] / A;

	A = probSum * this->dEp08.size();
	for (it = this->dEp08.begin(); it != this->dEp08.end(); it++)
		it->normP = pickingProb[5] / A;

	A = probSum * this->dEp12.size();
	for (it = this->dEp12.begin(); it != this->dEp12.end(); it++)
		it->normP = pickingProb[6] / A;
}

PotEx& ExchangeList::pickEx(double p) {
	std::list<PotEx>::iterator it;
	for (it = this->dEn12.begin(); it != this->dEn12.end(); it ++)
		if ( (p -= it->normP) < 0 ) return *it;
	for (it = this->dEn08.begin(); it != this->dEn08.end(); it ++)
		if ( (p -= it->normP) < 0 ) return *it;
	for (it = this->dEn04.begin(); it != this->dEn04.end(); it ++)
		if ( (p -= it->normP) < 0 ) return *it;
	for (it = this->dE000.begin(); it != this->dE000.end(); it ++)
		if ( (p -= it->normP) < 0 ) return *it;
	for (it = this->dEp04.begin(); it != this->dEp04.end(); it ++)
		if ( (p -= it->normP) < 0 ) return *it;
	for (it = this->dEp08.begin(); it != this->dEp08.end(); it ++)
		if ( (p -= it->normP) < 0 ) return *it;
	for (it = this->dEp12.begin(); it != this->dEp12.end(); it ++)
		if ( (p -= it->normP) < 0 ) return *it;
}

void ExchangeList::update(Ising& c, PotEx& exchange, Context* ctx) {
	int i1 = exchange.i1, j1 = exchange.j1;
	int i2 = exchange.i2, j2 = exchange.j2;
	this->push_back(createPotEx(c, i1, j1, i2, j2, ctx)); // this will always stay
	bool sigma = c(i1, j1);
	if (i1 == i2) {		// exchange happened in same row
		if (sigma != c(i1-1, j1))
			this->push_back(createPotEx(c, i1-1, j1  , i1  , j1  , ctx));
		if (sigma != c(i1+1, j1))
			this->push_back(createPotEx(c, i1  , j1  , i1+1, j1  , ctx));
		if (sigma != c(i1, j1-1))
			this->push_back(createPotEx(c, i1  , j1-1, i1  , j1  , ctx));
		if (sigma == c(i2-1, j2))
			this->push_back(createPotEx(c, i2-1, j2  , i2  , j2  , ctx));
		if (sigma == c(i2+1, j2))
			this->push_back(createPotEx(c, i2  , j2  , i2+1, j2  , ctx));
		if (sigma == c(i2, j2+1))
			this->push_back(createPotEx(c, i2  , j2  , i2  , j2+1, ctx));
	} else
	if (j1 == j2) {		// exchange happened in same column
		if (sigma != c(i1, j1+1))
			this->push_back(createPotEx(c, i1  , j1  , i1  , j1+1, ctx));
		if (sigma != c(i1, j1-1))
			this->push_back(createPotEx(c, i1  , j1-1, i1  , j1  , ctx));
		if (sigma != c(i1-1, j1))
			this->push_back(createPotEx(c, i1-1, j1  , i1  , j1  , ctx));
		if (sigma == c(i2, j2+1))
			this->push_back(createPotEx(c, i2  , j2  , i2  , j2+1, ctx));
		if (sigma == c(i2, j2-1))
			this->push_back(createPotEx(c, i2  , j2-1, i2  , j2  , ctx));
		if (sigma == c(i2+1, j2))
			this->push_back(createPotEx(c, i2  , j2  , i2+1, j2  , ctx));
	}
	/*
	std::cout << this->dEn12.size() << "\t";
	std::cout << this->dEn08.size() << "\t";
	std::cout << this->dEn04.size() << "\t";
	std::cout << this->dE000.size() << "\t";
	std::cout << this->dEp04.size() << "\t";
	std::cout << this->dEp08.size() << "\t";
	std::cout << this->dEp12.size() << "\t";
	std::cout << std::endl;
	*/
}
