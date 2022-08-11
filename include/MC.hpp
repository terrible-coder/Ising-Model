#include <cmath>
#include "Ising.hpp"
#include "context.hpp"

double Probability(double, double);

void dynamics(Ising* config, Context* ctx);
void spin_flip(Ising* config, int i, int j, Context* ctx);
void spin_exchange(Ising*, int i1, int j1, int i2, int j2, Context* ctx);
