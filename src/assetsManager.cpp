#include "assetsManager.h"

using std::vector;
using std::string;

vector<Kana> loadAssets()
{
    vector<Kana> kanas;
    kanas.reserve(142);

    string audioPath = "assets/sounds/";
    string hiraganaImgsPath = "assets/img/hiraganas/";
    string audioExtension = ".mp3";
    string imageExtension = ".png";
    string hiraganaGifPath = "assets/gifs/hiraganas/";
    string gifExtension = ".gif";

    string kanaNames[] = {
        "a", "e", "i", "o", "u",
        "ka", "ga", "ki", "gi", "ku",
        "gu", "ke", "ge", "ko", "go",
        "sa", "za", "shi", "ji", "su",
        "zu", "se", "ze", "so", "zo",
        "ta", "da", "chi", "di", "tsu",
        "du", "te", "de", "to", "do",
        "na", "ni", "nu", "ne", "no",
        "ha", "ba", "pa", "hi", "bi",
        "pi", "fu", "bu", "pu", "he",
        "be", "pe", "ho", "bo", "po",
        "ma", "mi", "mu", "me", "mo",
        "ya", "yu", "yo",
        "ra", "ri", "ru", "re", "ro",
        "wa", "wo", "n"
    };

    for (string &kanaName : kanaNames)
    {
        string actualAudioPath = audioPath + kanaName + audioExtension;
        Sound actualSound = LoadSound(actualAudioPath.c_str());
        SetSoundVolume(actualSound, 0.8);

        string actualImagePath = hiraganaImgsPath + kanaName + imageExtension;
        Texture2D actualTexture = LoadTexture(actualImagePath.c_str());
        Rectangle kanaBounds = {40, 40, (float)actualTexture.width, (float)actualTexture.height};

        string actualGifPath = hiraganaGifPath + kanaName + gifExtension;

        int animationFrames = 0;
        // Since I'm loading images, the ram consumption will go up.
        //  Load all GIF animation frames into a single Image
        //  NOTE: GIF data is always loaded as RGBA (32bit) by default
        //  NOTE: Frames are just appended one after another in image.data memory
        Image kanaAnimation = LoadImageAnim(actualGifPath.c_str(), &animationFrames);

        // Load texture from image
        // NOTE: We will update this texture when required with next frame data
        // WARNING: It's not recommended to use this technique for sprites animation,
        // use spritesheets instead, like illustrated in textures_sprite_anim example
        Texture2D drawKanaTexture = LoadTextureFromImage(kanaAnimation);

        kanas.push_back({kanaName, kanaBounds, actualTexture, actualSound, drawKanaTexture, kanaAnimation, animationFrames});
    }

    string katakanaImgsPath = "assets/img/katakanas/";
    string katakanaGifPath = "assets/gifs/katakanas/";

    int actualMaxSize = kanas.size();

    for (int i = 0; i < actualMaxSize; i++)
    {
        auto actualKana = kanas[i];

        string actualImagePath = katakanaImgsPath + actualKana.name + imageExtension;
        Texture2D actualTexture = LoadTexture(actualImagePath.c_str());
        Rectangle kanaBounds = {40, 40, (float)actualTexture.width, (float)actualTexture.height};

        string actualGifPath = katakanaGifPath + actualKana.name + gifExtension;

        int animationFrames = 0;
        Image kanaAnimation = LoadImageAnim(actualGifPath.c_str(), &animationFrames);

        Texture2D drawKanaTexture = LoadTextureFromImage(kanaAnimation);

        kanas.push_back({actualKana.name, kanaBounds, actualTexture, actualKana.sound, drawKanaTexture, kanaAnimation, animationFrames});
    }

    return kanas;
}

string handleMissingGifName(string kanaName)
{
    string actualName = kanaName;

    if (actualName.compare("ji") == 0)
    {
        actualName = "shi";
    }

    if (actualName.compare("gi") == 0)
    {
        actualName = "ki";
    }

    else if (actualName.compare("ga") == 0)
    {
        actualName = "ka";
    }

    else if (actualName.compare("ge") == 0)
    {
        actualName = "ke";
    }

    else if (actualName.compare("go") == 0)
    {
        actualName = "ko";
    }

    else if (actualName.compare("gu") == 0)
    {
        actualName = "ku";
    }

    else if (actualName.compare("za") == 0)
    {
        actualName = "sa";
    }

    else if (actualName.compare("ze") == 0)
    {
        actualName = "se";
    }

    else if (actualName.compare("zo") == 0)
    {
        actualName = "so";
    }

    else if (actualName.compare("zu") == 0)
    {
        actualName = "su";
    }

    else if (actualName.compare("da") == 0)
    {
        actualName = "ta";
    }

    else if (actualName.compare("de") == 0)
    {
        actualName = "te";
    }

    else if (actualName.compare("do") == 0)
    {
        actualName = "to";
    }

    else if (actualName.compare("di") == 0)
    {
        actualName = "chi";
    }

    else if (actualName.compare("du") == 0)
    {
        actualName = "tsu";
    }

    else if (actualName.compare("ba") == 0 || actualName.compare("pa") == 0)
    {
        actualName = "ha";
    }

    else if (actualName.compare("bo") == 0 || actualName.compare("po") == 0)
    {
        actualName = "ho";
    }

    else if (actualName.compare("be") == 0 || actualName.compare("pe") == 0)
    {
        actualName = "he";
    }

    else if (actualName.compare("bi") == 0 || actualName.compare("pi") == 0)
    {
        actualName = "hi";
    }

    else if (actualName.compare("bu") == 0 || actualName.compare("pu") == 0)
    {
        actualName = "fu";
    }

    return actualName;
}
