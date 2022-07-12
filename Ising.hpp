#include <iostream>
#include <string.h>
#include <SFML/Graphics.hpp>

double bool2spin(bool);
double bool2spin(int, int);

enum BoundaryCondition {
	PERIODIC = 0,
	SCREW,
	FREE
};

class Ising {
private:
	static double J, H;
	// static inline bool _setJ = false, _setH = false;
	static bool _setJ, _setH;
	int Lx, Ly;
	int N;
	double T;
	bool** lattice;
	BoundaryCondition boundary;

	void BC(int ui, int uj, int* ai, int* aj);

public:

	Ising(int w, int h,
				double temperature,
				BoundaryCondition b);
	~Ising();

	static void setCoupling(double coupling);
	static double getNNCoup();
	static void setField(double field);
	static double getField();

	bool operator() (int i, int j);

	int getHeight();
	int getWidth();
	int getSize();
	double getTemp();

	void generate();

	void printLattice();

	void drawLattice(sf::RenderWindow& w, int scale);

	void flip(int i, int j);
	void exchange(int i1, int j1, int i2, int j2);

	double Hamiltonian();

	double Magnetisation();
};


// bool Ising::_setJ;
// bool Ising::_setH;
// double Ising::H;
// double Ising::J;
