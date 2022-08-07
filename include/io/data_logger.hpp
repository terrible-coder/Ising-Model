#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "defaults.hpp"

void openLogger(std::string parentDir, double T);
void closeLogger();

void logData(double energy, double magnet);
void nextEnsemble();
