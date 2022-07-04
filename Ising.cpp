#include "Ising.hpp"

params P;

void initConfig(params* p) {
	P = *p;
}
void initConfig(std::string file) {
	P = readInput(file);
}

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

Ising::Ising(params* input) {
	this->p = input;
	this->N = input->width * input->height;
	this->boundary = (BoundaryCondition) input->boundary;
	// this->J = input->J;
	// this->H = input->H;
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
			*ai = (ui + this->p->height) % this->p->height; // For periodic boundary condition
			*aj = (uj + this->p->width) % this->p->width;  //  the lattice is lives on a torus
			break;

		case BoundaryCondition::SCREW:
			*ai = (ui + this->p->height) % this->p->height; // The lattices sites are on a
			if (uj >= this->p->height || uj < 0) {      //  single string. They wrap from
				*aj = (uj + this->p->width) % this->p->width; // end of a row to the start of
				*ai += uj >= this->p->width ? 1 : -1;     //  next row
			}
			break;

		case BoundaryCondition::FREE:
			*ai = (ui >= this->p->height || ui < 0) ? -1 : ui; // For free edge boundary
			*aj = (uj >= this->p->width  || uj < 0) ? -1 : uj; // the site sees nothing
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

double Ising::getField() {return this->p->H;}
double Ising::getNNCoup() {return this->p->J;}
int Ising::getWidth() {return this->p->width;}
int Ising::getHeight() {return this->p->height;}
params* Ising::getParams() {return this->p;}

/**
 * @brief Generates a lattice of spins of given dimensions. The spins are
 * represented as boolean values (`true` for up and `false` for down).
 * 
 */
void Ising::generate() {
	this->lattice = new bool*[this->p->height];
	for (int i = 0; i < this->p->height; i++)
		this->lattice[i] = (bool*) malloc(this->p->width * sizeof(bool));

	for (int i = 0; i < this->p->height; i++)
		for (int j = 0; j < this->p->width; j++)
			this->lattice[i][j] = rand() % 2 == 0;
}

/**
 * @brief Prints the lattice in text format into the console.
 * 
 */
void Ising::printLattice() {
	for (int i = 0; i < this->p->height; i++) {
		for (int j = 0; j < this->p->width; j++)
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
void Ising::drawLattice(sf::RenderWindow& w) {
	for (int i = 0; i < this->p->height; i++) {
		for (int j = 0; j < this->p->width; j++) {
			sf::RectangleShape s(sf::Vector2f(this->p->scale, this->p->scale));
			s.setPosition(j*this->p->scale, i*this->p->scale);
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
 * @brief Computes the Ising hamiltonian for a given configuration.
 * 
 * @return double 
 */
double Ising::Hamiltonian() {
	int w = this->p->width, h = this->p->height;
	double E = 0.;
	int SS = 0;
	// Single spin terms
	for (int i = 0; i < this->p->height; i++)
		for (int j = 0; j < this->p->width; j++)
			SS += this->lattice[i][j];
	E = -this->p->H * bool2spin(SS, this->N);

	// Two spin interaction terms
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++) {
			bool north = (*this)(i-1, j  ); // this->lattice[(i+h-1)%h][j  ];
			bool east  = (*this)(i  , j+1); // this->lattice[i  ][(j+w+1)%w];
			// bool south = grid[(i+h+1)%h][j  ];
			// bool west  = grid[i  ][(j+w-1)%w];
			SS = bool2spin(north + east, 2);
			E -= this->p->J * SS * bool2spin((*this)(i, j));
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
	for (int i = 0; i < this->p->height; i++)
		for (int j = 0; j < this->p->width; j++)
			M += (*this)(i, j);
	return bool2spin(M, this->N);
}
