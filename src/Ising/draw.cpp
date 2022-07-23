/**
 * @file draw.cpp
 * @author your name (you@domain.com)
 * @brief Contains the code for rendering the spin lattice.
 * @version 0.1
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "Ising.hpp"

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

void Ising::saveFrame(int count) {
	std::string name = "image/con" + std::to_string(this->getTemp())
									 + "fr" + std::to_string(count) + ".ppm";
	std::ofstream frame(name);
	frame << "P3" << std::endl;
	frame << this->Lx << " " << this->Ly << std::endl;
	frame << "255" << std::endl;
	for (int y = 0; y < this->Ly; y++) {
		for (int x = 0; x < this->Lx; x++) {
			int val = this->lattice[y][x] ? 255 : 0;
			frame << val << " " << val << " " << val << std::endl;
		}
	}
}
