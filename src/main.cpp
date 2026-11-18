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

    Texture2D puckTex = LoadTexture("assets/pucks/pol.png");
    SetTextureFilter(puckTex, TEXTURE_FILTER_BILINEAR);

    Pitch pitch;
    Rectangle bounds = pitch.getBounds();
    AimController aim;

    Team leftTeam("Poland", "POL", Side::Left, TeamSide::Player, Formation::F1_2_2, puckTex, puckTex);

    leftTeam.setupFormation(bounds);

    float centerX = bounds.x + bounds.width / 2.0f;
    float centerY = bounds.y + bounds.height / 2.0f;
    Ball ball({centerX, centerY});

    TeamSide currentTurn = TeamSide::Player;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        bool anyMoving = ball.isMoving() || leftTeam.isAnyPuckMoving();

        Team& activeTeam = (currentTurn == TeamSide::Player) ? leftTeam : leftTeam;
        if (aim.update(activeTeam.getPucks(), currentTurn, !anyMoving)) {
            //currentTurn = (currentTurn == TeamSide::Player) ? TeamSide::AI : TeamSide::Player;
        }

        auto leftPucks = leftTeam.getPucks();
        for (auto& puck : leftPucks) {
            updateMovement(puck, dt);
            pitch.handleCollision(puck, false);
            resolveCollision(puck, ball);
        }

        for (std::size_t i = 0; i < leftPucks.size(); ++i) {
            for (std::size_t j = i + 1; j < leftPucks.size(); ++j) {
                resolveCollision(leftPucks[i], leftPucks[j]);
            }
        }

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

        DrawTextEx(euroFont, TextFormat("%s %d  :  %d", leftTeam.getShortName().c_str(), scoreLeft, scoreRight), {centerX - 60.0f, 35.0f}, 48, 2.0f, WHITE);

        aim.draw();
        leftTeam.drawPucks();

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