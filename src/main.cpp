#include <raylib.h>
#include <vector>
#include <iostream>
#include <fstream>

#define MAX_INPUT_CHARS 4

const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 544;
const int MAX_GAME_TIME = 60;

float gameTimer = MAX_GAME_TIME;

int score = 0;
int highScore = 0;

typedef struct
{
    std::string name;
    Rectangle bounds;
    Texture2D texture;
    Sound sound;
} Kana;

std::vector<Kana> loadAssets()
{
    std::vector<Kana> kanas;
    kanas.reserve(71);

    std::string baseAudioPath = "assets/sounds/";
    std::string baseImagePath = "assets/img/";
    std::string audioExtension = ".mp3";
    std::string imageExtension = ".png";

    std::vector<std::string> kanaNames = {
        "a", "e", "i", "o", "u",
        "ka", "ga", "ki", "gi", "ku",
        "gu", "ke", "ge", "ko", "go",
        "sa", "za", "shi", "ji", "su",
        "zu", "se", "ze", "so", "zo",
        "ta", "da", "chi", "ji2", "tsu",
        "zu2", "te", "de", "to", "do",
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
        std::string actualAudioPath = baseAudioPath + kanaName + audioExtension;
        Sound actualSound = LoadSound(actualAudioPath.c_str());
        SetSoundVolume(actualSound, 0.8);

        std::string actualImagePath = baseImagePath + kanaName + imageExtension;
        Texture2D actualTexture = LoadTexture(actualImagePath.c_str());
        Rectangle kanaBounds = {40, 50, (float)actualTexture.width, (float)actualTexture.height};

        kanas.push_back({kanaName, kanaBounds, actualTexture, actualSound});
    }

    return kanas;
}

void saveScore()
{
    std::ofstream highScores("assets/high-score.txt");

    std::string scoreString = std::to_string(score);

    highScores << scoreString;

    highScores.close();
}

int loadHighScore()
{
    std::string highScoreText;

    std::ifstream highScores("assets/high-score.txt");

    if (!highScores.is_open())
    {
        saveScore();

        std::ifstream auxHighScores("assets/high-score.txt");

        getline(auxHighScores, highScoreText);

        highScores.close();

        int highScore = stoi(highScoreText);

        return highScore;
    }

    getline(highScores, highScoreText);

    highScores.close();

    int highScore = stoi(highScoreText);

    return highScore;
}

void resetGame()
{
    score *= gameTimer;

    if (score > highScore)
    {
        highScore = score;
        saveScore();

        gameTimer = MAX_GAME_TIME;
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
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jap-Tester");
    SetTargetFPS(60);

    highScore = loadHighScore();

    int attempts = 0;

    // need to explicitly define local variable values, if not I'll get a segmentation fault.
    float soundTimer = 0;

    InitAudioDevice();

    std::vector<Kana> kanas = loadAssets();

    int totalKanas = kanas.size() - 1;

    int actualKanaIndex = GetRandomValue(0, totalKanas);

    bool isLearningMode = true;

    if (isLearningMode)
    {
        PlaySound(kanas[actualKanaIndex].sound);
    }

    Rectangle mouseBounds = {0, 0, 8, 8};

    char answer[MAX_INPUT_CHARS] = "\0"; // NOTE: One extra space required for null terminator char '\0'
    int letterCount = 0;

    Rectangle textBoxBounds = {90, 400, 225, 50};

    int framesCounter = 0;

    float showMessageTimer = 0;
    bool showMessage = false;
    bool isAnswerCorrect = false;

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        Kana actualKana = kanas[actualKanaIndex];

        if (IsKeyPressed(KEY_F1))
        {
            isLearningMode = !isLearningMode;
            score = 0;
            attempts = 0;
            gameTimer = MAX_GAME_TIME;
        }

        if (!isLearningMode)
        {
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

            if (gameTimer < 1)
            {
                isLearningMode = true;
                attempts = 0;
                resetGame();
            }

            std::string actualKanaName = answer;

            if (IsKeyPressed(KEY_SPACE))
            {
                // removing the last character, that is always a blank space.
                actualKanaName.pop_back();
                // always converting to lower case, in case anyone writes in uppercase.
                toLowerCase(actualKanaName);

                if (actualKana.name.compare(actualKanaName) == 0)
                {
                    isAnswerCorrect = true;
                    // clearing the array.
                    answer[0] = '\0';
                    letterCount = 0;
                    actualKanaIndex = GetRandomValue(0, totalKanas);
                    score++;
                    PlaySound(actualKana.sound);
                }
                else
                {
                    isAnswerCorrect = false;
                    answer[0] = '\0';
                    letterCount = 0;
                    actualKanaIndex = GetRandomValue(0, totalKanas);
                    PlaySound(actualKana.sound);
                }

                showMessage = true;

                attempts++;
                if (attempts == 20)
                {
                    isLearningMode = true;
                    attempts = 0;
                    resetGame();
                }
            }
        }

        else
        {

            Vector2 mousePosition = GetMousePosition();

            mouseBounds.x = mousePosition.x;
            mouseBounds.y = mousePosition.y;

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mouseBounds, actualKana.bounds))
            {
                actualKanaIndex++;

                if (actualKanaIndex > totalKanas)
                {
                    actualKanaIndex = 0;
                }

                Kana nextKana = kanas[actualKanaIndex];
                PlaySound(nextKana.sound);
            }

            if (IsKeyPressed(KEY_RIGHT))
            {
                actualKanaIndex++;

                if (actualKanaIndex > totalKanas)
                {
                    actualKanaIndex = 0;
                }

                Kana nextKana = kanas[actualKanaIndex];
                PlaySound(nextKana.sound);
            }

            else if (IsKeyPressed(KEY_LEFT))
            {
                actualKanaIndex--;

                if (actualKanaIndex < 0)
                {
                    actualKanaIndex = totalKanas;
                }

                Kana nextKana = kanas[actualKanaIndex];
                PlaySound(nextKana.sound);
            }

            soundTimer += deltaTime;

            if (soundTimer > 0.6 && IsKeyPressed(KEY_SPACE))
            {
                PlaySound(actualKana.sound);
                soundTimer = 0;
            }
        }

        BeginDrawing();

        ClearBackground(Color{29, 29, 27, 255});

        DrawRectangleRounded(actualKana.bounds, 0.3, 6, WHITE);

        DrawTexture(actualKana.texture, actualKana.bounds.x, actualKana.bounds.y, WHITE);

        if (isLearningMode)
        {
            DrawText("Learning Mode", 110, 10, 24, WHITE);
        }
        else
        {
            if (gameTimer > 0)
            {
                gameTimer -= deltaTime;
            }

            DrawText(TextFormat("%i", (int)gameTimer), SCREEN_WIDTH - 40, 10, 24, WHITE);
            DrawText(TextFormat("%i", score), 200, 10, 24, WHITE);
            DrawText(TextFormat("%i", highScore), 20, 10, 24, WHITE);
            DrawRectangle(160, SCREEN_HEIGHT / 2, 70, 40, WHITE);

            // drawing text box
            DrawText("WRITE THE ANSWER", 90, 375, 20, LIGHTGRAY);

            DrawRectangleRec(textBoxBounds, LIGHTGRAY);

            DrawRectangleLines((int)textBoxBounds.x, (int)textBoxBounds.y, (int)textBoxBounds.width, (int)textBoxBounds.height, DARKGRAY);

            DrawText(answer, (int)textBoxBounds.x + 5, (int)textBoxBounds.y + 8, 40, DARKGRAY);

            if (showMessage)
            {
                if (isAnswerCorrect)
                {
                    DrawText("CORRECT!", 145, 470, 20, LIME);
                }
                else
                {
                    DrawText("WRONG!", 160, 470, 20, RED);
                }

                showMessageTimer += deltaTime;

                if (showMessageTimer > 1)
                {
                    showMessageTimer = 0;
                    showMessage = false;
                }
            }

            framesCounter++;

            if (letterCount < MAX_INPUT_CHARS)
            {
                // Draw blinking underscore char
                if (((framesCounter / 20) % 2) == 0)
                {
                    DrawText("_", (int)textBoxBounds.x + 8 + MeasureText(answer, 40), (int)textBoxBounds.y + 12, 40, DARKGRAY);
                }
            }
        }

        EndDrawing();
    }

    for (auto kana : kanas)
    {
        UnloadTexture(kana.texture);
        UnloadSound(kana.sound);
    }

    CloseAudioDevice();
    CloseWindow();
}
