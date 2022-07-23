/**
 * @file Ising_acc.cpp
 * @author your name (you@domain.com)
 * @brief This file contains the getters and setters for the Ising class.
 * @version 0.1
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "Ising.hpp"

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

double Ising::getField()  {  return Ising::H;  }
double Ising::getNNCoup() {  return Ising::J;  }
int Ising::getWidth()     {  return this->Lx;  }
int Ising::getHeight()    {  return this->Ly;  }
int Ising::getSize()      {  return this->N;   }
double Ising::getTemp()   {  return this->T;   }

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
