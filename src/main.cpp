#include <raylib.h>
#include <vector>

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
    kanas.reserve(4);

    InitAudioDevice();

    Sound kaSound = LoadSound("assets/sounds/ka.mp3");
    Sound maSound = LoadSound("assets/sounds/ma.mp3");
    Sound waSound = LoadSound("assets/sounds/wa.mp3");
    Sound toSound = LoadSound("assets/sounds/to.mp3");
    Sound aSound = LoadSound("assets/sounds/a.mp3");

    SetSoundVolume(kaSound, 0.4f);
    SetSoundVolume(maSound, 0.4f);
    SetSoundVolume(waSound, 0.4f);
    SetSoundVolume(toSound, 0.4f);
    SetSoundVolume(aSound, 0.4f);

    Texture2D texture = LoadTexture("assets/img/ka.png");
    Rectangle kanaBounds = {SCREEN_WIDTH / 3, 50, (float)texture.width, (float)texture.height};

    Kana kana = {kanaBounds, texture, kaSound};

    Texture2D texture2 = LoadTexture("assets/img/ma.png");
    Kana kana2 = {kanaBounds, texture2, maSound};

    Texture2D sprite3 = LoadTexture("assets/img/wa.png");
    Kana kana3 = {kanaBounds, sprite3, waSound};

    Texture2D sprite4 = LoadTexture("assets/img/to.png");
    Kana kana4 = {kanaBounds, sprite4, toSound};

    Texture2D sprite5 = LoadTexture("assets/img/a.png");
    Kana kana5 = {kanaBounds, sprite5, aSound};

    kanas.push_back(kana);
    kanas.push_back(kana2);
    kanas.push_back(kana3);
    kanas.push_back(kana4);
    kanas.push_back(kana5);

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

            if (actualKanaIndex > 4)
            {
                actualKanaIndex = 0;
            }
        }

        soundTimer += GetFrameTime();

        if (!isPracticeMode && soundTimer > 0.8 && (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && hasCollision))
        {
            PlaySound(actualKana.sound);
            soundTimer = 0;
        }

        BeginDrawing();

        ClearBackground(Color{29, 29, 27, 255});

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

    UnloadTexture(kana.texture);
    CloseAudioDevice();
    CloseWindow();
}