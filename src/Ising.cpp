#include "Ising.hpp"

/**
 * @brief Converts boolean representation of spin (`true` for up `false` for down)
 * to the spin representation in Ising model.
 * 
 * @param s The spin value: `true` for up and `false` for down.
 * @return double `+1.0` for up and `-1.0` for down.
 */
double bool2spin(bool s) {
	return 2.*s - 1.;
}
/**
 * @brief Converts sum of spins in boolean representation to integer representation.
 * 
 * @param S The sum of booleans.
 * @param n Number of spins that have been summed.
 * @return double 
 */
double bool2spin(int S, int n) {
	return 2.*S - n;
}

bool Ising::_setJ = false;
bool Ising::_setH = false;
double Ising::H;
double Ising::J;

void Ising::setCoupling(double coupling) {
	if (Ising::_setJ) return;
	Ising::_setJ = true;
	Ising::J = coupling;
}

void Ising::setField(double field) {
	if (Ising::_setH) return;
	Ising::_setH = true;
	Ising::H = field;
}

Ising::Ising(int w, int h,
						 double temperature,
						 BoundaryCondition b) {
	this->Lx = w;
	this->Ly = h;
	this->N  = w * h;
	this->T  = temperature;
	this->boundary = b;
	this->is_generated = false;

	std::cout << "Temperature of config: " << this->T << std::endl;
}

Ising::~Ising() {
	// free(this->lattice);
}

/**
 * @brief Takes care of the indexing at the boundary of the lattice.
 * 
 * @param ui The row index we "want" to see.
 * @param uj The column index we "want" to see.
 * @param ai The row index the lattice site sees.
 * @param aj The column index the lattice site sees.
 */
void Ising::BC(int ui, int uj, int* ai, int* aj) {
	switch (this->boundary) {
		case BoundaryCondition::PERIODIC:
			*ai = (ui + this->Ly) % this->Ly; // For periodic boundary condition
			*aj = (uj + this->Lx) % this->Lx;  //  the lattice is lives on a torus
			break;

		case BoundaryCondition::SCREW:
			*ai = (ui + this->Ly) % this->Ly;    // The lattices sites are on a
			if (uj >= this->Ly || uj < 0) {      // single string. They wrap from
				*aj = (uj + this->Lx) % this->Lx;  // end of a row to the start of
				*ai += uj >= this->Lx ? 1 : -1;    // next row
			}
			break;

		case BoundaryCondition::FREE:
			*ai = (ui >= this->Ly || ui < 0) ? -1 : ui; // For free edge boundary
			*aj = (uj >= this->Lx  || uj < 0) ? -1 : uj; // the site sees nothing
			break;

		default:
			std::cout << "Unknown boundary condition" << std::endl;
	}
}

/**
 * @brief Lattice point accessor. The index is of the site we "want" to look at.
 * The function takes care of the appropriate boundary conditions and returns
 * the spin value at the actual index in the grid. The function can return `NULL`
 * if the spin is supposed to be interpreted as absent at `(i, j)`.
 * 
 * @param i The row index we "want" to look at.
 * @param j The column index we "want" to look at.
 * @return true 
 * @return false 
 */
bool Ising::operator() (int i, int j) {
	int ii, jj;
	this->BC(i, j, &ii, &jj);

	if (ii == -1 || jj == -1)
		return NULL;
	
	return this->lattice[ii][jj];
}

double Ising::getField() {return Ising::H;}
double Ising::getNNCoup() {return Ising::J;}
int Ising::getWidth() {return this->Lx;}
int Ising::getHeight() {return this->Ly;}
int Ising::getSize() {return this->N;}
double Ising::getTemp() {return this->T;}

/**
 * @brief Generates a lattice of spins of given dimensions. The spins are
 * represented as boolean values (`true` for up and `false` for down).
 * This is the initial configuration of the model. That is, all ensemble
 * members will start from this configuration.
 * 
 * Note: This function should not be called more than once.
 */
void Ising::generate() {
	// Allow generation of initial configuration only once.
	if (this->is_generated) {
		std::cout << "Lattice already generated. Cannot be generated more than once." << std::endl;
		return;
	}
	this->is_generated = true;

	this->initial = new bool*[this->Ly];
	for (int i = 0; i < this->Ly; i++)
		this->initial[i] = (bool*) malloc(this->Lx * sizeof(bool));

	// Tie the seed value to the size of the lattice. This ensures the same
	// initial lattice for every type of experiment.
	int seed = this->N + (int)(this->T * 1000. + 0.5);
	std::default_random_engine RNG(seed);
	std::uniform_int_distribution<int> dist(0, 1);

	for (int i = 0; i < this->Ly; i++)
		for (int j = 0; j < this->Lx; j++)
			this->initial[i][j] = (bool) dist(RNG);

	this->lattice = new bool*[this->Ly];
	for (int i = 0; i < this->Ly; i++)
		this->lattice[i] = (bool*) malloc(this->Lx * sizeof(bool));
}

/**
 * @brief Initialise the lattice to the initial configuration generated.
 */
void Ising::reinit() {
	if (!this->is_generated) {
		std::cout << "Initial state not generated yet. Try that first." << std::endl;
		return;
	}
	// copy initial data into lattice
	for (int i = 0; i < this->Ly; i++)
		for (int j = 0; j < this->Lx; j++)
			this->lattice[i][j] = this->initial[i][j];
}

/**
 * @brief Prints the lattice in text format into the console.
 */
void Ising::printLattice() {
	for (int i = 0; i < this->Ly; i++) {
		for (int j = 0; j < this->Lx; j++)
			std::cout << ((*this)(i, j) ? "+" : "-") << "  ";
		std::cout << std::endl;
	}
}

/**
 * @brief Render the lattice onto an SFML window. Each site is drawn as a
 * `scale x scale` rectangle. White represents spin up and black represents
 * spin down.
 * 
 * @param w The SFML render window to draw on.
 * @param scale The visual size of each lattice point.
 */
void Ising::drawLattice(sf::RenderWindow& w, int scale) {
	for (int i = 0; i < this->Ly; i++) {
		for (int j = 0; j < this->Lx; j++) {
			sf::RectangleShape s(sf::Vector2f(scale, scale));
			s.setPosition(j*scale, i*scale);
			if ((*this)(i, j)) s.setFillColor(sf::Color::White);
			else s.setFillColor(sf::Color::Black);
			w.draw(s);
		}
	}
}

/**
 * @brief Flip the spin at the given index.
 * 
 * @param i The row index.
 * @param j The column index.
 */
void Ising::flip(int i, int j) {
	this->lattice[i][j] = !this->lattice[i][j];
}

/**
 * @brief Exchange the spins at the given indices.
 * 
 * @param i1 Row index 1.
 * @param j1 Column index 1.
 * @param i2 Row index 2.
 * @param j2 Column index 2.
 */
void Ising::exchange(int i1, int j1, int i2, int j2) {
	int i1a, j1a;
	int i2a, j2a;
	this->BC(i1, j1, &i1a, &j1a);
	this->BC(i2, j2, &i2a, &j2a);

	bool temp = this->lattice[i1a][j1a];
	this->lattice[i1a][j1a] = this->lattice[i2a][j2a];
	this->lattice[i2a][j2a] = temp;
}

/**
 * @brief Computes the Ising hamiltonian for a given configuration.
 * 
 * @return double 
 */
double Ising::Hamiltonian() {
	int w = this->Lx, h = this->Ly;
	double E = 0.;
	int SS = 0;

	// Single spin terms
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			SS += this->lattice[i][j];
	E = -Ising::getField() * bool2spin(SS, this->N);

	// Two spin interaction terms
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++) {
			bool north = (*this)(i-1, j  );
			bool east  = (*this)(i  , j+1);

			SS = bool2spin(north + east, 2);
			E -= Ising::getNNCoup() * SS * bool2spin((*this)(i, j));
		}
	return E;
}

/**
 * @brief Calculate the magnetisation of the configuration.
 * 
 * @return double 
 */
double Ising::Magnetisation() {
	int M = 0;
	for (int i = 0; i < this->Ly; i++)
		for (int j = 0; j < this->Lx; j++)
			M += this->lattice[i][j];
	return bool2spin(M, this->N);
}
