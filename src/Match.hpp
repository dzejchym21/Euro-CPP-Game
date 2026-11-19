#pragma once
#include "raylib.h"
#include "Team.hpp"
#include "Pitch.hpp"
#include "AimController.hpp"
#include <chrono>
#include <algorithm>

struct MatchRules {
    bool allowDraw;
    std::chrono::seconds realMatchDuration{180};
    std::chrono::seconds extraTimeDuration{60};
};

enum class MatchPhase {
    RegularTime,
    ExtraTime,
    Finished
};

struct MatchResult {
    int scoreLeft;
    int scoreRight;
};

class Match {
private:
    Team leftTeam;
    Team rightTeam;
    Pitch pitch;
    Ball ball;
    AimController aim;
    MatchRules rules;
    MatchPhase phase = MatchPhase::RegularTime;
    int scoreLeft = 0;
    int scoreRight = 0;
    Side currentTurn = Side::Left;
    std::chrono::duration<float> elapsedTime{0.0f};

    bool isAnyObjectMoving() const {
        return ball.isMoving() || leftTeam.isAnyPuckMoving() || rightTeam.isAnyPuckMoving();
    }

    void updatePhysics(float dt) {
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
    }

    void checkGoals() {
        auto bounds = pitch.getBounds();
        float centerX = bounds.x + bounds.width / 2.0f;
        float centerY = bounds.y + bounds.height / 2.0f;

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
    }

    void updateClockAndPhase() {
        if (phase == MatchPhase::RegularTime) {
            if (elapsedTime >= rules.realMatchDuration && !isAnyObjectMoving()) {
                if (scoreLeft != scoreRight || rules.allowDraw) {
                    phase = MatchPhase::Finished;
                } else {
                    phase = MatchPhase::ExtraTime;
                    elapsedTime = std::chrono::duration<float>{0.0f};

                    auto bounds = pitch.getBounds();
                    ball.reset({ bounds.x + bounds.width / 2.0f, bounds.y + bounds.height / 2.0f });
                    leftTeam.setupFormation(bounds);
                    rightTeam.setupFormation(bounds);
                }
            }
        } else if (phase == MatchPhase::ExtraTime) {
            if (elapsedTime >= rules.extraTimeDuration && !isAnyObjectMoving()) {
                phase = MatchPhase::Finished;
            }
        }
    }

public:
    Match(Team left, Team right, MatchRules rules): leftTeam(std::move(left)), rightTeam(std::move(right)), rules(rules) {
        auto bounds = pitch.getBounds();
        float centerX = bounds.x + bounds.width / 2.0f;
        float centerY = bounds.y + bounds.height / 2.0f;

        ball.setPosition({centerX, centerY});
        leftTeam.setupFormation(bounds);
        rightTeam.setupFormation(bounds);
    }

    void update(float dt, Vector2 virtualMouse) {
        if (phase == MatchPhase::Finished) return;

        elapsedTime += std::chrono::duration<float>(dt);
        bool anyMoving = isAnyObjectMoving();
        Team& activeTeam = (currentTurn == Side::Left) ? leftTeam : rightTeam;
        if (aim.update(activeTeam.getPucks(), currentTurn, !anyMoving, virtualMouse)) {
            currentTurn = (currentTurn == Side::Left) ? Side::Right : Side::Left;
        }

        updatePhysics(dt);
        checkGoals();
        updateClockAndPhase();
    }

    const Team& getLeftTeam() const { return leftTeam; }
    const Team& getRightTeam() const { return rightTeam; }
    const Pitch& getPitch() const { return pitch; }
    const Ball& getBall() const { return ball; }
    const AimController& getAim() const { return aim; }

    int getScoreLeft() const { return scoreLeft; }
    int getScoreRight() const { return scoreRight; }
    Side getCurrentTurn() const { return currentTurn; }
    MatchPhase getPhase() const { return phase; }
    bool isFinished() const { return phase == MatchPhase::Finished; }

    MatchResult getResult() const {
        return MatchResult{ scoreLeft, scoreRight };
    }

    int getDisplayMatchSeconds() const {
        if (phase == MatchPhase::RegularTime) {
            float progress = std::min(1.0f, elapsedTime / rules.realMatchDuration);
            return static_cast<int>(progress * 90.0f * 60.0f);
        }
        if (phase == MatchPhase::ExtraTime) {
            float progress = std::min(1.0f, elapsedTime / rules.extraTimeDuration);
            return (90 * 60) + static_cast<int>(progress * 30.0f * 60.0f);
        }
        return 90 * 60;
    }
};