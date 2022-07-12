#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "MC.hpp"

#define BIN 1000
#define RUN 1000
#define SEED 15

static Specifications SPECS;

void handleEvents(sf::RenderWindow &w) {
	sf::Event ev;
	while (w.pollEvent(ev)) {
		switch (ev.type) {
		case sf::Event::Closed:
			w.close();
			break;
		case sf::Event::KeyPressed:
			if (ev.key.code == sf::Keyboard::Escape)
				w.close();
			break;
		default:
			break;
		}
	}
}

int main(int argc, char** argv) {
	std::string filename;
	if (argc > 1) filename = argv[1];
	else          filename = "input.txt";

	sf::Font font;
	bool loaded =	font.loadFromFile("./font/times new roman.ttf");
	if (!loaded) {
		std::cout << "Could not load font file." << std::endl;
		return EXIT_FAILURE;
	}

	srand(SEED);
	std::cout << "Initialising system..." << std::endl;
	init_system(filename, &SPECS);

	std::cout << "Setting global values..." << std::endl;
	Ising::setCoupling(SPECS.Coupling);
	Ising::setField(SPECS.Field);

	std::cout << "Initialising configuration" << std::endl;
	Ising config(SPECS.Lx, SPECS.Ly,
							 SPECS.Temperature[0],
							 BoundaryCondition::PERIODIC);
	std::cout << "Generating configuration" << std::endl;
	config.generate();

	int wWidth  = SPECS.Lx * SPECS.scale;
	int wHeight = SPECS.Ly * SPECS.scale;

	sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "Ising model");

	while (window.isOpen()) {
		handleEvents(window);

		window.clear();
		config.drawLattice(window, SPECS.scale);
		for (int i = 0; i < BIN; i++)
			dynamics(&config, &SPECS);
		window.display();
	}

	return EXIT_SUCCESS;
}
