#include <list>
#include <cmath>
#include <functional>

#include "transition.hpp"
#include "Ising.hpp"

/**
 * @brief Store data about a potential exchange. It stores the position of the
 * spins to exchange and the transition rate.
 * 
 */
struct PotEx {
	int i1, j1;
	int i2, j2;
	double delE;
	// double transition;
};

/**
 * @brief Create a Pot Ex object.
 * 
 * @param i1 
 * @param j1 
 * @param i2 
 * @param j2 
 * @param dE 
 * @param ctx 
 * @return struct PotEx* 
 */
PotEx createPotEx(Ising& config, int i1, int j1, int i2, int j2, Context* ctx);

/**
 * @brief Calculates the change in energy (in units of J) for flipping of
 * single spin.
 * 
 * @param c 
 * @param i 
 * @param j 
 * @return double 
 */
double flipChange(Ising& c, uint i, uint j);

/**
 * @brief Calculates the change in energy (in units of J) for exchanging two
 * neighbouring spins.
 * 
 * @param c 
 * @param i1 
 * @param j1 
 * @param i2 
 * @param j2 
 * @return double 
 */
double exchangeChange(Ising& c, int i1, int j1, int i2, int j2);

class ExchangeList {
private:
	// List of exchanges with dE = -12 J
	std::list<PotEx> dEn12;
	// List of exchanges with dE = - 8 J
	std::list<PotEx> dEn08;
	// List of exchanges with dE = - 4 J
	std::list<PotEx> dEn04;
	// List of exchanges with dE =   0 J
	std::list<PotEx> dE000;
	// List of exchanges with dE = + 4 J
	std::list<PotEx> dEp04;
	// List of exchanges with dE = + 8 J
	std::list<PotEx> dEp08;
	// List of exchanges with dE = +12 J
	std::list<PotEx> dEp12;

	double pickingProb[7];
	double normProb[7];
	bool isCreated;

public:

	ExchangeList();
	ExchangeList(std::function<double(double E)> pick);

	/**
	 * @brief Push a potential exchange into the corresponding list.
	 * 
	 * @param exchange 
	 */
	void push_back(const PotEx& exchange);

	void createList(Ising& config, Context* ctx);

	/**
	 * @brief Remove elements from all containers given by the condition function.
	 * 
	 * @param f 
	 */
	void remove_if(const std::function<bool(const PotEx&)>& f);

	void normalise();

	PotEx& pickEx(double p);

	/**
	 * @brief Update all lists given that the passed exchange has been selected.
	 * 
	 * @param exchange 
	 */
	void update(Ising& c, PotEx& exchange, Context* ctx);
};
