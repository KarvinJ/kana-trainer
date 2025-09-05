#pragma once

#include <raylib.h>
#include <vector>
#include <string>

using std::vector;
using std::string;

typedef struct
{
    std::string name;
    Texture2D animationTexture;
    Image image;
    int animationFrames;
} KanaAnimation;

typedef struct
{
    string name;
    Rectangle bounds;
    Sound sound;
} Kana;

vector<string> customSplit(string &str, char separator);

void loadKanas(vector<Kana> &kanas);