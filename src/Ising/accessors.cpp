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

uWord_t* Ising::getRaw() {
	return this->lattice;
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
	uint idx = idx2to1(ii, jj, this->Lx);
	return (this->lattice[idx / WORD_SIZE] >> (WORD_SIZE - idx%WORD_SIZE - 1)) & 1 ;
}

void Ising::__leftShift(uWord_t* shifted) {
	bool* MSBs = new bool[this->rawN];
	for (uint idx = 0; idx < this->rawN; idx++) {
		MSBs[idx] = this->lattice[idx] >> (WORD_SIZE - 1);
		shifted[idx] = this->lattice[idx] << 1;
	}
	for (uint idx = 0; idx < this->N; idx += WORD_SIZE) {
		int i, j;
		idx1to2(idx, this->Lx, (uint*)(&i), (uint*)(&j));
		imposeBC(this->Lx, this->Ly, i, j-1, &i, &j, this->boundary);
		if (i == -1 || j == -1) // for free boundary condition
			continue;
		shifted[idx2to1(i, j, this->Lx) / WORD_SIZE] |= MSBs[idx / WORD_SIZE];
	}
}

void Ising::__downShift(uWord_t* shifted) {
	for (uint i = this->rawX; i < this->rawY; i++)
		shifted[i] = this->lattice[i - this->rawX];
	int k, _;
	imposeBC(this->Lx, this->Ly, -1, 0, &k, &_, this->boundary);
	if (k == -1) // for free boundary condition
		for (uint i = 0; i < rawX; i++)
			shifted[i] = 0;
	else
		for (uint i = 0; i < this->rawX; i++)
			shifted[i] = this->lattice[idx2to1(k, i, this->rawX)];
}
