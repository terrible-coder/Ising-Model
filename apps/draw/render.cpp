#include <iomanip>
#include <fstream>
#include <exception>

#include <SFML/Graphics.hpp>
#include "defaults.hpp"
#include "context.hpp"

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
std::string getStatus(int time, int member, float T) {
	return "t = " + std::to_string(time) + "\t" +
				 "Ensemble = " + std::to_string(member) + "\t" +
				 "Temperature = " + std::to_string(T);
}
std::string getStatus(int time, std::string member, float T) {
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
 * @return float 
 */
float getTemp(std::string name) {
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
bool readNext(std::ifstream& file, uWord* grid, const uIndx w, const uIndx h) {
	uSize N = (uSize)w * h;
	uWord number;
	uIndx idx = 0;
	while (N > 0) {
		if (N < WORD_SIZE) {
			std::cout << "Bad file format." << std::endl;
			return false;
		}
		if (!file.read((char*) &number, sizeof(uWord)))
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

void drawFrame(uWord* grid, uIndx Lx, uIndx Ly, float scale, sf::RenderTexture& target) {
	for (uIndx y = 0; y < Ly; y++) {
		for (uIndx x = 0; x < Lx; x++) {
			sf::RectangleShape sq(sf::Vector2f(scale, scale));
			uSize idx = y * Lx + x;
			uWord number = grid[idx / WORD_SIZE];
			bool spin = (number >> (WORD_SIZE - (idx%WORD_SIZE) - 1)) & 1;
			if ( !spin ) continue;
			sq.setFillColor(sf::Color::White);
			sq.setOutlineThickness(0);
			sq.setPosition(x*scale, y*scale);
			target.draw(sq);
		}
	}
}
void drawFrame(uWord* grid, uIndx Lx, uIndx Ly, float scale, sf::RenderWindow& target) {
	for (uIndx y = 0; y < Ly; y++) {
		for (uIndx x = 0; x < Lx; x++) {
			sf::RectangleShape sq(sf::Vector2f(scale, scale));
			uSize idx = (uSize)Lx * y + x;
			uWord number = grid[idx / WORD_SIZE];
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

enum Mode {
	save, draw
};
enum Source {
	ensemble, initial
};

int main(int argc, char** argv) {
	std::string expName;
	int member;
	Mode disp = Mode::draw;
	Source src = Source::ensemble;
	if (argc < 2) {
		std::cout << "Don't know what to do." << std::endl;
		return EXIT_FAILURE;
	}
	if (argc < 3) {
		std::cout << "No file given." << std::endl;
		return EXIT_FAILURE;
	}
	if (argc < 4) {
		std::cout << "No ensemble specified." << std::endl;
		return EXIT_FAILURE;
	}

	if (std::string(argv[1]) == "save")	disp = Mode::save; else
	if (std::string(argv[1]) != "draw") {
		std::cout << "Unknown mode." << std::endl;
		return EXIT_FAILURE;
	}
	expName = argv[2];
	if (std::string(argv[3]) == "ini")
		src = Source::initial;
	else {
		member = std::stoi(argv[3]);
		src = Source::ensemble;
	}

	if (src == Source::initial) {
		std::string path = expName + "initial" + BIN_EXT;
		std::ifstream iniCon(path);
		std::ofstream frame(expName + "initial" + IMG_EXT);
		uIndx Lx, Ly, Lz;
		iniCon.read((char*) &Lx, sizeof(uIndx));
		iniCon.read((char*) &Ly, sizeof(uIndx));
		iniCon.read((char*) &Lz, sizeof(uIndx));
		float scale = (float)sysWidth / Lx;

		sf::Text statusBar;
		try { getStatusBar(&statusBar, 5, sysHeight); }
		catch (std::exception) {
			std::cout << "Something went wrong." << std::endl;
			return EXIT_FAILURE;
		}
		uWord* lattice = new uWord[Lx*Ly / sizeof(uWord)];
		if (!readNext(iniCon, lattice, Lx, Ly)) {
			std::cout << "Could not read initial frame." << std::endl;
			return EXIT_FAILURE;
		}

		statusBar.setString("Initial condition "
												+ std::to_string(Lx) + "x" + std::to_string(Ly)
												+ "\tTemperature: " + std::to_string(getTemp(expName)));

		int wWidth  = sysWidth;
		int wHeight = sysHeight + STAT_BAR_H;
		sf::RenderTexture texture;
		texture.create(wWidth, wHeight);
		texture.clear();

		drawFrame(lattice, Lx, Ly, scale, texture);
		texture.draw(statusBar);
		texture.display();
		texture.getTexture().copyToImage().saveToFile(expName + "initial" + IMG_EXT);
		delete lattice;
		return EXIT_SUCCESS;
	}

	std::string snapsPath = expName + "snaps/En" + std::to_string(member) + BIN_EXT;
	std::string framePath = expName + "frames/En" + std::to_string(member) + "/fr";

	std::ifstream snap(snapsPath, std::ios::binary);
	std::ofstream frame;
	if (!snap) {
		std::cout << "Could not open file." << std::endl;
		return EXIT_FAILURE;
	}

	uIndx Lx, Ly, Lz;
	snap.read((char*) &Lx, sizeof(uIndx));
	snap.read((char*) &Ly, sizeof(uIndx));
	snap.read((char*) &Lz, sizeof(uIndx));
	float scale = (float)sysWidth / Lx;

	std::cout << "Width: " << Lx << "\n";
	std::cout << "Height: " << Ly << "\n";
	std::cout << "Scale: " << scale << "\n";

	// read Temp from expName
	// read ensemble # from expName
	float temp = getTemp(expName);

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
	uWord* lattice = new uWord[Lx*Ly / sizeof(uWord)];

	sf::RenderWindow window;
	sf::RenderTexture texture;
	if (disp == Mode::draw)	{
		window.create(sf::VideoMode(wWidth, wHeight), "Ising model");
		texture.~RenderTexture();
	}	else {
		texture.create(wWidth, wHeight);
		window.~RenderWindow();
	}

	int t = 0;
	while ((disp == Mode::draw && window.isOpen()) || disp == Mode::save) {
		if (disp == Mode::draw) handleEvents(window);
		if (pause) continue;
		if (!readNext(snap, lattice, Lx, Ly)) {
			if (t == RUN) break;
			std::cout << "Terminating at t = " << t << std::endl;
			return EXIT_FAILURE;
		}

		status.setString(getStatus(++t, member, temp));
		if (disp == Mode::draw) {
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
