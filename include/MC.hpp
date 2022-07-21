#include <cmath>
#include "Ising.hpp"
#include "specs.hpp"

double Probability(double, double);

void dynamics(Ising* config, Specifications* S);
void spin_flip(Ising* config, int i, int j, Specifications* S);
void spin_exchange(Ising*, int i1, int j1, int i2, int j2, Specifications* S);
