#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "Ising.hpp"

int main() {
	srand(15);
	initConfig("input.txt");
	bool** lattice = generate(5, 5);
	printLattice(lattice, 5, 5);

	return 0;
}
