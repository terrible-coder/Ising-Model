#include <filesystem>

#include <SFML/Graphics.hpp>
#include "../Ising.hpp"

void printLattice(Ising* config);
sf::RenderTexture drawLattice(Ising* config, int scale, sf::RenderTarget& rt);
void saveFrame(Ising* config, int count, int ensemble, sf::RenderTexture& rt);
