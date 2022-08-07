#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <cstdint>
#include <exception>

#include <SFML/Graphics.hpp>
#include "defaults.hpp"

#define sysWidth 600
#define sysHeight 600

bool pause;

void handleEvents(sf::RenderWindow &w) {
	sf::Event ev;
	while (w.pollEvent(ev)) {
		switch (ev.type) {
		case sf::Event::Closed:
			w.close();
			break;
		case sf::Event::KeyPressed:
			if (ev.key.code == sf::Keyboard::P) pause = !pause;
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
std::string getStatus(int time, std::string member, double T) {
	return "t = " + std::to_string(time) + "\t" +
				 "Ensemble = " + member + "\t" +
				 "Temperature = " + std::to_string(T);
}
std::string getStatus(int time, std::string member, std::string T) {
	return "t = " + std::to_string(time) + "\t" +
				 "Ensemble = " + member + "\t" +
				 "Temperature = " + T;
}

/**
 * @brief Read the temperature from the name of the file.
 * This function works only if the full path to the experiment folder is used.
 * 
 * @param name 
 * @return double 
 */
double getTemp(std::string name) {
	int idxT = name.find("Temp") + 4;
	int c = 0;
	for (int i = 0; i < (int)name.length() - idxT; i++, c++)
		if (name.at(idxT + i) == '/') break;
	return std::stod(name.substr(idxT, c));
}

/**
 * @brief Read the next frame of the lattice.
 * 
 * @param file The binary file.
 * @param grid The boolean lattice to read into.
 * @param w The width of the lattice to be read.
 * @param h The height of the lattice to be read.
 * @return true if the read is successful,
 * @return false if the read did not complete
 */
bool readNext(std::ifstream& file, bool** grid, const int w, const int h) {
	int N = w * h;
	std::uint64_t number;
	int idx = 0;
	int i, j, k;
	while (N > 0) {
		if (N < BUFFER) {
			std::cout << "Bad file format." << std::endl;
			return false;
		}
		if (!file.read((char*) &number, sizeof(std::uint64_t)))
			return false;
		for (k = BUFFER - 1; k >= 0; k--, idx++) {
			i = idx / w;
			j = idx % w;
			std::uint64_t shift = ((std::uint64_t)1) << k;
			grid[i][j] = (number & shift) >> k;
		}
		N -= BUFFER;
	}

	if (idx != w*h) std::cout << "Bad read" << std::endl;
	return true;
}

std::string frameName(int n, const int maxLen) {
	std::string fno = std::to_string(n);
	int d = maxLen - fno.length();
	for (int i = 0; i < d; i++)
		fno = "0" + fno;
	return fno;
}

void print_progress(int p, int total, int width = 80) {
	std::string total_str = std::to_string(total);
	std::string p_str = std::to_string(p);
	int bar_width = width - total_str.size()*2 - 4;

	std::cout << '\r';
	if(bar_width > 1) {
		int fill_width = bar_width * p / total;
		std::cout << "[";
		for(int i = 0; i < bar_width; ++i) {
			char c = ' ';
			if(i < fill_width || p == total) c = '=';
			else if(i == fill_width) c = '>';

			std::cout << c;
		}
		std::cout << "] ";
	}
	std::cout << std::setfill(' ') << std::setw(total_str.size())
		<< p_str << "/" << total_str << std::flush;
	if(p == total) std::cout << std::endl;
}

int main(int argc, char** argv) {
	std::string exT;
	std::string member;
	if (argc < 2) {
		std::cout << "No file given." << std::endl;
		return EXIT_FAILURE;
	} else if (argc < 3) {
		std::cout << "Ensemble not specified." << std::endl;
		return EXIT_FAILURE;
	}
	exT = argv[1];
	member = argv[2];

	std::string snapsPath = exT + "snaps/En" + member + BIN_EXT;
	std::string framePath = exT + "frames/En" + member + "/fr";

	std::ifstream snap(snapsPath, std::ios::binary);
	std::ofstream frame;
	if (!snap) {
		std::cout << "Could not open file." << std::endl;
		return EXIT_FAILURE;
	}

	int Lx, Ly;
	snap.read((char*) &Lx, sizeof(int));
	snap.read((char*) &Ly, sizeof(int));
	float scale = (float)sysWidth / Lx;

	std::cout << "Width: " << Lx << "\n";
	std::cout << "Height: " << Ly << "\n";
	std::cout << "Scale: " << scale << "\n";

	// read Temp from exT
	// read ensemble # from exT
	double temp = getTemp(exT);
	// int en   = std::stoi(member);

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

	// sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "Ising model");
	sf::RenderTexture texture;
	texture.create(wWidth, wHeight);

	int t = 0;
	// while(window.isOpen()) {
	while (true) {
		// handleEvents(window);
		if (pause) continue;
		// window.clear();
		texture.clear();
		if (!readNext(snap, lattice, Lx, Ly)) {
			if (t == RUN) break;
			std::cout << "Terminating at t = " << t << std::endl;
			return EXIT_FAILURE;
		}
		for (int y = 0; y < Ly; y++) {
			for (int x = 0; x < Lx; x++) {
				sf::RectangleShape sq(sf::Vector2f(scale, scale));
				if (!lattice[y][x]) continue;
				sq.setFillColor(sf::Color::White);
				sq.setPosition(x*scale, y*scale);
				texture.draw(sq);
			}
		}
		status.setString(getStatus(++t, member, temp));
		texture.draw(status);
		texture.display();
		sf::Texture img = texture.getTexture();
		img.copyToImage().saveToFile(framePath+frameName(t, 4)+IMG_EXT);
		print_progress(t, RUN);
		// window.draw(sf::Sprite(img));
		// window.display();
	}
	std::cout << "\nReading done. Closing file." << std::endl;
	snap.close();

	return EXIT_SUCCESS;
}
