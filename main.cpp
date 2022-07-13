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

	std::cout << SPECS.ENSEMBLE_SIZE << " " << SPECS._t_points << std::endl;

	std::ofstream energyOutput;
	energyOutput.open("energy"+std::to_string(SPECS.Temperature[0]) + ".csv");

	std::cout << "Setting global values..." << std::endl;
	Ising::setCoupling(SPECS.Coupling);
	Ising::setField(SPECS.Field);

	std::cout << "Initialising configuration" << std::endl;
	Ising config(SPECS.Lx, SPECS.Ly,
							 SPECS.Temperature[0],
							 BoundaryCondition::PERIODIC);

	sf::Text status;
	status.setFont(font);
	status.setString("Hello! I am just sitting here.");
	status.setCharacterSize(16);
	status.setFillColor(sf::Color::Yellow);
	status.setStyle(sf::Text::Bold);

	int sysWidth  = SPECS.Lx * SPECS.scale;
	int sysHeight = SPECS.Ly * SPECS.scale;
	int statWidth = sysWidth;
	int statHeight= 20;

	int wWidth  = sysWidth;
	int wHeight = sysHeight + statHeight;
	status.setPosition(sf::Vector2f(5, sysHeight));

	sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "Ising model");

	int k = 0;
	int ensemble = 0;
	int tp = 0;
	while (window.isOpen()) {
		handleEvents(window);
		if (k == RUN) {
			k = 0;
			ensemble++;
			energyOutput << std::endl;
			if (ensemble == SPECS.ENSEMBLE_SIZE) {
				ensemble = 0;
				tp++;
				if (tp == SPECS._t_points)	break;

				energyOutput.close();
				energyOutput.open("energy"+std::to_string(SPECS.Temperature[tp]) + ".csv");

				config = *new Ising(SPECS.Lx, SPECS.Ly,
													SPECS.Temperature[tp],
													BoundaryCondition::PERIODIC);
				continue;
			}
			std::cout << "Generating configuration " << ensemble+1 << std::endl;
			config.generate();
		} else if (k == 0) {
			std::cout << "Generating configuration " << ensemble+1 << std::endl;
			config.generate();
		}

		window.clear();
		config.drawLattice(window, SPECS.scale);
		for (int i = 0; i < BIN; i++)
			dynamics(&config, &SPECS);
		status.setString("t = " + std::to_string(k));
		energyOutput << config.Hamiltonian() << ",";
		window.draw(status);
		window.display();
		k++;
	}

	energyOutput.close();
	return EXIT_SUCCESS;
}
