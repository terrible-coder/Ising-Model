#include <iostream>
#include <string.h>
#include "params.hpp"
#include <SFML/Graphics.hpp>

void initConfig(params*);
void initConfig(std::string);
double bool2spin(bool);
double bool2spin(int, int);

enum BoundaryCondition {
	PERIODIC = 0,
	SCREW,
	FREE
};

class Ising {
private:

	// int WIDTH, HEIGHT;
	int N;
	params* p;
	bool** lattice;
	BoundaryCondition boundary;

	void BC(int ui, int uj, int* ai, int* aj);

public:

	Ising(params* input);
	~Ising();

	bool operator() (int i, int j);

	double getField();
	double getNNCoup();
	int getHeight();
	int getWidth();
	int getSize();
	params* getParams();

	void generate();

	void printLattice();

	void drawLattice(sf::RenderWindow& w);

	void flip(int i, int j);
	void exchange(int i1, int j1, int i2, int j2);

	double Hamiltonian();

	double Magnetisation();
};
