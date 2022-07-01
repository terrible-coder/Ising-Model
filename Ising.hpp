#include <iostream>

double bool2spin(bool);
double bool2spin(int, int);
bool** generate(int, int);
void printLattice(bool**, int, int);
void bc(int, int, const int*, const int*);
double Hamiltonian(const bool**, int, int);
double Magnetisation(const bool**, int, int);
