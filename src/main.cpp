#include "raylib.h"
#include "Physics.hpp"
#include "Pitch.hpp"

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    int scoreLeft = 0;
    int scoreRight = 0;

    InitWindow(screenWidth, screenHeight, "Physics Test");
    SetTargetFPS(60);
    Font euroFont = LoadFont("assets/fonts/Qatar2022-Bold.otf");
    Texture2D pitchTex = LoadTexture("assets/pitch.png");
    SetTextureFilter(pitchTex, TEXTURE_FILTER_BILINEAR);
    Texture2D puckTex = LoadTexture("assets/pucks/pol.png");
    SetTextureFilter(puckTex, TEXTURE_FILTER_BILINEAR);

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

        GoalResult goal = pitch.checkGoal(ball);
        if (goal == GoalResult::LeftTeamScored) {
            scoreLeft++;
            ball.reset({centerX, centerY});
        } else if (goal == GoalResult::RightTeamScored) {
            scoreRight++;
            ball.reset({centerX, centerY});
        }

        BeginDrawing();
        ClearBackground(DARKBLUE);

        Rectangle sourceRect = { 0.0f, 0.0f, (float)pitchTex.width, (float)pitchTex.height };
        DrawTexturePro(pitchTex, sourceRect, bounds, { 0.0f, 0.0f }, 0.0f, WHITE);

        DrawTextEx(euroFont, TextFormat("%d  :  %d", scoreLeft, scoreRight), {centerX - 60.0f, 35.0f}, 48, 2.0f, WHITE);

        Vector2 pos = playerPuck.getPosition();
        float r = playerPuck.getRadius();

        DrawCircleV({ pos.x + 4.0f, pos.y + 6.0f }, r, Fade(BLACK, 0.35f));

        Rectangle src = { 0.0f, 0.0f, (float)puckTex.width, (float)puckTex.height };
        Rectangle dst = { pos.x, pos.y, r * 2.0f, r * 2.0f };
        Vector2 origin = { r, r };

        DrawTexturePro(puckTex, src, dst, origin, 0.0f, WHITE);
        DrawCircleLinesV(pos, r, BLACK);

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