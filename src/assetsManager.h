#pragma once

#include <raylib.h>
#include <vector>
#include <string>

typedef struct
{
    std::string name;
    Rectangle bounds;
    Texture2D texture;
    Sound sound;
    Texture2D animationTexture;
    Image image;
    int animationFrames;
} Kana;

std::vector<Kana> loadAssets();

std::string handleMissingGifName(std::string kanaName);
