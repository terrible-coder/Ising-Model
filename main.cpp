#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "MC.hpp"

#define WIDTH 100
#define HEIGHT 100
#define SCALE 5

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
	bool** lattice = generate(WIDTH, HEIGHT);

	sf::RenderWindow window(sf::VideoMode(WIDTH*SCALE, HEIGHT*SCALE), "Ising model");

	while (window.isOpen()) {
		handleEvents(window);
		window.clear();
		for (int i = 0; i < HEIGHT; i++) {
			for (int j = 0; j < WIDTH; j++) {
				sf::RectangleShape s(sf::Vector2f(SCALE, SCALE));
				s.setPosition(j*SCALE, i*SCALE);
				if (lattice[i][j]) s.setFillColor(sf::Color::White);
				else s.setFillColor(sf::Color::Black);
				window.draw(s);
			}
		}
		int ri = rand() % HEIGHT;
		int rj = rand() % WIDTH;
		std::cout << ri << ", " << rj << " " << lattice[ri][rj] << std::endl;
		spin_flip(lattice, ri, rj, WIDTH, HEIGHT, p);
		std::cout << ri << ", " << rj << " " << lattice[ri][rj] << std::endl;
		window.display();
	}

	return 0;
}
