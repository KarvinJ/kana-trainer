#include <raylib.h>
#include <iostream>
#include "fileManager.h"

#define MAX_INPUT_CHARS 4

const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 544;
const int MAX_GAME_TIME = 60;

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

std::vector<Kana> loadAssets()
{
    std::vector<Kana> kanas;
    kanas.reserve(142);

    std::string audioPath = "assets/sounds/";
    std::string hiraganaImgsPath = "assets/img/hiraganas/";
    std::string audioExtension = ".mp3";
    std::string imageExtension = ".png";
    std::string hiraganaGifPath = "assets/gifs/hiraganas/";
    std::string gifExtension = ".gif";

    std::string kanaNames[] = {
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

    for (std::string &kanaName : kanaNames)
    {
        std::string actualAudioPath = audioPath + kanaName + audioExtension;
        Sound actualSound = LoadSound(actualAudioPath.c_str());
        SetSoundVolume(actualSound, 0.8);

        std::string actualImagePath = hiraganaImgsPath + kanaName + imageExtension;
        Texture2D actualTexture = LoadTexture(actualImagePath.c_str());
        Rectangle kanaBounds = {40, 40, (float)actualTexture.width, (float)actualTexture.height};

        std::string actualGifPath = hiraganaGifPath + kanaName + gifExtension;

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

    std::string katakanaImgsPath = "assets/img/katakanas/";
    std::string katakanaGifPath = "assets/gifs/katakanas/";

    int actualMaxSize = kanas.size();

    for (int i = 0; i < actualMaxSize; i++)
    {
        auto actualAnimation = kanas[i];

        std::string actualImagePath = katakanaImgsPath + actualAnimation.name + imageExtension;
        Texture2D actualTexture = LoadTexture(actualImagePath.c_str());
        Rectangle kanaBounds = {40, 40, (float)actualTexture.width, (float)actualTexture.height};

        std::string actualGifPath = katakanaGifPath + actualAnimation.name + gifExtension;

        int animationFrames = 0;
        Image kanaAnimation = LoadImageAnim(actualGifPath.c_str(), &animationFrames);

        Texture2D drawKanaTexture = LoadTextureFromImage(kanaAnimation);

        kanas.push_back({actualAnimation.name, kanaBounds, actualTexture, actualAnimation.sound, drawKanaTexture, kanaAnimation, animationFrames});
    }

    return kanas;
}

void updateHighScore(int &score, int &highScore)
{
    if (score > highScore)
    {
        highScore = score;
        saveScore(score);
        score = 0;
    }
}

// me trae recuerdos de fundamentos de programacion
void toLowerCase(std::string &string)
{
    // Manual converting each character to lowercase using ASCII values
    for (char &character : string)
    {
        if (character >= 'A' && character <= 'Z')
        {
            // Convert uppercase to lowercase by adding 32
            character += 32;
        }
    }
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "KANA-TRAINER");
    SetTargetFPS(60);

    Texture2D soundIconTexture = LoadTexture("assets/icons/sound-icon.png");
    Rectangle soundIconBounds = {SCREEN_WIDTH - 32, 10, (float)soundIconTexture.width, (float)soundIconTexture.height};

    Texture2D muteIconTexture = LoadTexture("assets/icons/mute-icon.png");

    Texture2D checkIconTexture = LoadTexture("assets/icons/check-icon.png");
    Rectangle checkIconBounds = {8, 10, (float)checkIconTexture.width, (float)checkIconTexture.height};

    Texture2D highScoresIconTexture = LoadTexture("assets/icons/high-scores-icon.png");
    Rectangle highScoreIconBounds = {SCREEN_WIDTH - 32, SCREEN_HEIGHT - 32, (float)highScoresIconTexture.width, (float)highScoresIconTexture.height};

    Texture2D backIconTexture = LoadTexture("assets/icons/back-icon.png");
    Rectangle backIconBounds = {8, SCREEN_HEIGHT - 32, (float)highScoresIconTexture.width, (float)highScoresIconTexture.height};

    bool isMute = false;
    bool showKanaAnimation = false;

    int score = 0;
    float gameTimer = MAX_GAME_TIME;

    int highScore = loadHighScore();

    std::string playerName = loadPlayerName();

    bool isHighScoreScreen = false;

    if (playerName.compare("aaa") == 0)
    {
        isHighScoreScreen = true;
    }

    std::vector<std::string> highScores = loadHighScores();

    int attempts = 0;

    // need to explicitly define local variable values, if not I'll get a segmentation fault.
    float soundTimer = 0;
    float showScoreTimer = 5;

    InitAudioDevice();

    std::vector<Kana> kanas = loadAssets();

    // there are 71 hiragana + 71 katakanas = 142.
    int totalKanas = kanas.size() - 1;

    int hiraganasInitialIndex = 0;
    int totalHiraganas = kanas.size() / 2 - 1;

    int katakanasInitialIndex = totalHiraganas + 1;

    bool isLearningMode = true;
    bool isHiraganaMode = true;

    int previousKanaIndex = 0;
    int actualKanaIndex = GetRandomValue(hiraganasInitialIndex, totalHiraganas);

    Rectangle mouseBounds = {0, 0, 8, 8};

    char answer[MAX_INPUT_CHARS] = "\0"; // NOTE: One extra space required for null terminator char '\0'
    int letterCount = 0;

    Rectangle textBoxBounds = {90, 425, 225, 50};

    int textBoxFrameCounter = 0;

    float showMessageTimer = 0;
    bool showMessage = false;
    bool isAnswerCorrect = false;

    int nextFrameDataOffset = 0;   // Current byte offset to next frame in image.data
    int currentAnimationFrame = 0; // Current animation frame to load and draw
    int frameDelay = 8;            // Frame delay to switch between animation frames
    int animationFrameCounter = 0; // General frames counter

    while (!WindowShouldClose())
    {
        if (isHiraganaMode)
        {
            totalKanas = totalHiraganas;
        }
        else
        {
            totalKanas = kanas.size() - 1;
        }

        Kana actualKana = kanas[actualKanaIndex];

        animationFrameCounter++;

        if (showKanaAnimation && animationFrameCounter >= frameDelay)
        {
            // Move to next frame
            // NOTE: If final frame is reached we return to first frame
            currentAnimationFrame++;
            if (currentAnimationFrame >= actualKana.animationFrames)
                currentAnimationFrame = 0;

            // Get memory offset position for next frame data in image.data
            nextFrameDataOffset = actualKana.image.width * actualKana.image.height * 4 * currentAnimationFrame;

            // Update GPU texture data with next frame image data
            // WARNING: Data size (frame size) and pixel format must match already created texture
            UpdateTexture(actualKana.animationTexture, ((unsigned char *)actualKana.image.data) + nextFrameDataOffset);

            animationFrameCounter = 0;
        }

        float deltaTime = GetFrameTime();

        // Get char pressed (unicode character) on the queue
        int character = GetCharPressed();

        // Check if more characters have been pressed on the same frame
        while (character > 0)
        {
            // NOTE: Only allow keys in range [32..125]
            if ((character >= 32) && (character <= 125) && (letterCount < MAX_INPUT_CHARS))
            {
                answer[letterCount] = (char)character;
                answer[letterCount + 1] = '\0'; // Add null terminator at the end of the string.
                letterCount++;
            }

            character = GetCharPressed(); // Check next character in the queue
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {
            letterCount--;

            if (letterCount < 0)
            {
                letterCount = 0;
            }

            answer[letterCount] = '\0';
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            isLearningMode = !isLearningMode;
            showKanaAnimation = false;
            score = 0;
            attempts = 0;
            gameTimer = MAX_GAME_TIME;

            answer[0] = '\0';
            letterCount = 0;

            if (isHiraganaMode)
            {
                actualKanaIndex = GetRandomValue(hiraganasInitialIndex, totalKanas);
            }
            else
            {
                actualKanaIndex = GetRandomValue(katakanasInitialIndex, totalKanas);
            }
        }

        Vector2 mousePosition = GetMousePosition();

        mouseBounds.x = mousePosition.x;
        mouseBounds.y = mousePosition.y;

        std::string actualKanaName = answer;

        if (!isLearningMode && !isHighScoreScreen)
        {
            if (gameTimer < 1)
            {
                isLearningMode = true;
                attempts = 0;
                showScoreTimer = 0;
                
                score *= gameTimer;
                
                if (score > 100)
                {
                    highScores = saveActualHighScores(highScores, score, playerName);
                    updateHighScore(score, highScore);
                    gameTimer = MAX_GAME_TIME;
                }
            }

            if (IsKeyPressed(KEY_SPACE))
            {
                // removing the last character, that is always a blank space.
                actualKanaName.pop_back();
                // always converting to lower case, in case anyone writes in uppercase.
                toLowerCase(actualKanaName);

                if (actualKana.name.compare(actualKanaName) == 0)
                {
                    isAnswerCorrect = true;
                    score++;
                }
                else
                {
                    isAnswerCorrect = false;
                }

                if (!isMute)
                {
                    PlaySound(actualKana.sound);
                }

                showMessage = true;

                // clearing the textbox array.
                answer[0] = '\0';
                letterCount = 0;
                previousKanaIndex = actualKanaIndex;
                if (isHiraganaMode)
                {
                    actualKanaIndex = GetRandomValue(hiraganasInitialIndex, totalKanas);
                }
                else
                {
                    actualKanaIndex = GetRandomValue(katakanasInitialIndex, totalKanas);
                }

                attempts++;

                if (attempts == 20)
                {
                    isLearningMode = true;
                    attempts = 0;
                    showScoreTimer = 0;

                    score *= gameTimer;
                    
                    if (score > 100)
                    {
                        highScores = saveActualHighScores(highScores, score, playerName);
                        updateHighScore(score, highScore);
                        gameTimer = MAX_GAME_TIME;
                    }
                }
            }
        }

        else if (isLearningMode && !isHighScoreScreen)
        {
            if (IsKeyPressed(KEY_SPACE))
            {
                actualKanaName.pop_back();
                toLowerCase(actualKanaName);

                int actualInitialIndex = hiraganasInitialIndex;

                if (!isHiraganaMode)
                {
                    actualInitialIndex = katakanasInitialIndex;
                }

                for (int i = actualInitialIndex; i < totalKanas + 1; i++)
                {
                    if (kanas[i].name.compare(actualKanaName) == 0)
                    {
                        actualKana = kanas[i];
                        actualKanaIndex = i;

                        answer[0] = '\0';
                        letterCount = 0;
                        break;
                    }
                }
            }

            if (!isMute)
            {
                soundTimer += deltaTime;

                if (soundTimer > 0.6 && IsKeyPressed(KEY_SPACE))
                {
                    PlaySound(actualKana.sound);
                    soundTimer = 0;
                }
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mouseBounds, soundIconBounds))
            {
                isMute = !isMute;
            }

            else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mouseBounds, checkIconBounds))
            {
                isHiraganaMode = !isHiraganaMode;

                if (isHiraganaMode)
                {
                    actualKanaIndex = GetRandomValue(hiraganasInitialIndex, totalHiraganas);
                }
                else
                {
                    actualKanaIndex = GetRandomValue(katakanasInitialIndex, kanas.size() - 1);
                }

                if (!isMute)
                {
                    Kana nextKana = kanas[actualKanaIndex];
                    PlaySound(nextKana.sound);
                }
            }

            else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mouseBounds, highScoreIconBounds))
            {
                isHighScoreScreen = true;
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mouseBounds, actualKana.bounds))
            {
                showKanaAnimation = !showKanaAnimation;
            }

            if (IsKeyPressed(KEY_RIGHT))
            {
                actualKanaIndex++;

                if (isHiraganaMode && actualKanaIndex > totalKanas)
                {
                    actualKanaIndex = hiraganasInitialIndex;
                }
                else if (!isHiraganaMode && actualKanaIndex > totalKanas)
                {
                    actualKanaIndex = katakanasInitialIndex;
                }

                if (!isMute)
                {
                    Kana nextKana = kanas[actualKanaIndex];
                    PlaySound(nextKana.sound);
                }
            }

            else if (IsKeyPressed(KEY_LEFT))
            {
                actualKanaIndex--;

                if (isHiraganaMode && actualKanaIndex < hiraganasInitialIndex)
                {
                    actualKanaIndex = totalKanas;
                }
                else if (!isHiraganaMode && actualKanaIndex < katakanasInitialIndex)
                {
                    actualKanaIndex = totalKanas;
                }

                if (!isMute)
                {
                    Kana nextKana = kanas[actualKanaIndex];
                    PlaySound(nextKana.sound);
                }
            }
        }

        if (isHighScoreScreen)
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mouseBounds, backIconBounds))
            {
                isHighScoreScreen = false;
            }

            if (IsKeyPressed(KEY_SPACE))
            {
                std::string name = answer;
                name.pop_back();
                toLowerCase(name);
                playerName = name;

                savePlayerName(name);

                answer[0] = '\0';
                letterCount = 0;
            }
        }

        BeginDrawing();

        ClearBackground(Color{29, 29, 27, 255});

        if (!showKanaAnimation && !isHighScoreScreen)
        {
            DrawRectangleRec(actualKana.bounds, WHITE);
            DrawTexture(actualKana.texture, actualKana.bounds.x, actualKana.bounds.y, WHITE);
        }

        if (isLearningMode && !isHighScoreScreen)
        {
            if (isHiraganaMode)
            {
                DrawText("Hiragana mode", 110, 10, 24, LIGHTGRAY);
            }
            else
            {
                DrawText("katakana mode", 110, 10, 24, LIGHTGRAY);
            }

            DrawRectangleRounded(soundIconBounds, 0.3, 6, LIGHTGRAY);

            if (!isMute)
            {
                DrawTexture(soundIconTexture, soundIconBounds.x, soundIconBounds.y, WHITE);
            }
            else
            {
                DrawTexture(muteIconTexture, soundIconBounds.x, soundIconBounds.y, WHITE);
            }

            DrawRectangleRounded(checkIconBounds, 0.3, 6, LIGHTGRAY);

            if (isHiraganaMode)
            {
                DrawTexture(checkIconTexture, checkIconBounds.x, checkIconBounds.y, WHITE);
            }

            if (showScoreTimer < 5 && highScore > 0)
            {
                DrawText(TextFormat("High score: %i", highScore), 110, 320, 24, LIGHTGRAY);
                showScoreTimer += deltaTime;
            }

            if (showScoreTimer < 5 && score > 0)
            {
                DrawText(TextFormat("Actual score: %i", score), 100, 360, 24, DARKGRAY);
            }

            if (showKanaAnimation)
            {
                DrawTexture(actualKana.animationTexture, GetScreenWidth() / 2 - actualKana.animationTexture.width / 2, 40, WHITE);
            }

            DrawText("SEARCH", 90, 400, 20, LIGHTGRAY);

            DrawRectangleRounded(highScoreIconBounds, 0.3, 6, LIGHTGRAY);
            DrawTexture(highScoresIconTexture, highScoreIconBounds.x, highScoreIconBounds.y, WHITE);
        }

        else if (!isLearningMode && !isHighScoreScreen)
        {
            if (gameTimer > 0)
            {
                gameTimer -= deltaTime;
            }

            DrawText(TextFormat("%i", (int)gameTimer), SCREEN_WIDTH - 40, 10, 24, WHITE);
            DrawText(TextFormat("%i", score), 200, 10, 24, WHITE);
            DrawText(TextFormat("%i", highScore), 20, 10, 24, WHITE);
            DrawRectangle(160, SCREEN_HEIGHT / 2 - 10, 80, 45, WHITE);

            // drawing text box
            DrawText("WRITE THE ANSWER", 90, 400, 20, LIGHTGRAY);

            if (showMessage)
            {
                if (isAnswerCorrect)
                {
                    DrawText("CORRECT!", 145, 500, 20, LIME);
                }
                else
                {
                    DrawText("WRONG!", 145, 500, 20, RED);

                    Kana previousKana = kanas[previousKanaIndex];
                    DrawText(previousKana.name.c_str(), 235, 495, 25, LIME);
                }

                showMessageTimer += deltaTime;

                if (showMessageTimer > 2)
                {
                    showMessageTimer = 0;
                    showMessage = false;
                }
            }
        }

        if (isHighScoreScreen)
        {
            DrawText("Top Players", 120, 10, 24, LIGHTGRAY);

            DrawText("Rank", 20, 40, 24, LIGHTGRAY);
            DrawText("Name", 170, 40, 24, LIGHTGRAY);
            DrawText("Score", 310, 40, 24, LIGHTGRAY);
            int yPosition = 70;

            for (auto &highScore : highScores)
            {
                DrawText(highScore.c_str(), 20, 0 + yPosition, 20, LIGHTGRAY);
                yPosition += 25;
            }

            DrawText("WRITE YOUR NAME", 90, 400, 20, LIGHTGRAY);

            if (!playerName.empty())
            {
                DrawText(("Current Player: " + playerName).c_str(), 90, 500, 20, LIGHTGRAY);
            }

            DrawRectangleRounded(backIconBounds, 0.3, 6, LIGHTGRAY);
            DrawTexture(backIconTexture, backIconBounds.x, backIconBounds.y, WHITE);
        }

        DrawRectangleRec(textBoxBounds, LIGHTGRAY);
        DrawRectangleLines((int)textBoxBounds.x, (int)textBoxBounds.y, (int)textBoxBounds.width, (int)textBoxBounds.height, DARKGRAY);
        DrawText(answer, (int)textBoxBounds.x + 5, (int)textBoxBounds.y + 8, 40, DARKGRAY);

        textBoxFrameCounter++;

        if (letterCount < MAX_INPUT_CHARS)
        {
            // Draw blinking underscore char
            if (((textBoxFrameCounter / 20) % 2) == 0)
            {
                DrawText("_", (int)textBoxBounds.x + 8 + MeasureText(answer, 40), (int)textBoxBounds.y + 12, 40, DARKGRAY);
            }
        }

        EndDrawing();
    }

    UnloadTexture(muteIconTexture);
    UnloadTexture(soundIconTexture);
    UnloadTexture(checkIconTexture);
    UnloadTexture(highScoresIconTexture);
    UnloadTexture(backIconTexture);

    int index = 0;
    for (auto &kana : kanas)
    {
        UnloadTexture(kana.texture);
        UnloadTexture(kana.animationTexture);
        UnloadImage(kana.image);

        // I just have sounds loaded for the hiraganas, and for the katakanas I just copied.
        if (index < totalHiraganas)
        {
            UnloadSound(kana.sound);
        }

        index++;
    }

    CloseAudioDevice();
    CloseWindow();
}