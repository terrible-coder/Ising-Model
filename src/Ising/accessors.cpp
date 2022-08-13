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

double Ising::getField() {
	return Ising::H;
}

double Ising::getNNCoup() {
	return Ising::J;
}

std::uint16_t Ising::getWidth() {
	return this->Lx;
}

std::uint16_t Ising::getHeight() {
	return this->Ly;
}

std::uint16_t Ising::getSize() {
	return this->N;
}

double Ising::getTemp() {
	return this->T;
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
bool Ising::operator() (uint i, uint j) {
	int ii, jj;
	imposeBC(this->Lx, this-> Ly, i, j, &ii, &jj, this->boundary);

	if (ii == -1 || jj == -1)
		return NULL;
	uint idx = ii * this->Lx + jj;
	return (this->lattice[idx / WORD_SIZE] >> (WORD_SIZE - idx%WORD_SIZE - 1)) & 1 ;
}
