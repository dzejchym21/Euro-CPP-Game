#include "raylib.h"
#include "Physics.hpp"
#include "Pitch.hpp"
#include "AimController.hpp"
#include <vector>

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
    AimController aim;
    std::vector<Puck> pucks;
    pucks.emplace_back(Vector2{400.0f, 360.0f}, TeamSide::Player, 10);

    float centerX = bounds.x + bounds.width / 2.0f;
    float centerY = bounds.y + bounds.height / 2.0f;

    Ball ball({centerX, centerY});

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        bool canShoot = !pucks[0].isMoving() && !ball.isMoving();
        aim.update(pucks, TeamSide::Player, canShoot);

        if (IsKeyPressed(KEY_R)) {
            pucks[0].setPosition({400.0f, 360.0f});
            pucks[0].setVelocity({0.0f, 0.0f});
            ball.reset({640.0f, 360.0f});
        }
        updateMovement(pucks[0], dt);
        pitch.handleCollision(pucks[0], false);
        resolveCollision(pucks[0], ball);

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

        Vector2 pos = pucks[0].getPosition();
        float r = pucks[0].getRadius();

        DrawCircleV({ pos.x + 4.0f, pos.y + 6.0f }, r, Fade(BLACK, 0.35f));

        Rectangle src = { 0.0f, 0.0f, (float)puckTex.width, (float)puckTex.height };
        Rectangle dst = { pos.x, pos.y, r * 2.0f, r * 2.0f };
        Vector2 origin = { r, r };

        aim.draw();
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