#pragma once
#include "Physics.hpp"
#include "raylib.h"
#include <array>
#include <span>
#include <string>

enum class Side {
    Left,
    Right
};

enum class Formation {
    F1_2_2,
    F1_3_1
};

class Team {
private:
    std::string name;
    std::string shortName;
    Side side;
    TeamSide teamSide;
    Formation formation;
    Texture2D puckTexture;
    Texture2D flagTexture;
    std::array<Puck, 5> pucks;
public:
    Team(std::string name,
        std::string shortName,
        Side side,
        TeamSide teamSide,
        Formation formation,
        Texture2D puckTexture,
        Texture2D flagTexture)
            : name(name), shortName(shortName),
            side(side), formation(formation), teamSide(teamSide),
            puckTexture(puckTexture), flagTexture(flagTexture),
            pucks{{
                Puck({0.0f, 0.0f}, teamSide, 1),
                Puck({0.0f, 0.0f}, teamSide, 4),
                Puck({0.0f, 0.0f}, teamSide, 7),
                Puck({0.0f, 0.0f}, teamSide, 9),
                Puck({0.0f, 0.0f}, teamSide, 10)
            }} {}

    void setupFormation(Rectangle bounds) {
        float centerX = bounds.x + bounds.width / 2.0f;
        float centerY = bounds.y + bounds.height / 2.0;

        std::array<Vector2, 5> positions{};

        if (formation == Formation::F1_2_2) {
            positions = {{
                {bounds.x + 60.0f, centerY },
                {bounds.x + 210.0f, centerY - 120.0f},
                {bounds.x + 210.0f, centerY + 120.0f},
                {bounds.x + 370.0f, centerY - 80.0f},
                {bounds.x + 370.0f, centerY + 80.0f}
            }};
        }

        for (std::size_t i = 0; i < pucks.size(); i++) {
            Vector2 pos = positions[i];

            if (side == Side::Right) {
                pos.x = centerX + (centerX - pos.x);
            }

            pucks[i].setPosition(pos);
            pucks[i].setVelocity({ 0.0f, 0.0f });
        }
    }

    bool isAnyPuckMoving() const {
        for (const auto& puck : pucks) {
            if (puck.isMoving()) return true;
        }
        return false;
    }

    void drawPucks() const {
        const float r = pucks[0].getRadius();
        for (const auto& puck : pucks) {
            Vector2 pos = puck.getPosition();
            DrawCircleV({ pos.x + 4.0f, pos.y + 6.0f }, r, Fade(BLACK, 0.35f));
            Rectangle src = { 0.0f, 0.0f, (float)puckTexture.width, (float)puckTexture.height };
            Rectangle dst = { pos.x, pos.y, r * 2.0f, r * 2.0f };
            Vector2 origin = { r, r };

            DrawTexturePro(puckTexture, src, dst, origin, 0.0f, WHITE);
            DrawCircleLinesV(pos, r, BLACK);
        }
    }

    std::span<Puck> getPucks() { return pucks; }
    std::span<const Puck> getPucks() const { return pucks; }
    Side getSide() const { return side; }
    TeamSide getTeamSide() const { return teamSide; }
    const std::string& getShortName() const { return shortName; }
    Texture2D getFlagTexture() const { return flagTexture; }
};
