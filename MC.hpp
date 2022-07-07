#include <cmath>
#include "Ising.hpp"

double Probability(double, params);

void dynamics(Ising*);
void spin_flip(Ising*, int, int);
void spin_exchange(Ising*, int, int, int, int);
