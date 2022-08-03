#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <exception>

#include <SFML/Graphics.hpp>
#include "defaults.hpp"
// #include "io/draw.hpp"

bool draw;

void handleEvents(sf::RenderWindow &w) {
	sf::Event ev;
	while (w.pollEvent(ev)) {
		switch (ev.type) {
		case sf::Event::Closed:
			w.close();
			break;
		case sf::Event::KeyPressed:
		//	if (ev.key.code == sf::Keyboard::D) draw = !draw;
			if (ev.key.code == sf::Keyboard::Escape)
				w.close();
			break;
		default:
			break;
		}
	}
}

/**
 * @brief Load a font file into the Font object.
 * 
 * @param font 
 */
void getFont(sf::Font* font) {
	bool loaded = font->loadFromFile(FONTS);
	if (!loaded)
		throw std::exception();
}

/**
 * @brief Initialise the `sf::Text` object for the status bar.
 * 
 * @param statusBar The pointer to the sf::Text object to initialise.
 */
void getStatusBar(sf::Text* statusBar, float x, float y) {
	static sf::Font font;
	getFont(&font);
	statusBar->setFont(font);
	statusBar->setString("Hello! I am just sitting here.");
	statusBar->setCharacterSize(TEXT_SIZE);
	statusBar->setFillColor(sf::Color::Yellow);
	statusBar->setStyle(sf::Text::Bold);
	statusBar->setPosition(x, y);
}

/**
 * @brief Get the current status of the simulation.
 * 
 * @param time The integer time step.
 * @param member The ensemble member currently running.
 * @param T The temperature of the simulation.
 * @return std::string 
 */
std::string getStatus(int time, int member, double T) {
	return "t = " + std::to_string(time) + "\t" +
				 "Ensemble = " + std::to_string(member) + "\t" +
				 "Temperature = " + std::to_string(T);
}

bool readNext(std::ifstream& file, bool** grid, const int w, const int h) {
	int N = w * h;
	std::int64_t number;
	int idx = 0;
	int i, j, k;
	while (N > 0) {
		if (N > BUFFER) k = BUFFER - 1;
		else k = N - 1;
		N -= k + 1;
		if (!file.read((char*) &number, (k+1)/4))
			return false;
		for (; k >= 0; k--, idx++) {
			i = idx / w;
			j = idx % w;
			grid[i][j] = (number & (1 << k)) >> k;
		}
	}

	if (i != h-1 && j != w-1) {
		std::cout << "Incomplete file." << std::endl;
		return false;
	}
	return true;
}

int main(int argc, char** argv) {
	std::string filename;
	if (argc > 1) filename = argv[1];
	else {
		std::cout << "No file given." << std::endl;
		return EXIT_FAILURE;
	}
	std::ifstream snap(filename, std::ios::binary);
	if (!snap) {
		std::cout << "Could not open file." << std::endl;
		return EXIT_FAILURE;
	}

	int Lx, Ly;
	int scale;
	snap.read((char*) &Lx, sizeof(int));
	snap.read((char*) &Ly, sizeof(int));
	// snap.read((char*) &scale, sizeof(int));
	scale = 10;

	std::cout << "Width: " << Lx << "\n";
	std::cout << "Height: " << Ly << "\n";

	// read Temp from filename
	// read ensemble # from filename
	std::string temp = "??";
	std::string en   = "??";

	// The width and height of the visualisation area
	int sysWidth  = Lx * scale;
	int sysHeight = Ly * scale;

	// The total window width and height
	int wWidth  = sysWidth;
	int wHeight = sysHeight + STAT_BAR_H;

	sf::Text status;
	try { getStatusBar(&status, 5, sysHeight); }
	catch (std::exception) {
		std::cout << "Could not create status bar object."
							<< " It is likely that the font did not load\n";
		return EXIT_FAILURE;
	}

	std::cout << "Generating lattice..." << std::endl;
	bool** lattice = new bool*[Ly];
	for (int i = 0; i < Ly; i++)
		lattice[i] = (bool*) malloc(Lx * sizeof(bool));

	sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "Ising model");
	std::cout << "Window created" << std::endl;
	while(window.isOpen()) {
		handleEvents(window);
		window.clear();
		if (!readNext(snap, lattice, Lx, Ly)) {
			std::cout << "Terminating." << std::endl;
			return EXIT_FAILURE;
		}
		for (int y = 0; y < Ly; y++) {
			for (int x = 0; x < Lx; x++) {
				sf::RectangleShape sq(sf::Vector2f(scale, scale));
				if (lattice[y][x])
					sq.setFillColor(sf::Color::Black);
				sq.setPosition(x*scale, y*scale);
				window.draw(sq);
			}
		}
		window.display();
	}
	std::cout << "Reading done. Closing file." << std::endl;
	snap.close();

	return EXIT_SUCCESS;
}
