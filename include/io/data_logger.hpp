#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "defaults.hpp"

void openLogger(int w, int h, int en, double T);
void closeLogger();

void logData(double energy, double magnet);
void nextEnsemble();
