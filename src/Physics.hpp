#pragma once
#include "raylib.h"
#include <concepts>
#include <cmath>
#include <string>

enum class TeamSide {
    Player,
    AI
};

template <typename T>
concept Collidable = requires(T obj, Vector2 v) {
    { obj.getPosition() } -> std::same_as<Vector2>;
    { obj.getVelocity() } -> std::same_as<Vector2>;
    { obj.getRadius() } -> std::convertible_to<float>;
    { obj.getMass() } -> std::convertible_to<float>;
    { obj.setPosition(v) };
    { obj.setVelocity(v) };
};

class Puck {
private:
    Vector2 position {0.0f, 0.0f};
    Vector2 velocity {0.0f, 0.0f};
    float radius = 26.0f;
    float mass = 2.0f;
    TeamSide team;
    int shirtNumber;

public:
    Puck(Vector2 startPos, TeamSide side, int number)
        : position(startPos), team(side), shirtNumber(number) {}

    void kick(Vector2 direction, float power) {
        velocity.x = direction.x * power;
        velocity.y = direction.y * power;
    }

    bool isMoving() const {
        return std::hypot(velocity.x, velocity.y) > 0.0f;
    }

    Vector2 getPosition() const { return position; }
    Vector2 getVelocity() const { return velocity; }
    float getRadius() const { return radius; }
    float getMass() const { return mass; }
    TeamSide getTeam() const { return team; }
    int getShirtNumber() const { return shirtNumber; }

    void setPosition(Vector2 pos) { position = pos;}
    void setVelocity(Vector2 v) { velocity = v;}
};

class Ball {
private:
    Vector2 position {0.0f, 0.0f};
    Vector2 velocity {0.0f, 0.0f};
    float radius = 16.0f;
    float mass = 1.0f;

public:
    Ball(Vector2 startPos): position(startPos) {}

    void reset(Vector2 centerPos) {
        position = centerPos;
        velocity = {0.0f, 0.0f};
    }

    bool isMoving() const {
        return std::hypot(velocity.x, velocity.y) > 0.0f;
    }

    Vector2 getPosition() const { return position; }
    Vector2 getVelocity() const { return velocity; }
    float getRadius() const { return radius; }
    float getMass() const { return mass; }

    void setPosition(Vector2 pos) { position = pos;}
    void setVelocity(Vector2 v) { velocity = v;}
};

template <Collidable T>
void updateMovement(T& obj, float dt, float friction = 0.98f) {
    Vector2 pos = obj.getPosition();
    Vector2 vel = obj.getVelocity();

    pos.x += vel.x * dt;
    pos.y += vel.y * dt;

    vel.x *= friction;
    vel.y *= friction;

    if (std::hypot(vel.x, vel.y) < 5.0f) {
        vel = {0.0f, 0.0f};
    }

    obj.setPosition(pos);
    obj.setVelocity(vel);
}

template <Collidable A, Collidable B>
void resolveCollision(A& obj1, B& obj2, float restitution = 0.85f) {
    Vector2 pos1 = obj1.getPosition();
    Vector2 pos2 = obj2.getPosition();

    float dx = pos2.x - pos1.x;
    float dy = pos2.y - pos1.y;
    float dist = std::hypot(dx, dy);
    float minDist = obj1.getRadius() + obj2.getRadius();

    // If two distance between two objects is bigger than sum of their radius there is no collision
    if (dist >= minDist || dist == 0.0f) return;

    float nx = dx / dist;
    float ny = dy / dist;

    float overlap = minDist - dist;
    float totalMass = obj1.getMass() + obj2.getMass();
    pos1.x -= nx * overlap * (obj2.getMass() / totalMass);
    pos1.y -= ny * overlap * (obj2.getMass() / totalMass);
    pos2.x += nx * overlap * (obj1.getMass() / totalMass);
    pos2.y += ny * overlap * (obj1.getMass() / totalMass);
    obj1.setPosition(pos1);
    obj2.setPosition(pos2);

    Vector2 vel1 = obj1.getVelocity();
    Vector2 vel2 = obj2.getVelocity();
    float rvx = vel2.x - vel1.x;
    float rvy = vel2.y - vel1.y;
    float velAlongNormal = rvx * nx + rvy * ny;

    if (velAlongNormal > 0.0f) return;

    float impulse = -(1.0f + restitution) * velAlongNormal;
    impulse /= (1.0f / obj1.getMass() + 1.0f / obj2.getMass());

    vel1.x -= (impulse / obj1.getMass()) * nx;
    vel1.y -= (impulse / obj1.getMass()) * ny;
    vel2.x += (impulse / obj2.getMass()) * nx;
    vel2.y += (impulse / obj2.getMass()) * ny;

    obj1.setVelocity(vel1);
    obj2.setVelocity(vel2);
}