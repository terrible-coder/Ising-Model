#include <iostream>
#include <fstream>
#include <string>
#include <exception>

#include <SFML/Graphics.hpp>
#include "defaults.hpp"
#include "io/data_logger.hpp"
#include "MC.hpp"

static Context CTX;
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
	init_system(filename, &CTX);

	const int BIN = CTX.Lx * CTX.Ly;

	std::cout << "Setting global values..." << std::endl;
	Ising::setCoupling(CTX.Coupling);
	Ising::setField(CTX.Field);

	// The width and height of the visualisation area
	int sysWidth  = CTX.Lx * CTX.scale;
	int sysHeight = CTX.Ly * CTX.scale;

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
	sf::RenderTexture texture;
	texture.create(wWidth, wHeight);

	for (int tp = 0; tp < CTX._t_points; tp++) {
		double T = CTX.Temperature[tp];
		openLogger(CTX.Lx, CTX.Ly, CTX.ENSEMBLE_SIZE, T);

		std::cout << SEPARATOR;

		Ising config(CTX.Lx, CTX.Ly, T, BoundaryCondition::PERIODIC);
		config.generate();

		for (int ensemble = 0; ensemble < CTX.ENSEMBLE_SIZE; ensemble++) {
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
					dynamics(&config, &CTX);
				logData(config.Hamiltonian() / config.getSize(), config.Magnetisation() / config.getSize());

				handleEvents(window);
				if (k % SKIP == 0) {
					texture.clear();
					config.drawLattice(texture, CTX.scale);
					std::string text = getStatus(k, ensemble+1, T);
					status.setString(text);
					texture.draw(status);
					config.saveFrame(texture, k, ensemble);
					texture.display();
				}

				if (draw && k % (SKIP*SKIP) == 0) {
					window.clear();
					sf::Sprite sprite(texture.getTexture());
					window.draw(sprite);
					window.display();
				}
			}
			nextEnsemble();
		}
		closeLogger();
	}

	return EXIT_SUCCESS;
}
