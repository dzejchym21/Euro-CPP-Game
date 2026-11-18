#include "raylib.h"
#include "Physics.hpp"
#include "Pitch.hpp"
#include "AimController.hpp"
#include "Team.hpp"

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

    Texture2D puckTex1 = LoadTexture("assets/pucks/pol.png");
    SetTextureFilter(puckTex1, TEXTURE_FILTER_BILINEAR);
    Texture2D puckTex2 = LoadTexture("assets/pucks/gre.png");
    SetTextureFilter(puckTex2, TEXTURE_FILTER_BILINEAR);

    Pitch pitch;
    Rectangle bounds = pitch.getBounds();
    AimController aim;

    Team leftTeam("Poland", "POL", Side::Left, Formation::F1_2_2, puckTex1, puckTex1);
    Team rightTeam("Greece", "GRE", Side::Right, Formation::F1_3_1, puckTex2, puckTex2);

    leftTeam.setupFormation(bounds);
    rightTeam.setupFormation(bounds);

    float centerX = bounds.x + bounds.width / 2.0f;
    float centerY = bounds.y + bounds.height / 2.0f;
    Ball ball({centerX, centerY});

    Side currentTurn = Side::Left;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        bool anyMoving = ball.isMoving() || leftTeam.isAnyPuckMoving() || rightTeam.isAnyPuckMoving();

        Team& activeTeam = (currentTurn == Side::Left) ? leftTeam : rightTeam;
        if (aim.update(activeTeam.getPucks(), currentTurn, !anyMoving)) {
            currentTurn = (currentTurn == Side::Left) ? Side::Right : Side::Left;
        }

        auto leftPucks = leftTeam.getPucks();
        auto rightPucks = rightTeam.getPucks();

        for (auto& puck : leftPucks) {
            updateMovement(puck, dt);
            pitch.handleCollision(puck, false);
            resolveCollision(puck, ball);
        }

        for (auto& puck : rightPucks) {
            updateMovement(puck, dt);
            pitch.handleCollision(puck, false);
            resolveCollision(puck, ball);
        }

        for (std::size_t i = 0; i < leftPucks.size(); ++i) {
            for (std::size_t j = i + 1; j < leftPucks.size(); ++j) {
                resolveCollision(leftPucks[i], leftPucks[j]);
            }
            for (std::size_t j = 0; j < rightPucks.size(); j++) {
                resolveCollision(rightPucks[j], leftPucks[i]);
            }
        }

        for (std::size_t i = 0; i < rightPucks.size(); ++i) {
            for (std::size_t j = i + 1; j < rightPucks.size(); ++j) {
                resolveCollision(rightPucks[i], rightPucks[j]);
            }
        }

        updateMovement(ball, dt);
        pitch.handleCollision(ball, true);

        GoalResult goal = pitch.checkGoal(ball);
        if (goal == GoalResult::LeftTeamScored) {
            scoreLeft++;
            ball.reset({centerX, centerY});
            leftTeam.setupFormation(bounds);
            rightTeam.setupFormation(bounds);
            currentTurn = Side::Right;
        } else if (goal == GoalResult::RightTeamScored) {
            scoreRight++;
            ball.reset({centerX, centerY});
            leftTeam.setupFormation(bounds);
            rightTeam.setupFormation(bounds);
            currentTurn = Side::Left;
        }

        BeginDrawing();
        ClearBackground(DARKBLUE);

        Rectangle sourceRect = { 0.0f, 0.0f, (float)pitchTex.width, (float)pitchTex.height };
        DrawTexturePro(pitchTex, sourceRect, bounds, { 0.0f, 0.0f }, 0.0f, WHITE);

        DrawTextEx(euroFont, TextFormat("%s %d  :  %d %s", leftTeam.getShortName().c_str(), scoreLeft, scoreRight, rightTeam.getShortName().c_str()), {centerX - 150.0f, 35.0f}, 48, 2.0f, WHITE);

        aim.draw();
        leftTeam.drawPucks();
        rightTeam.drawPucks();

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