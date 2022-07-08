#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "MC.hpp"

#define BIN 1000
#define RUN 1000
#define SEED 15

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

	srand(SEED);
	params p = readInput(filename);

	initConfig(&p);
	Ising config(&p);
	config.generate();

	// double energy[RUN/2];
	// double magnet[RUN/2];
	// std::ofstream energy_data("energy.tsv");
	// std::ofstream magnet_data("magnet.tsv");
	// if (!energy_data.is_open() || !magnet_data.is_open()) {
	// 	return EXIT_FAILURE;
	// }

	int wWidth = p.width * p.scale;
	int wHeight = p.height * p.scale;

	sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "Ising model");

	// int k = 0;

	while (window.isOpen() && p.T < 4) {
		// if (k == RUN) {
		// 	for (int i = 0; i < RUN/2; i++) {
		// 		energy_data << energy[i] / config.getSize() << "\t";
		// 		magnet_data << magnet[i] / config.getSize() << "\t";
		// 	}
		// 	energy_data << std::endl;
		// 	magnet_data << std::endl;

		// 	p.T += 0.01;
		// 	k = 0;
		// 	std::cout << "Restarting for T = " << p.T << std::endl;

		// 	initConfig(&p);
		// 	config = *(new Ising(&p));
		// 	config.generate();
		// }

		handleEvents(window);
		window.clear();
		config.drawLattice(window);
		for (int i = 0; i < BIN; i++)
			dynamics(&config);
		// if (k >= RUN / 2) {
		// 	energy[k-RUN/2] = config.Hamiltonian();
		// 	magnet[k-RUN/2] = config.Magnetisation();
		// }
		// k++;
		window.display();
	}
	// energy_data.close();
	// magnet_data.close();

	return 0;
}