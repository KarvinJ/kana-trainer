#pragma once

#include <vector>
#include <fstream>

typedef struct
{
    int rank;
    std::string name;
    int score;
} HighScore;

std::vector<std::string> saveInitialHighScores();

std::string extractLastNChars(std::string const &str, size_t n);

std::vector<HighScore> getFullScoreData(std::vector<std::string> highScores);

std::vector<std::string> saveActualHighScores(std::vector<std::string> highScores, int actualScore, std::string playerName);

std::vector<std::string> loadHighScores();

void saveScore(int score);

void savePlayerName(std::string playerName);

std::string loadPlayerName();

int loadHighScore();