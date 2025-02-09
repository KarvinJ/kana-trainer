#include <raylib.h>

const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 544;

typedef struct
{
    Rectangle bounds;
    Texture2D sprite;
    int speed;
    int score;
} Player;

Player player;

typedef struct
{
    Vector2 position;
    Vector2 velocity;
    int radius;
} Ball;

Rectangle ball = {SCREEN_WIDTH / 2 + 50, SCREEN_HEIGHT / 2, 32, 32};

int ballVelocityX = 300;
int ballVelocityY = 300;

bool isGamePaused;

Sound hitSound;

void update(float deltaTime)
{
    if (IsKeyDown(KEY_W) && player.bounds.y >= 0)
    {
        player.bounds.y -= player.speed * deltaTime;
    }

    else if (IsKeyDown(KEY_S) && player.bounds.y <= SCREEN_HEIGHT - player.bounds.height)
    {
        player.bounds.y += player.speed * deltaTime;
    }

    else if (IsKeyDown(KEY_D) && player.bounds.x <= SCREEN_WIDTH - player.bounds.width)
    {
        player.bounds.x += player.speed * deltaTime;
    }

    else if (IsKeyDown(KEY_A) && player.bounds.x > 0)
    {
        player.bounds.x -= player.speed * deltaTime;
    }

    if (ball.x < 0 || ball.x > SCREEN_WIDTH - ball.width)
    {
        ballVelocityX *= -1;
    }

    else if (ball.y < 0 || ball.y > SCREEN_HEIGHT - ball.height)
    {
        ballVelocityY *= -1;
    }

    // Check collision between a two rectangles.
    if (CheckCollisionRecs(ball, player.bounds))
    {
        ballVelocityX *= -1;
        ballVelocityY *= -1;

        player.score++;

        PlaySound(hitSound);
    }

    ball.x += ballVelocityX * deltaTime;
    ball.y += ballVelocityY * deltaTime;
}

void draw()
{
    BeginDrawing();

    ClearBackground(BLACK);

    DrawText(TextFormat("%i", player.score), 230, 20, 80, WHITE);

    DrawTexture(player.sprite, player.bounds.x, player.bounds.y, WHITE);

    DrawRectangleRec(ball, WHITE);

    if (isGamePaused)
    {
        DrawText("Game Paused", 220, 100, 80, WHITE);
    }

    EndDrawing();
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Starter");
    SetTargetFPS(60);

    Texture2D sprite = LoadTexture("assets/img/alien.png");
    player = {{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, (float)sprite.width, (float)sprite.height}, sprite, 600, 0};

    InitAudioDevice(); // Initialize audio device, before loading sound and music.

    hitSound = LoadSound("assets/sounds/okay.wav");
    Music music = LoadMusicStream("assets/music/pixel3.mp3");

    music.looping = true;

    PlayMusicStream(music);

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        float deltaTime = GetFrameTime();

        if (IsKeyPressed(KEY_SPACE))
        {
            isGamePaused = !isGamePaused;
            PlaySound(hitSound);
        }

        if (!isGamePaused)
        {
            update(deltaTime);
        }

        draw();
    }

    // Unload texture data
    UnloadTexture(player.sprite);

    // Unload sound data
    UnloadSound(hitSound);
    UnloadMusicStream(music);

    CloseAudioDevice();

    CloseWindow();
}