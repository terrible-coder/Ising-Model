#include <iostream>

enum BoundaryCondition {
	PERIODIC = 0,
	SCREW,
	FREE
};

/**
 * @brief Takes care of the indexing at the boundary of the lattice.
 * 
 * @param ui The row index we "want" to see.
 * @param uj The column index we "want" to see.
 * @param ai The row index the lattice site sees.
 * @param aj The column index the lattice site sees.
 */
void imposeBC(int w, int h, int ui, int uj, int* ai, int* aj, BoundaryCondition boundary);

/**
 * @brief Transform from double index to a row major indexing.
 * 
 * @param i The row index.
 * @param j The column index.
 * @param w The width of the lattice.
 * @return uint 
 */
uint idx2to1(uint i, uint j, uint w);

/**
 * @brief Transform from a row major index to double indexing.
 * 
 * @param idx The row major index.
 * @param w The width of the lattice.
 * @param i The row index.
 * @param j The column index.
 */
void idx1to2(uint idx, uint w, uint* i, uint* j);
