#include "assetsManager.h"
#include "fileManager.h"
#include <iostream>

#define MAX_INPUT_CHARS 4
// #define MAX_INPUT_CHARS 5 for the web build

const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 544;
const int MAX_GAME_TIME = 60;

void updateHighScore(int &score, int &highScore)
{
    if (score > highScore)
    {
        highScore = score;
        saveScore(score);
        score = 0;
    }
}

void toLowerCase(string &string)
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

    string playerName = loadPlayerName();

    bool isHighScoreScreen = false;

    if (playerName.compare("aaa") == 0)
    {
        isHighScoreScreen = true;
    }

    vector<string> highScores = loadHighScores();

    vector<HighScore> fullScores = getFullScoreData(highScores);

    int attempts = 0;

    // need to explicitly define local variable values, if not I'll get a segmentation fault.
    float soundTimer = 0;
    float showScoreTimer = 5;

    InitAudioDevice();

    Texture2D hiraganaSpriteSheet = LoadTexture("assets/img/hiraganas/hiraganas.png");
    Texture2D katakanaSpriteSheet = LoadTexture("assets/img/katakanas/katakanas.png");

    // vector<Kana> extraKanas = loadAssets();
    vector<TextureInfo> kanas = loadKanas();

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

    // int nextFrameDataOffset = 0;   // Current byte offset to next frame in image.data
    // int currentAnimationFrame = 0; // Current animation frame to load and draw
    // int frameDelay = 8;            // Frame delay to switch between animation frames
    // int animationFrameCounter = 0; // General frames counter

    while (!WindowShouldClose())
    {
        // int kanasInitialIndex = hiraganasInitialIndex;

        if (isHiraganaMode)
        {
            totalKanas = totalHiraganas;
        }
        else
        {
            totalKanas = kanas.size() - 1;
            // kanasInitialIndex = katakanasInitialIndex;
        }

        TextureInfo actualKana = kanas[actualKanaIndex];

        // Kana actualAnimationKana;

        // for (int i = kanasInitialIndex; i < totalKanas + 1; i++)
        // {
        //     auto actualName = handleMissingGifName(actualKana.name);

        //     if (kanas[i].name.compare(actualName) == 0)
        //     {
        //         actualAnimationKana = kanas[i];
        //         break;
        //     }
        // }

        // animationFrameCounter++;

        // if (showKanaAnimation && animationFrameCounter >= frameDelay)
        // {
        //     // Move to next frame. NOTE: If final frame is reached we return to first frame
        //     currentAnimationFrame++;
        //     if (currentAnimationFrame >= actualAnimationKana.animationFrames)
        //         currentAnimationFrame = 0;

        //     // Get memory offset position for next frame data in image.data
        //     nextFrameDataOffset = actualAnimationKana.image.width * actualAnimationKana.image.height * 4 * currentAnimationFrame;

        //     // Update GPU texture data with next frame image data
        //     // WARNING: Data size (frame size) and pixel format must match already created texture
        //     UpdateTexture(actualAnimationKana.animationTexture, ((unsigned char *)actualAnimationKana.image.data) + nextFrameDataOffset);

        //     animationFrameCounter = 0;
        // }

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

        if (!isHighScoreScreen && IsKeyPressed(KEY_ENTER))
        {
            isLearningMode = !isLearningMode;
            showKanaAnimation = false;
            score = 0;
            attempts = 0;
            gameTimer = MAX_GAME_TIME;
            showMessageTimer = 0;
            showMessage = false;

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

        string actualKanaName = answer;

        if (!isLearningMode && !isHighScoreScreen)
        {
            if (gameTimer < 1)
            {
                isLearningMode = true;
                attempts = 0;
                showScoreTimer = 0;

                score *= gameTimer;

                if (score > 0)
                {
                    highScores = saveActualHighScores(highScores, score, playerName);
                    fullScores = getFullScoreData(highScores);
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
                    // PlaySound(actualKana.sound);
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

                    if (score > 0)
                    {
                        highScores = saveActualHighScores(highScores, score, playerName);
                        fullScores = getFullScoreData(highScores);
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
                    // PlaySound(actualKana.sound);
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
                    // TextureInfo nextKana = kanas[actualKanaIndex];
                    // PlaySound(nextKana.sound);
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
                    // TextureInfo nextKana = kanas[actualKanaIndex];
                    // PlaySound(nextKana.sound);
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
                    // TextureInfo nextKana = kanas[actualKanaIndex];
                    // PlaySound(nextKana.sound);
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
                string name = answer;
                name.pop_back();
                toLowerCase(name);

                if (!name.empty())
                {
                    playerName = name;
                    savePlayerName(name);
                }

                answer[0] = '\0';
                letterCount = 0;
            }
        }

        BeginDrawing();

        ClearBackground(Color{29, 29, 27, 255});

        if (!showKanaAnimation && !isHighScoreScreen)
        {
            DrawRectangle(40, 40, 320, 268, WHITE);

            if (isHiraganaMode)
            {
                DrawTextureRec(hiraganaSpriteSheet, actualKana.bounds, {40, 40}, WHITE);
            }
            else
            {
                DrawTextureRec(katakanaSpriteSheet, actualKana.bounds, {40, 40}, WHITE);
            }
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

            // if (showKanaAnimation)
            // {
            //     DrawTexture(actualAnimationKana.animationTexture, GetScreenWidth() / 2 - actualAnimationKana.animationTexture.width / 2, 40, WHITE);
            // }

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

                    TextureInfo previousKana = kanas[previousKanaIndex];
                    DrawText(previousKana.name.c_str(), 235, 495, 25, LIME);
                }

                showMessageTimer += deltaTime;

                if (showMessageTimer > 1.5)
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
            for (auto &fullScore : fullScores)
            {
                DrawText(TextFormat("%i", fullScore.rank), 20, 0 + yPosition, 20, LIGHTGRAY);
                DrawText(fullScore.name.c_str(), 170, 0 + yPosition, 20, LIGHTGRAY);
                DrawText(TextFormat("%i", fullScore.score), 310, 0 + yPosition, 20, LIGHTGRAY);
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

    // int index = 0;
    // for (auto &kana : kanas)
    // {
    //     UnloadTexture(kana.texture);
    //     UnloadTexture(kana.animationTexture);
    //     UnloadImage(kana.image);

    //     // I just have sounds loaded for the hiraganas, and for the katakanas I just copied.
    //     if (index < totalHiraganas)
    //     {
    //         UnloadSound(kana.sound);
    //     }

    //     index++;
    // }

    CloseAudioDevice();
    CloseWindow();
}