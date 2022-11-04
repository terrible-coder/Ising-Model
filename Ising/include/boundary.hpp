#include <iostream>

#include "defaults.hpp"
#include "context.hpp"

/**
 * @brief Takes care of the indexing at the boundary of the lattice.
 * 
 * @param len The size of the lattice.
 * @param uIdx 
 * @param aIdx 
 * @param boundary 
 */
void imposeBC(vec3<uIndx>& len, const vec3<int>& uIdx, vec3<uIndx>* aIdx, vec3<BoundaryCondition>& boundary);

/**
 * @brief Transform from double index to a row major indexing.
 * 
 * @param p 
 * @param len
 * @return uint 
 */
uSize idx3to1(pos const& p, vec3<uIndx> const& L);

/**
 * @brief Transform from a row major index to double indexing.
 * 
 * @param idx The row major index.
 * @param w The width of the lattice.
 * @param i The row index.
 * @param j The column index.
 */
void idx1to3(uSize idx, vec3<uIndx> const& L, pos* p);

/**
 * @brief Check if the index is valid.
 * 
 * @param L 
 * @param idx 
 * @return true 
 * @return false 
 */
bool outOfBounds(vec3<uIndx> const& L, pos const& idx);
