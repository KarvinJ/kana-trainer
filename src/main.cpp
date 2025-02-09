#include <raylib.h>
#include <vector>
#include <iostream>

const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 544;

typedef struct
{
    Rectangle bounds;
    Texture2D texture;
    Sound sound;
} Kana;

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Starter");
    SetTargetFPS(60);

    // bool isGamePaused = false;
    bool isPracticeMode = false;

    Rectangle mouseBounds = {0, 0, 8, 8};

    // need to explicit define local variable values, if not I'll get a segmentation fault.
    int actualKanaIndex = 0;
    int score = 0;
    float soundTimer = 0;
    
    std::vector<Kana> kanas;
    kanas.reserve(5);

    InitAudioDevice();

    std::string baseAudioPath = "assets/sounds/";
    std::string baseImagePath = "assets/img/";
    std::string audioExtension = ".mp3";
    std::string imageExtension = ".png";

    std::vector<std::string> kanaNames = {"ka", "ma", "wa", "to", "a"};

    for (std::string &kanaName : kanaNames)
    {
        std::string actualAudioPath = baseAudioPath + kanaName + audioExtension;
        Sound actualSound = LoadSound(actualAudioPath.c_str());
        SetSoundVolume(actualSound, 0.4f);

        std::string actualImagePath = baseImagePath + kanaName + imageExtension;

        Texture2D actualTexture = LoadTexture(actualImagePath.c_str());
        Rectangle kanaBounds = {SCREEN_WIDTH / 3, 50, (float)actualTexture.width, (float)actualTexture.height};

        kanas.push_back({kanaBounds, actualTexture, actualSound});
    }

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            isPracticeMode = !isPracticeMode;
        }

        Vector2 mousePosition = GetMousePosition();

        mouseBounds.x = mousePosition.x;
        mouseBounds.y = mousePosition.y;

        Kana actualKana = kanas[actualKanaIndex];

        bool hasCollision = CheckCollisionRecs(mouseBounds, actualKana.bounds);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hasCollision)
        {
            score++;
            actualKanaIndex++;
            // actualKanaIndex = GetRandomValue(0, 4);

            if (actualKanaIndex > 4)
            {
                actualKanaIndex = 0;
            }
        }

        soundTimer += GetFrameTime();

        if (!isPracticeMode && soundTimer > 0.6 && (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && hasCollision))
        {
            PlaySound(actualKana.sound);
            soundTimer = 0;
        }

        BeginDrawing();

        ClearBackground(BLACK);

        DrawRectangleRounded({280, 0, 400, SCREEN_HEIGHT}, 0.2, 6, Color{29, 29, 27, 255});

        DrawRectangleRounded(actualKana.bounds, 0.3, 6, WHITE);
        DrawTexture(actualKana.texture, actualKana.bounds.x, actualKana.bounds.y, WHITE);

        if (isPracticeMode)
        {
            DrawText("Practice Mode", 400, 10, 20, WHITE);
            DrawRectangle(SCREEN_WIDTH / 2 - 40, SCREEN_HEIGHT / 2, 70, 40, WHITE);
        }
        else
        {
            DrawText(TextFormat("%i", score), SCREEN_WIDTH / 2, 10, 20, WHITE);
        }

        EndDrawing();
    }

    // UnloadTexture(kana.texture);
    CloseAudioDevice();
    CloseWindow();
}