#include "energyChange.hpp"

float flipChange(Ising& c, pos const& i) {
	float sigma = bool2spin(c(i));
	uIndx S = 0;
	vec3<int> off;

	if (c.getSizeX() > 1) {        // do not consider neighbour if only single layer thick.
		off = {1, 0, 0};  // x-offset
		S +=	c(i + off) + c(i - off);
	}
	if (c.getSizeY() > 1) {        // do not consider neighbour if only single layer thick.
		off = {0, 1, 0};  // y-offset
		S +=	c(i + off) + c(i - off);
	}
	if (c.getSizeZ() > 1) {        // do not consider neighbour if only single layer thick.
		off = {0, 0, 1};  // z-offset
		S +=	c(i + off) + c(i - off);
	}
	return sigma * bool2spin(S, 6u);
}
float flipChange(Ising& c, vec3<int> const& i) {
	pos ai = c.equiv(i);
	return flipChange(c, ai);
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

// PotEx createPotEx(Ising& config, int i1, int j1, int i2, int j2, Context* ctx) {
// 	const double BETA = 1 / (ctx->BoltzConstant * config.getTemp());
// 	PotEx ptr;
// 	config.equiv(&i1, &j1);
// 	config.equiv(&i2, &j2);
// 	ptr.i1 = i1; ptr.j1 = j1;
// 	ptr.i2 = i2; ptr.j2 = j2;
// 	ptr.delE = exchangeChange(config, i1, j1, i2, j2);
// 	// ptr.transition = Probability(ptr.delE, BETA, ctx);
// 	return ptr;
// }

// ExchangeList::ExchangeList() {}

// ExchangeList::ExchangeList(std::function<double(double)> pick) {
// 	this->isCreated = false;
// 	double a = 4. * Ising::getNNCoup();
// 	for (int i = 0; i < 7; i++) {
// 		this->pickingProb[i] = pick((i-3) * a);
// 		this->normProb[i] = 0.;
// 	}
// }

// void ExchangeList::push_back(const PotEx& exchange) {
// 	int dE; // in units of J
// 	if (exchange.delE > 0) dE = (int) (exchange.delE / Ising::getNNCoup() + 0.5);
// 	else dE = (int) (exchange.delE / Ising::getNNCoup() - 0.5);

// 	if (dE == -12) this->dEn12.push_back(exchange); else
// 	if (dE == - 8) this->dEn08.push_back(exchange); else
// 	if (dE == - 4) this->dEn04.push_back(exchange); else
// 	if (dE ==   0) this->dE000.push_back(exchange); else
// 	if (dE == + 4) this->dEp04.push_back(exchange); else
// 	if (dE == + 8) this->dEp08.push_back(exchange); else
// //if (delE == +12)
// 		this->dEp12.push_back(exchange);
// }

// void ExchangeList::createList(Ising& config, Context* ctx) {
// 	if (this->isCreated) return;
// 	this->isCreated = true;
// 	for (uint i = 0; i < config.getHeight(); i++)
// 		for (uint j = 0; j < config.getWidth(); j++) {
// 			bool sigma = config(i, j);
// 			if (sigma != config(i-1, j  ))	// north
// 				this->push_back(createPotEx(config, i-1, j  , i  , j  , ctx));
// 			if (sigma != config(i  , j+1))	// east
// 				this->push_back(createPotEx(config, i  , j  , i  , j+1, ctx));
// 		}
// }

// void ExchangeList::remove_if(const std::function<bool(const PotEx&)>& f) {
// 	this->dEn12.remove_if(f); this->dEn08.remove_if(f); this->dEn04.remove_if(f);
// 	this->dE000.remove_if(f);
// 	this->dEp04.remove_if(f); this->dEp08.remove_if(f); this->dEp12.remove_if(f);
// }

// void ExchangeList::normalise() {
// 	double probSum = 0.;
// 	probSum += (this->dEn12.size() > 0) * this->pickingProb[0];
// 	probSum += (this->dEn08.size() > 0) * this->pickingProb[1];
// 	probSum += (this->dEn04.size() > 0) * this->pickingProb[2];
// 	probSum += (this->dE000.size() > 0) * this->pickingProb[3];
// 	probSum += (this->dEp04.size() > 0) * this->pickingProb[4];
// 	probSum += (this->dEp08.size() > 0) * this->pickingProb[5];
// 	probSum += (this->dEp12.size() > 0) * this->pickingProb[6];

// 	this->normProb[0] = (this->dEn12.size() > 0) * this->pickingProb[0] / probSum;
// 	this->normProb[1] = (this->dEn08.size() > 0) * this->pickingProb[1] / probSum;
// 	this->normProb[2] = (this->dEn04.size() > 0) * this->pickingProb[2] / probSum;
// 	this->normProb[3] = (this->dE000.size() > 0) * this->pickingProb[3] / probSum;
// 	this->normProb[4] = (this->dEp04.size() > 0) * this->pickingProb[4] / probSum;
// 	this->normProb[5] = (this->dEp08.size() > 0) * this->pickingProb[5] / probSum;
// 	this->normProb[6] = (this->dEp12.size() > 0) * this->pickingProb[6] / probSum;
// }

// PotEx& findInList(std::list<PotEx>& enList, double p, double r) {
// 	int a = (int) (enList.size() * p / r);
// 	std::list<PotEx>::iterator it = enList.begin();
// 	std::advance(it, a);
// 	return *it;
// }

// PotEx& ExchangeList::pickEx(double p) {
// 	if (p < normProb[0]) return findInList(dEn12, p, normProb[0]);
// 	else p -= normProb[0];
// 	if (p < normProb[1]) return findInList(dEn08, p, normProb[1]);
// 	else p -= normProb[1];
// 	if (p < normProb[2]) return findInList(dEn04, p, normProb[2]);
// 	else p -= normProb[2];
// 	if (p < normProb[3]) return findInList(dE000, p, normProb[3]);
// 	else p -= normProb[3];
// 	if (p < normProb[4]) return findInList(dEp04, p, normProb[4]);
// 	else p -= normProb[4];
// 	if (p < normProb[5]) return findInList(dEp08, p, normProb[5]);
// 	else p -= normProb[5];
// //if (p < normProb[6])
// 		return findInList(dEp12, p, normProb[6]);
// //else p -= this->normProb[6];
// }

// void ExchangeList::update(Ising& c, PotEx& exchange, Context* ctx) {
// 	int i1 = exchange.i1, j1 = exchange.j1;
// 	int i2 = exchange.i2, j2 = exchange.j2;
// 	this->push_back(createPotEx(c, i1, j1, i2, j2, ctx)); // this will always stay
// 	bool sigma = c(i1, j1);
// 	if (i1 == i2) {		// exchange happened in same row
// 		if (sigma != c(i1-1, j1))
// 			this->push_back(createPotEx(c, i1-1, j1  , i1  , j1  , ctx));
// 		if (sigma != c(i1+1, j1))
// 			this->push_back(createPotEx(c, i1  , j1  , i1+1, j1  , ctx));
// 		if (sigma != c(i1, j1-1))
// 			this->push_back(createPotEx(c, i1  , j1-1, i1  , j1  , ctx));
// 		if (sigma == c(i2-1, j2))
// 			this->push_back(createPotEx(c, i2-1, j2  , i2  , j2  , ctx));
// 		if (sigma == c(i2+1, j2))
// 			this->push_back(createPotEx(c, i2  , j2  , i2+1, j2  , ctx));
// 		if (sigma == c(i2, j2+1))
// 			this->push_back(createPotEx(c, i2  , j2  , i2  , j2+1, ctx));
// 	} else
// 	if (j1 == j2) {		// exchange happened in same column
// 		if (sigma != c(i1, j1+1))
// 			this->push_back(createPotEx(c, i1  , j1  , i1  , j1+1, ctx));
// 		if (sigma != c(i1, j1-1))
// 			this->push_back(createPotEx(c, i1  , j1-1, i1  , j1  , ctx));
// 		if (sigma != c(i1-1, j1))
// 			this->push_back(createPotEx(c, i1-1, j1  , i1  , j1  , ctx));
// 		if (sigma == c(i2, j2+1))
// 			this->push_back(createPotEx(c, i2  , j2  , i2  , j2+1, ctx));
// 		if (sigma == c(i2, j2-1))
// 			this->push_back(createPotEx(c, i2  , j2-1, i2  , j2  , ctx));
// 		if (sigma == c(i2+1, j2))
// 			this->push_back(createPotEx(c, i2  , j2  , i2+1, j2  , ctx));
// 	}
// 	/*
// 	std::cout << this->dEn12.size() << "\t";
// 	std::cout << this->dEn08.size() << "\t";
// 	std::cout << this->dEn04.size() << "\t";
// 	std::cout << this->dE000.size() << "\t";
// 	std::cout << this->dEp04.size() << "\t";
// 	std::cout << this->dEp08.size() << "\t";
// 	std::cout << this->dEp12.size() << "\t";
// 	std::cout << std::endl;
// 	*/
// }
