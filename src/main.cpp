#include <raylib.h>
#include <vector>

const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 544;

bool isGamePaused;

typedef struct
{
    Rectangle bounds;
    Texture2D sprite;
} Kana;

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Starter");
    SetTargetFPS(60);

    Rectangle mouseBounds = {0, 0, 8, 8};

    // need to explicit define local variable values, if not I'll get a segmentation fault.
    int actualKanaIndex = 0;
    int score = 0;

    std::vector<Kana> kanas;
    kanas.reserve(4);

    Texture2D sprite = LoadTexture("assets/img/ka.png");
    Kana kana = {{SCREEN_WIDTH / 3, 50, (float)sprite.width, (float)sprite.height}, sprite};

    Texture2D sprite2 = LoadTexture("assets/img/ma.png");
    Kana kana2 = {{SCREEN_WIDTH / 3, 50, (float)sprite2.width, (float)sprite2.height}, sprite2};

    Texture2D sprite3 = LoadTexture("assets/img/wa.png");
    Kana kana3 = {{SCREEN_WIDTH / 3, 50, (float)sprite3.width, (float)sprite3.height}, sprite3};

    Texture2D sprite4 = LoadTexture("assets/img/to.png");
    Kana kana4 = {{SCREEN_WIDTH / 3, 50, (float)sprite4.width, (float)sprite4.height}, sprite4};

    Texture2D sprite5 = LoadTexture("assets/img/a.png");
    Kana kana5 = {{SCREEN_WIDTH / 3, 50, (float)sprite5.width, (float)sprite5.height}, sprite5};

    kanas.push_back(kana);
    kanas.push_back(kana2);
    kanas.push_back(kana3);
    kanas.push_back(kana4);
    kanas.push_back(kana5);

    InitAudioDevice();

    Sound hitSound = LoadSound("assets/sounds/okay.wav");

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            isGamePaused = !isGamePaused;
            PlaySound(hitSound);
        }

        Vector2 mousePosition = GetMousePosition();

        mouseBounds.x = mousePosition.x;
        mouseBounds.y = mousePosition.y;

        Kana actualKana = kanas[actualKanaIndex];

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionRecs(mouseBounds, actualKana.bounds))
        {
            score++;
            actualKanaIndex++;

            if (actualKanaIndex > 4)
            {
                actualKanaIndex = 0;
            }
        }

        // if (!isGamePaused)
        // {
        // }

        BeginDrawing();

        ClearBackground(Color{29, 29, 27, 255});

        DrawRectangleRounded(actualKana.bounds, 0.3, 6, WHITE);
        DrawTexture(actualKana.sprite, actualKana.bounds.x, actualKana.bounds.y, WHITE);

        DrawText(TextFormat("%i", score), SCREEN_WIDTH / 2, 10, 20, WHITE);

        if (isGamePaused)
        {
            DrawText("Game Paused", 220, 100, 80, WHITE);
        }

        EndDrawing();
    }

    UnloadTexture(kana.sprite);

    UnloadSound(hitSound);
    CloseAudioDevice();

    CloseWindow();
}