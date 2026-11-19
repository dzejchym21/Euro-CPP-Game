#include "raylib.h"
#include "Match.hpp"
#include <algorithm>

int main() {
    const int virtualWidth = 1280;
    const int virtualHeight = 720;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(virtualWidth, virtualHeight, "Soccer Stars 5v5");
    SetTargetFPS(60);

    RenderTexture2D target = LoadRenderTexture(virtualWidth, virtualHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    Font euroFont = LoadFont("assets/fonts/Qatar2022-Bold.otf");
    Texture2D pitchTex = LoadTexture("assets/pitch.png");
    SetTextureFilter(pitchTex, TEXTURE_FILTER_BILINEAR);

    Texture2D puckTex1 = LoadTexture("assets/pucks/pol.png");
    SetTextureFilter(puckTex1, TEXTURE_FILTER_BILINEAR);
    Texture2D puckTex2 = LoadTexture("assets/pucks/gre.png");
    SetTextureFilter(puckTex2, TEXTURE_FILTER_BILINEAR);

    Team leftTeam("Poland", "POL", Side::Left, Formation::F1_2_2, puckTex1, puckTex1);
    Team rightTeam("Greece", "GRE", Side::Right, Formation::F1_3_1, puckTex2, puckTex2);

    MatchRules rules{ false, std::chrono::seconds(180), std::chrono::seconds(60) };
    Match match(std::move(leftTeam), std::move(rightTeam), rules);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        float scale = std::min(
            static_cast<float>(GetScreenWidth()) / virtualWidth,
            static_cast<float>(GetScreenHeight()) / virtualHeight
        );
        float offsetX = (GetScreenWidth() - (virtualWidth * scale)) * 0.5f;
        float offsetY = (GetScreenHeight() - (virtualHeight * scale)) * 0.5f;

        Vector2 virtualMouse = {
            (GetMousePosition().x - offsetX) / scale,
            (GetMousePosition().y - offsetY) / scale
        };

        match.update(dt, virtualMouse);

        const auto& pitch = match.getPitch();
        Rectangle bounds = pitch.getBounds();
        float centerX = bounds.x + bounds.width / 2.0f;

        int totalSec = match.getDisplayMatchSeconds();
        int minutes = totalSec / 60;
        int seconds = totalSec % 60;

        BeginTextureMode(target);
        ClearBackground(DARKBLUE);

        Rectangle sourceRect = { 0.0f, 0.0f, (float)pitchTex.width, (float)pitchTex.height };
        DrawTexturePro(pitchTex, sourceRect, bounds, { 0.0f, 0.0f }, 0.0f, WHITE);

        DrawTextEx(euroFont,
                   TextFormat("%s %d  :  %d %s",
                              match.getLeftTeam().getShortName().c_str(),
                              match.getScoreLeft(),
                              match.getScoreRight(),
                              match.getRightTeam().getShortName().c_str()),
                   {centerX - 150.0f, 30.0f}, 46, 2.0f, WHITE);

        DrawTextEx(euroFont, TextFormat("%02d:%02d", minutes, seconds), {centerX - 42.0f, 76.0f}, 26, 2.0f, YELLOW);

        if (match.isFinished()) {
            DrawTextEx(euroFont, "KONIEC MECZU!", {centerX - 160.0f, 330.0f}, 54, 2.0f, GOLD);
        }

        match.getAim().draw();

        Vector2 bPos = match.getBall().getPosition();
        float bR = match.getBall().getRadius();
        DrawCircleV({ bPos.x + 3.0f, bPos.y + 4.0f }, bR, Fade(BLACK, 0.35f));
        DrawCircleV(bPos, bR, WHITE);
        DrawCircleLinesV(bPos, bR, BLACK);

        match.getLeftTeam().drawPucks();
        match.getRightTeam().drawPucks();

        for (const auto& post : pitch.getGoalPosts()) {
            DrawCircleV(post, pitch.getPostRadius(), WHITE);
            DrawCircleLinesV(post, pitch.getPostRadius(), BLACK);
        }
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);

        Rectangle srcRec = { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height };
        Rectangle dstRec = { offsetX, offsetY, virtualWidth * scale, virtualHeight * scale };
        DrawTexturePro(target.texture, srcRec, dstRec, { 0.0f, 0.0f }, 0.0f, WHITE);
        EndDrawing();
    }

    UnloadRenderTexture(target);
    UnloadFont(euroFont);
    UnloadTexture(pitchTex);
    UnloadTexture(puckTex1);
    UnloadTexture(puckTex2);
    CloseWindow();
    return 0;
}