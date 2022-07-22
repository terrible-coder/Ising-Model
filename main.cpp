#include <iostream>
#include <fstream>
#include <string>
#include <exception>

#include <SFML/Graphics.hpp>
#include "defaults.hpp"
#include "io/data_logger.hpp"
#include "MC.hpp"

static Specifications SPECS;
bool draw;

void handleEvents(sf::RenderWindow &w) {
	sf::Event ev;
	while (w.pollEvent(ev)) {
		switch (ev.type) {
		case sf::Event::Closed:
			w.close();
			break;
		case sf::Event::KeyPressed:
			if (ev.key.code == sf::Keyboard::D) draw = !draw;
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

int main(int argc, char** argv) {
	std::string filename;
	if (argc > 1) filename = argv[1];
	else          filename = INPUT_FILE;

	std::cout << "Initialising system..." << std::endl;
	init_system(filename, &SPECS);

	const int BIN = SPECS.Lx * SPECS.Ly;

	std::cout << "Setting global values..." << std::endl;
	Ising::setCoupling(SPECS.Coupling);
	Ising::setField(SPECS.Field);

	// The width and height of the visualisation area
	int sysWidth  = SPECS.Lx * SPECS.scale;
	int sysHeight = SPECS.Ly * SPECS.scale;

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

	sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "Ising model");

	for (int tp = 0; tp < SPECS._t_points; tp++) {
		double T = SPECS.Temperature[tp];
		openLogger(SPECS.Lx, SPECS.Ly, SPECS.ENSEMBLE_SIZE, T);

		std::cout << SEPARATOR;

		Ising config(SPECS.Lx, SPECS.Ly, T, BoundaryCondition::PERIODIC);
		config.generate();

		for (int ensemble = 0; ensemble < SPECS.ENSEMBLE_SIZE; ensemble++) {
			std::cout << "Ensemble member " << ensemble+1 << "\n";
			config.reinit();

			for (int k = 0; k < RUN; k++) {
				if (!window.isOpen()) {
					std::cout << "\n\nWindow closed abruptly\nAt ";
					std::cout << "t = " << k;
					std::cout << " ensemble = " << ensemble;
					std::cout << " tp = " << tp << std::endl;
					return EXIT_FAILURE;
				}

				for (int i = 0; i < BIN; i++)
					dynamics(&config, &SPECS);
				logData(config.Hamiltonian() / config.getSize(), config.Magnetisation() / config.getSize());

				handleEvents(window);
				if (draw || k % (SKIP*SKIP) == 0)
					window.clear();
				if (draw)
					config.drawLattice(window, SPECS.scale);
				if (draw || k % (SKIP*SKIP) == 0) {
					std::string text = getStatus(k, ensemble+1, T);
					status.setString(text);
					window.draw(status);
					window.display();
				}
			}
			nextEnsemble();
		}
		closeLogger();
	}

	return EXIT_SUCCESS;
}
