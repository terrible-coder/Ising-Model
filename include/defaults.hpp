#pragma once

#include <cstdint>
#include <iostream>
#include <string>

#define FONTS "./assets/font/times new roman.ttf"
#define DATA_EXT ".csv"
#define BIN_EXT ".ism"
#define IMG_EXT ".png"
#define DATA_DIR_PREFIX "data/data"
#define INPUT_FILE "input.txt"
#define SEPARATOR "\n==========================\n"

#define TEXT_SIZE 16
#define STAT_BAR_H 20
#define RUN 15000
#define SKIP 10

#define WORD_SIZE 64
typedef std::uint64_t uWord;
typedef std::uint16_t uIndx;
typedef std::uint32_t uSize;
