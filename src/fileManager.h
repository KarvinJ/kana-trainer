#pragma once

#include <vector>
#include <string>
#include <raylib.h>

//with usings I can remove the need to the of the extra typing string, vector, etc..
using std::vector;
using std::string;

typedef struct
{
    int rank;
    string name;
    int score;
} HighScore;

typedef struct
{
    string name;
    Rectangle bounds;
} TextureInfo;

vector<string> saveInitialHighScores();

string extractLastNChars(string const &str, size_t n);

vector<HighScore> getFullScoreData(vector<string> highScores);

vector<string> saveActualHighScores(vector<string> highScores, int actualScore, string playerName);

vector<string> loadHighScores();

vector<TextureInfo> loadSpriteSheet();

void saveScore(int score);

void savePlayerName(string playerName);

string loadPlayerName();

int loadHighScore();