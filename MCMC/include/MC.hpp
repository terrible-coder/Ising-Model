#include <cmath>
#include "Ising.hpp"
#include "context.hpp"

double Probability(double, double);

void dynamics(Ising& c, Context* ctx);
void spin_flip(Ising& c, int i, int j, Context* ctx);
void spin_exchange(Ising& c, int i1, int j1, int i2, int j2, Context* ctx);
