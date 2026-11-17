#include "raylib.h"
#include "Physics.hpp"

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Physics Test");
    SetTargetFPS(60);

    Puck playerPuck({400.0f, 300.0f}, TeamSide::Player, 10);
    Ball ball({640.0f, 360.0f});
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
        updateMovement(ball, dt);

        resolveCollision(playerPuck, ball);

        // 4. Rysowanie
        BeginDrawing();
        ClearBackground(DARKGREEN);

        DrawText("Strzalki: sterowanie kapslem | SPACJA: mocny strzal w prawo | R: reset", 20, 20, 20, WHITE);

        DrawCircleV(playerPuck.getPosition(), playerPuck.getRadius(), BLUE);
        DrawCircleLinesV(playerPuck.getPosition(), playerPuck.getRadius(), WHITE);

        DrawCircleV(ball.getPosition(), ball.getRadius(), WHITE);
        DrawCircleLinesV(ball.getPosition(), ball.getRadius(), BLACK);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}