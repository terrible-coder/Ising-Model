#include <iomanip>
#include <fstream>
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
bool readNext(std::ifstream& file, uWord_t* grid, const int w, const int h) {
	int N = w * h;
	uWord_t number;
	int idx = 0;
	while (N > 0) {
		if (N < WORD_SIZE) {
			std::cout << "Bad file format." << std::endl;
			return false;
		}
		if (!file.read((char*) &number, sizeof(uWord_t)))
			return false;
		grid[idx++] = number;
		N -= WORD_SIZE;
	}

	if (idx != w*h/64) std::cout << "Bad read" << std::endl;
	return true;
}

std::string frameName(int n, const int maxLen) {
	std::string fno = std::to_string(n);
	int d = maxLen - fno.length();
	for (int i = 0; i < d; i++)
		fno = "0" + fno;
	return fno;
}

void drawFrame(uWord_t* grid, std::uint16_t Lx, std::uint16_t Ly, float scale, sf::RenderTexture& target) {
	for (uint y = 0; y < Ly; y++) {
		for (uint x = 0; x < Lx; x++) {
			sf::RectangleShape sq(sf::Vector2f(scale, scale));
			uint idx = y * Lx + x;
			uWord_t number = grid[idx / WORD_SIZE];
			bool spin = (number >> (WORD_SIZE - (idx%WORD_SIZE) - 1)) & 1;
			if ( !spin ) continue;
			sq.setFillColor(sf::Color::White);
			sq.setOutlineThickness(0);
			sq.setPosition(x*scale, y*scale);
			target.draw(sq);
		}
	}
}
void drawFrame(uWord_t* grid, std::uint16_t Lx, std::uint16_t Ly, float scale, sf::RenderWindow& target) {
	for (uint y = 0; y < Ly; y++) {
		for (uint x = 0; x < Lx; x++) {
			sf::RectangleShape sq(sf::Vector2f(scale, scale));
			uint idx = y * Lx + x;
			uWord_t number = grid[idx / WORD_SIZE];
			bool spin = (number >> (WORD_SIZE - (idx%WORD_SIZE) - 1)) & 1;
			if ( !spin ) continue;
			sq.setFillColor(sf::Color::White);
			sq.setOutlineThickness(0);
			sq.setPosition(x*scale, y*scale);
			target.draw(sq);
		}
	}
}

/**
 * @brief Print the progress of a process. The function has been taken from
 * https://gist.github.com/juliusikkala/946f505656ed3c35f6c2741f29f26080
 * 
 * @param p 
 * @param total 
 * @param width 
 */
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
	std::string dispMode = "draw";
	std::string source = "en";
	if (argc < 2) {
		std::cout << "No file given." << std::endl;
		return EXIT_FAILURE;
	} else if (argc < 3) {
		std::cout << "Ensemble not specified." << std::endl;
		return EXIT_FAILURE;
	}
	exT = argv[1];
	member = argv[2];

	// true is for draw, false is for save
	bool mode = true;
	if (argc > 3) {
		dispMode = argv[3];
		if (dispMode != "save" && dispMode != "draw") {
			std::cout << "Unknown mode." << std::endl;
		} else
		mode = dispMode == "draw";
		if (argc > 4) {
			source = argv[4];
			if (source != "en" && source != "ini") {
				std::cout << "Unknown source." << std::endl;
				source = "ini";
			}
		}
	}

	if (source == "ini") {
		std::string path = exT + "initial" + BIN_EXT;
		std::ifstream iniCon(path);
		std::ofstream frame(exT + "initial" + IMG_EXT);
		std::uint16_t Lx, Ly;
		iniCon.read((char*) &Lx, sizeof(std::uint16_t));
		iniCon.read((char*) &Ly, sizeof(std::uint16_t));
		float scale = (float)sysWidth / Lx;

		sf::Text statusBar;
		try { getStatusBar(&statusBar, 5, sysHeight); }
		catch (std::exception) {
			std::cout << "Something went wrong." << std::endl;
			return EXIT_FAILURE;
		}
		uWord_t* lattice = new uWord_t[Lx*Ly / sizeof(uWord_t)];
		if (!readNext(iniCon, lattice, Lx, Ly)) {
			std::cout << "Could not read initial frame." << std::endl;
			return EXIT_FAILURE;
		}

		statusBar.setString("Initial condition "
												+ std::to_string(Lx) + "x" + std::to_string(Ly)
												+ "\tTemperature: " + std::to_string(getTemp(exT)));

		int wWidth  = sysWidth;
		int wHeight = sysHeight + STAT_BAR_H;
		sf::RenderTexture texture;
		texture.create(wWidth, wHeight);
		texture.clear();

		drawFrame(lattice, Lx, Ly, scale, texture);
		texture.draw(statusBar);
		texture.display();
		texture.getTexture().copyToImage().saveToFile(exT + "initial" + IMG_EXT);
		delete lattice;
		return EXIT_SUCCESS;
	}

	std::string snapsPath = exT + "snaps/En" + member + BIN_EXT;
	std::string framePath = exT + "frames/En" + member + "/fr";

	std::ifstream snap(snapsPath, std::ios::binary);
	std::ofstream frame;
	if (!snap) {
		std::cout << "Could not open file." << std::endl;
		return EXIT_FAILURE;
	}

	std::uint16_t Lx, Ly;
	snap.read((char*) &Lx, sizeof(std::uint16_t));
	snap.read((char*) &Ly, sizeof(std::uint16_t));
	float scale = (float)sysWidth / Lx;

	std::cout << "Width: " << Lx << "\n";
	std::cout << "Height: " << Ly << "\n";
	std::cout << "Scale: " << scale << "\n";

	// read Temp from exT
	// read ensemble # from exT
	double temp = getTemp(exT);

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
	uWord_t* lattice = new uWord_t[Lx*Ly / sizeof(uWord_t)];

	sf::RenderWindow window;
	sf::RenderTexture texture;
	if (mode)	{
		window.create(sf::VideoMode(wWidth, wHeight), "Ising model");
		texture.~RenderTexture();
	}	else {
		texture.create(wWidth, wHeight);
		window.~RenderWindow();
	}

	int t = 0;
	while ((mode && window.isOpen()) || !mode) { 
		if (mode) handleEvents(window);
		if (pause) continue;
		if (!readNext(snap, lattice, Lx, Ly)) {
			if (t == RUN) break;
			std::cout << "Terminating at t = " << t << std::endl;
			return EXIT_FAILURE;
		}

		status.setString(getStatus(++t, member, temp));
		if (mode) {
			window.clear();
			drawFrame(lattice, Lx, Ly, scale, window);
			window.draw(status);
			window.display();
		} else {
			texture.clear();
			drawFrame(lattice, Lx, Ly, scale, texture);
			texture.draw(status);
			texture.getTexture().copyToImage().saveToFile(framePath+frameName(t, 4)+IMG_EXT);
		}
		print_progress(t, RUN);
	}
	std::cout << "\nReading done. Closing file." << std::endl;
	snap.close();

	delete lattice;
	return EXIT_SUCCESS;
}
