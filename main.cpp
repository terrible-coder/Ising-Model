#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "MC.hpp"

// #define WIDTH 50
// #define HEIGHT 50
// #define SCALE 10
#define BIN 1000
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

	int wWidth = p.width * p.scale;
	int wHeight = p.height * p.scale;

	sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "Ising model");

	while (window.isOpen()) {
		handleEvents(window);
		window.clear();
		config.drawLattice(window);
		for (int i = 0; i < BIN; i++) {
			int ri = rand() % config.getParams()->height;
			int rj = rand() % config.getParams()->width;
			spin_flip(&config, ri, rj);
		}
		window.display();
	}

	return 0;
}
