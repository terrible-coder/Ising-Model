#include "monte_carlo.hpp"

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

void spin_flip(Ising& c, Context* ctx) {
	int ri, rj;
	getRandomIndices(c.getWidth(), c.getHeight(), &ri, &rj);
	// The change in energy, if the spin is flipped
	double dE = flipChange(c, ri, rj);
	if (isAccepted(dE, c.getTemp(), ctx))
		c.flip(ri, rj);
}

/**
 * @brief List of possible exchanges arranged in decreasing order of -ve
 * energy change.
 */
static std::list<PotEx> exchanges;

/**
 * @brief Comparator function to sort a list of possible exchanges.
 * 
 * @param first 
 * @param second 
 * @return true 
 * @return false 
 */
bool compareExchange(const PotEx& first, const PotEx& second) {
	return first.delE < second.delE;
}

void createList(Ising& config, Context* ctx) {
	for (uint i = 0; i < config.getHeight(); i++) {
		for (uint j = 0; j < config.getWidth(); j++) {
			bool sigma = config(i, j);
			if (sigma != config(i-1, j  ))	// north
				exchanges.push_back(createPotEx(config, i-1, j  , i  , j  , ctx));
			if (sigma != config(i  , j+1))	// east
				exchanges.push_back(createPotEx(config, i  , j  , i  , j+1, ctx));
		}
	}
}

double picking(double dE, double a) {
	return exp(-a * dE);
}

void normalise(Ising& config, Context* ctx) {
	std::list<PotEx>::iterator it;
	double probSum = 0.;
	for (it = exchanges.begin(); it != exchanges.end(); it++)
		probSum += picking(it->delE, 1.);
	for (it = exchanges.begin(); it != exchanges.end(); it++)
		it->normP = (picking(it->delE, 1.)) / probSum;
}

static bool _firstEx = true;

void spin_exchange(Ising& c, Context* ctx) {
	if (_firstEx) {
		_firstEx = false;
		createList(c, ctx);
	}
	// sample i1, j1 and i2, j2 from the list
	exchanges.sort(compareExchange);
	normalise(c, ctx);
	double p = rProbability();
	std::list<PotEx>::iterator it;
	for (it = exchanges.begin(); it != exchanges.end(); it++) {
		p -= it->normP;
		if (p < 0) break;
	}
	// for (auto ptr = exchanges.begin(); ptr != exchanges.end(); ptr++) {
	// 	if (ptr->delE < 0) {
	// 		std::cout << ptr->delE << "\t" << ptr->normP << "\t";
	// 		std::cout << ptr->i1 << ", " << ptr->j1 << "\t\t";
	// 		std::cout << ptr->i2 << ", " << ptr->j2 << "\t";
	// 		std::cout << c(ptr->i1, ptr->j1) << " " << c(ptr->i2, ptr->j2) << std::endl;
	// 	}
	// 	if (c(ptr->i1, ptr->j1) == c(ptr->i2, ptr->j2)) {
	// 		std::cout << "WHATTTT" << std::endl;
	// 		break;
	// 	}
	// }
	// std::cout << "Exchanged: " << it->i1 << ", " << it->j1 << "\t";
	// std::cout << it->i2 << ", " << it->j2 << std::endl;
	std::cout << "size: " << exchanges.size() << "\t" << exchanges.begin()->delE << std::endl;
	// std::cout << it->delE << "\t" << exchanges.begin()->delE << "\t\t";
	// std::cout << it->normP << "\t" << exchanges.begin()->normP << std::endl;
	int i1 = it->i1, j1 = it->j1;
	int i2 = it->i2, j2 = it->j2;
	c.exchange(i1, j1, i2, j2);
	// remove all exchanges with i1,j1 or i2,j2 except the one i1,j1 and i2,j2

	exchanges.remove_if([i1, j1, i2, j2](const PotEx& ex) {
		return (ex.i1 == i1 && ex.j1 == j1) ||
					 (ex.i2 == i2 && ex.j2 == j2) ||
					 (ex.i1 == i2 && ex.j1 == j2) ||
					 (ex.i2 == i1 && ex.j2 == j1);
	});

	// scan the rectangle of neighbours to construct the new exchanges
	exchanges.push_back(createPotEx(c, i1, j1, i2, j2, ctx)); // this will always stay
	bool sigma = c(i1, j1);
	if (i1 == i2) {		// exchange happened in same row
		if (sigma != c(i1-1, j1))
			exchanges.push_back(createPotEx(c, i1-1, j1  , i1  , j1  , ctx));
		if (sigma != c(i1+1, j1))
			exchanges.push_back(createPotEx(c, i1  , j1  , i1+1, j1  , ctx));
		if (sigma != c(i1, j1-1))
			exchanges.push_back(createPotEx(c, i1  , j1-1, i1  , j1  , ctx));
		if (sigma == c(i2-1, j2))
			exchanges.push_back(createPotEx(c, i2-1, j2  , i2  , j2  , ctx));
		if (sigma == c(i2+1, j2))
			exchanges.push_back(createPotEx(c, i2  , j2  , i2+1, j2  , ctx));
		if (sigma == c(i2, j2+1))
			exchanges.push_back(createPotEx(c, i2  , j2  , i2  , j2+1, ctx));
	} else
	if (j1 == j2) {		// exchange happened in same column
		if (sigma != c(i1, j1+1))
			exchanges.push_back(createPotEx(c, i1  , j1  , i1  , j1+1, ctx));
		if (sigma != c(i1, j1-1))
			exchanges.push_back(createPotEx(c, i1  , j1-1, i1  , j1  , ctx));
		if (sigma != c(i1-1, j1))
			exchanges.push_back(createPotEx(c, i1-1, j1  , i1  , j1  , ctx));
		if (sigma == c(i2, j2+1))
			exchanges.push_back(createPotEx(c, i2  , j2  , i2  , j2+1, ctx));
		if (sigma == c(i2, j2-1))
			exchanges.push_back(createPotEx(c, i2  , j2-1, i2  , j2  , ctx));
		if (sigma == c(i2+1, j2))
			exchanges.push_back(createPotEx(c, i2  , j2  , i2+1, j2  , ctx));
	}
}
