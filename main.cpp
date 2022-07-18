#include <iostream>
#include <fstream>
#include <string>
#include <exception>

#include <SFML/Graphics.hpp>
#include "MC.hpp"

#define RUN 5000
#define SEED 15

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
	std::string fontFile = "./font/times new roman.ttf";
	bool loaded = font->loadFromFile(fontFile);
	if (!loaded)
		throw std::exception();
}

/**
 * @brief Initialise the `sf::Text` object for the status bar.
 * 
 * @param statusBar The pointer to the sf::Text object to initialise.
 */
void getStatusBar(sf::Text* statusBar) {
	static sf::Font font;
	getFont(&font);
	statusBar->setFont(font);
	statusBar->setString("Hello! I am just sitting here.");
	statusBar->setCharacterSize(16);
	statusBar->setFillColor(sf::Color::Yellow);
	statusBar->setStyle(sf::Text::Bold);
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

/**
 * @brief Generate the output file name to save the data to.
 * 
 * @param T The temperature of the system.
 * @param energy If the data is for energy. `true` means energy, `false` means magnetisation.
 * @return std::string 
 */
std::string outputFileName(double T, bool energy) {
	const std::string EN = std::to_string(SPECS.ENSEMBLE_SIZE);
	const std::string Lx = std::to_string(SPECS.Lx);
	const std::string Ly = std::to_string(SPECS.Ly);
	std::string folder   = "data" + Lx + "x" + Ly + "en" + EN;
	std::string name     = (energy ? "energy" : "magnet") + std::to_string(T);
	std::string ext      = "csv";
	return folder + "/" + name + "." + ext;
}

int main(int argc, char** argv) {
	std::string filename;
	if (argc > 1) filename = argv[1];
	else          filename = "input.txt";

	srand(SEED);
	std::cout << "Initialising system..." << std::endl;
	init_system(filename, &SPECS);

	const int BIN = SPECS.Lx * SPECS.Ly;
	const std::string SEPARATOR = "\n==========================\n";

	std::ofstream energyOutput;
	energyOutput.open(outputFileName(SPECS.Temperature[0], true));

	std::cout << "Setting global values..." << std::endl;
	Ising::setCoupling(SPECS.Coupling);
	Ising::setField(SPECS.Field);

	std::cout << SEPARATOR;
	Ising config(SPECS.Lx, SPECS.Ly,
							 SPECS.Temperature[0],
							 BoundaryCondition::PERIODIC);
	config.generate();
	std::cout << std::endl;

	// The width and height of the visualisation area
	int sysWidth  = SPECS.Lx * SPECS.scale;
	int sysHeight = SPECS.Ly * SPECS.scale;
	// The width and height of the status bar
	int statWidth = sysWidth;
	int statHeight= 20;

	// The total window width and height
	int wWidth  = sysWidth;
	int wHeight = sysHeight + statHeight;

	sf::Text status;
	try {getStatusBar(&status);}
	catch (std::exception) {return EXIT_FAILURE;}
	status.setPosition(sf::Vector2f(5, sysHeight));

	sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "Ising model");

	// Counter for the Monte Carlo run
	int k = 0;
	// Counter for the ensemble members
	int ensemble = 0;
	// Counter for the temperature values
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
				energyOutput.open(outputFileName(SPECS.Temperature[tp], true));

				std::cout << SEPARATOR;
				config = *new Ising(SPECS.Lx, SPECS.Ly,
													SPECS.Temperature[tp],
													BoundaryCondition::PERIODIC);
				config.generate();
				std::cout << std::endl;
				continue;
			}
			std::cout << "Ensemble member " << ensemble+1 << std::endl;
			config.reinit();
		} else if (k == 0) {
			std::cout << "Ensemble member " << ensemble+1 << std::endl;
			config.reinit();
		}

		if (draw || k%10 == 0)
			window.clear();
		if (draw)
			config.drawLattice(window, SPECS.scale);

		for (int i = 0; i < BIN; i++)
			dynamics(&config, &SPECS);

		if (draw || k%10 == 0) {
			std::string text = getStatus(k, ensemble+1, SPECS.Temperature[tp]);
			status.setString(text);
			window.draw(status);
			window.display();
		}

		energyOutput << config.Hamiltonian() << ",";
		k++;
	}

	energyOutput.close();
	return EXIT_SUCCESS;
}
