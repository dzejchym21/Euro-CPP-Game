#include "raylib.h"
#include "Physics.hpp"
#include "Pitch.hpp"

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Physics Test");
    SetTargetFPS(60);
    Font euroFont = LoadFont("assets/fonts/Qatar2022-Bold.otf");

    Pitch pitch;
    Rectangle bounds = pitch.getBounds();
    Puck playerPuck({400.0f, 300.0f}, TeamSide::Player, 10);
    Ball ball({640.0f, 360.0f});

    float centerX = bounds.x + bounds.width / 2.0f;
    float centerY = bounds.y + bounds.height / 2.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        float acc = 900.0f;
        Vector2 vel = playerPuck.getVelocity();

        if (IsKeyDown(KEY_RIGHT)) vel.x += acc * dt;
        if (IsKeyDown(KEY_LEFT)) vel.x -= acc * dt;
        if (IsKeyDown(KEY_DOWN)) vel.y += acc * dt;
        if (IsKeyDown(KEY_UP)) vel.y -= acc * dt;

        playerPuck.setVelocity(vel);
        if (IsKeyPressed(KEY_R)) {
            playerPuck.setPosition({400.0f, 360.0f});
            playerPuck.setVelocity({0.0f, 0.0f});
            ball.reset({640.0f, 360.0f});
        }
        updateMovement(playerPuck, dt);
        pitch.handleCollision(playerPuck, false);
        resolveCollision(playerPuck, ball);

        updateMovement(ball, dt);
        pitch.handleCollision(ball, true);

        BeginDrawing();
        ClearBackground(DARKBLUE);

        DrawRectangleRec(bounds, {46, 139, 57, 255});
        DrawRectangleLinesEx(bounds, 4.0f, WHITE);
        DrawLineEx({centerX, bounds.y}, {centerX, bounds.y + bounds.height}, 4.0f, WHITE);
        DrawCircleLines(centerX, centerY, 75.0f, WHITE);
        DrawCircle(centerX, centerY, 6.0f, WHITE);

        DrawTextEx(euroFont, "TEST PITCH", (Vector2){20.0f, 20.0f}, 40, 2.0f, WHITE);

        DrawCircleV(playerPuck.getPosition(), playerPuck.getRadius(), BLUE);
        DrawCircleLinesV(playerPuck.getPosition(), playerPuck.getRadius(), WHITE);

        DrawCircleV(ball.getPosition(), ball.getRadius(), WHITE);
        DrawCircleLinesV(ball.getPosition(), ball.getRadius(), BLACK);

        for (const auto& post : pitch.getGoalPosts()) {
            DrawCircleV(post, pitch.getPostRadius(), WHITE);
            DrawCircleLinesV(post, pitch.getPostRadius(), BLACK);
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}