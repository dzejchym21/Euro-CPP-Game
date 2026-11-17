#pragma once
#include "Physics.hpp"
#include "raylib.h"
#include <array>

enum class GoalResult {
    None,
    LeftTeamScored,
    RightTeamScored
};

class Pitch {
private:
    Rectangle bounds{ 140.0f, 110.0f, 1000.0f, 520.0f };
    float goalTopY = 290.0f;
    float goalBottomY = 450.0f;
    float goalDepth = 50.0f;
    float postRadius = 8.0f;

    std::array<Vector2, 4> goalPosts{{
        {140.0f, 290.0f},
        {140.0f, 450.0f},
        {1140.0f, 290.0f},
        {1140.0f, 450.0f}
    }};

public:
    template <Collidable T>
    void handleCollision(T& obj, bool canEnterGoal) const {
        Vector2 pos = obj.getPosition();
        Vector2 vel = obj.getVelocity();
        float r = obj.getRadius();

        if (pos.y - r < bounds.y) {
            pos.y = bounds.y + r;
            vel.y = - vel.y * 0.8f;
        } else if (pos.y + r > bounds.y + bounds.height) {
            pos.y = bounds.y + bounds.height - r;
            vel.y = - vel.y * 0.8f;
        }

        bool inGoalY = (pos.y > goalTopY && pos.y < goalBottomY);
        float wallRestitution = (canEnterGoal && inGoalY) ? 0.35f : 0.8f;


        float leftLimit = (canEnterGoal && inGoalY) ? (bounds.x - goalDepth) : bounds.x;
        if (pos.x - r < leftLimit) {
            pos.x = leftLimit + r;
            vel.x = - vel.x * wallRestitution;
        }

        float rightLimit = (canEnterGoal && inGoalY) ? (bounds.x + bounds.width + goalDepth): (bounds.x + bounds.width);
        if (pos.x + r > rightLimit) {
            pos.x = rightLimit - r;
            vel.x = - vel.x * wallRestitution;
        }

        bool insideLeftGoalDepth = (pos.x < bounds.x);
        bool insideRightGoalDepth = (pos.x > bounds.x + bounds.width);

        if (canEnterGoal && (insideLeftGoalDepth || insideRightGoalDepth)) {
            if (pos.y - r < goalTopY) {
                pos.y = goalTopY + r;
                vel.y = - vel.y * 0.35f;
            } else if (pos.y + r > goalBottomY) {
                pos.y = goalBottomY - r;
                vel.y = -vel.y * 0.35f;
            }
        }

        for (const auto& post : goalPosts) {
            float dx = pos.x - post.x;
            float dy = pos.y - post.y;
            float dist = std::hypot(dx, dy);
            float minDist = r + postRadius;

            if (dist < minDist && dist > 0.0f) {
                float nx = dx / dist;
                float ny = dy / dist;
                pos.x = post.x + nx * minDist;
                pos.y = post.y + ny * minDist;

                float dot = vel.x * nx + vel.y * ny;
                if (dot < 0.0f) {
                    vel.x -= 1.8f * dot * nx;
                    vel.y -= 1.8f * dot * ny;
                }
            }
        }

        obj.setPosition(pos);
        obj.setVelocity(vel);
    }

    Rectangle getBounds() const { return bounds; }
    float getGoalTopY() const { return goalTopY; }
    float getGoalBottomY() const { return goalBottomY; }
    float getGoalDepth() const { return goalDepth; }
    float getPostRadius() const { return postRadius; }
    const std::array<Vector2, 4>& getGoalPosts() const { return goalPosts; }
};