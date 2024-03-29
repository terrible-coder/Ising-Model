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

#include "io/draw.hpp"

/**
 * @brief Prints the lattice in text format into the console.
 */
void printLattice(Ising* config) {
	for (int i = 0; i < config->getHeight(); i++) {
		for (int j = 0; j < config->getWidth(); j++)
			std::cout << ((*config)(i, j) ? "+" : "-") << "  ";
		std::cout << std::endl;
	}
}

/**
 * @brief Render the lattice onto an SFML window. Each site is drawn as a
 * `scale x scale` rectangle. White represents spin up and black represents
 * spin down.
 * 
 * @param rt The SFML render window to draw on.
 * @param scale The visual size of each lattice point.
 */
void drawLattice(Ising* config, int scale, sf::RenderTexture& rt) {
	for (int i = 0; i < config->getHeight(); i++) {
		for (int j = 0; j < config->getWidth(); j++) {
			if(!(*config)(i, j))
				continue;
			sf::RectangleShape s(sf::Vector2f(scale, scale));
			s.setPosition(j*scale, i*scale);
			s.setFillColor(sf::Color::White);
			rt.draw(s);
		}
	}
}

void saveFrame(Ising* config, int count, int en, sf::RenderTexture& rt) {
	static int _last_en = -1;

	std::string dir = "image/con" + std::to_string(config->getTemp()) + "/"
									+ "en" + std::to_string(en+1) + "/";

	if (en != _last_en) {
		if (std::filesystem::create_directories(dir) == 0)
			std::cout << "Problem creating directory." << std::endl;
		_last_en = en;
	}

	std::string path = dir + "frame" + std::to_string(count) + ".png";
	rt.getTexture().copyToImage().saveToFile(path);
}
