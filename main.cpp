#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "MC.hpp"

#define WIDTH 50
#define HEIGHT 50
#define SCALE 10

void handleEvents(sf::RenderWindow &w) {
	sf::Event ev;
	while (w.pollEvent(ev)) {
		switch (ev.type)
		{
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
	srand(15);
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
		int ri = rand() % HEIGHT;
		int rj = rand() % WIDTH;
		std::cout << ri << ", " << rj << " " << config(ri, rj) << std::endl;
		spin_flip(&config, ri, rj);
		std::cout << ri << ", " << rj << " " << config(ri, rj) << std::endl;
		window.display();
	}

	return 0;
}
