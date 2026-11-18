#pragma once
#include "Physics.hpp"
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <span>

class AimController {
private:
    Puck* selectedPuck = nullptr;
    Vector2 currentPull = {0.0f, 0.0f};
    float maxDrag = 120.0f;
    float minDrag = 15.0f;
    float maxPower = 1150.0f;

public:
    bool update(std::span<Puck> pucks, Side allowedTeam, bool canShoot) {
        if (!canShoot) {
            selectedPuck = nullptr;
            currentPull = {0.0f, 0.0f};
            return false;
        }
        Vector2 mousePos = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            for (auto& puck : pucks) {
                if (puck.getTeam() == allowedTeam && CheckCollisionPointCircle(mousePos, puck.getPosition(), puck.getRadius())) {
                    selectedPuck = &puck;
                    currentPull = {0.0f, 0.0f};
                    break;
                }
            }
        }

        if (selectedPuck != nullptr && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 puckPos = selectedPuck->getPosition();
            Vector2 rawPull = {puckPos.x - mousePos.x, puckPos.y - mousePos.y};
            float len = Vector2Length(rawPull);

            if (len > maxDrag) {
                rawPull = Vector2Scale(Vector2Normalize(rawPull), maxDrag);
            }
            currentPull = rawPull;
        }

        if (selectedPuck != nullptr && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            float len = Vector2Length(currentPull);
            bool shotFired = false;

            if (len >= minDrag) {
                Vector2 dir = Vector2Normalize(currentPull);
                float power = (len / maxDrag) * maxPower;
                selectedPuck->kick(dir, power);
                shotFired = true;
            }

            selectedPuck = nullptr;
            currentPull = { 0.0f, 0.0f };
            return shotFired;
        }

        return false;
    }

    void draw() const {
        if (selectedPuck == nullptr) return;

        Vector2 pos = selectedPuck->getPosition();
        float len = Vector2Length(currentPull);

        DrawCircleV(pos, len, Fade(BLACK, 0.35f));

        if (len >= minDrag) {
            float ratio = len / maxDrag;
            Color aimColor = {
                static_cast<unsigned char>(255 * ratio),
                static_cast<unsigned char>(255 * (1.0f - ratio * 0.7f)),
                40,
                230
            };

            Vector2 tip = Vector2Add(pos, currentPull);
            DrawLineEx(pos, tip, 6.0f, aimColor);

            Vector2 dir = Vector2Normalize(currentPull);
            Vector2 perp = { -dir.y, dir.x };
            Vector2 p1 = Vector2Add(tip, Vector2Scale(dir, 14.0f));
            Vector2 p2 = Vector2Add(Vector2Subtract(tip, Vector2Scale(dir, 6.0f)), Vector2Scale(perp, 10.0f));
            Vector2 p3 = Vector2Subtract(Vector2Subtract(tip, Vector2Scale(dir, 6.0f)), Vector2Scale(perp, 10.0f));
            DrawTriangle(p1, p3, p2, aimColor);
        }
    }

    bool isAiming() const { return selectedPuck != nullptr; }
};