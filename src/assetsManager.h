#pragma once

#include <raylib.h>
#include <vector>
#include <string>

using std::vector;
using std::string;

typedef struct
{
    std::string name;
    Sound sound;
    Texture2D animationTexture;
    Image image;
    int animationFrames;
} Kana;

typedef struct
{
    string name;
    Rectangle bounds;
    bool isHiragana;
} TextureInfo;

std::vector<Kana> loadAssets();

vector<string> customSplit(string &str, char separator);

vector<TextureInfo> loadKanas();

std::string handleMissingGifName(std::string kanaName);
