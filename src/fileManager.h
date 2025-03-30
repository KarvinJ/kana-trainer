#pragma once

#include <vector>
#include <fstream>

std::vector<std::string> saveInitialHighScores();

std::string extractLastNChars(std::string const &str, size_t n);

std::vector<std::string> saveActualHighScores(std::vector<std::string> highScores, int actualScore, std::string playerName);

std::vector<std::string> loadHighScores();

void saveScore(int score);

void savePlayerName(std::string playerName);

std::string loadPlayerName();

int loadHighScore();