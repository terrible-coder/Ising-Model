#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "MC.hpp"

#define WIDTH 50
#define HEIGHT 50
#define SCALE 10
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

int main() {
	srand(SEED);
	// initConfig("input.txt");
	params p = readInput("input.txt");
	initConfig(&p);
	Ising config(&p, WIDTH, HEIGHT, BoundaryCondition::PERIODIC);
	config.generate();

	sf::RenderWindow window(sf::VideoMode(WIDTH*SCALE, HEIGHT*SCALE), "Ising model");

	while (window.isOpen()) {
		handleEvents(window);
		window.clear();
		config.drawLattice(window, SCALE);
		for (int i = 0; i < BIN; i++) {
			int ri = rand() % HEIGHT;
			int rj = rand() % WIDTH;
			spin_flip(&config, ri, rj);
		}
		window.display();
	}

	return 0;
}
