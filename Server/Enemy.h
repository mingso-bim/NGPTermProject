#pragma once
#include <atlimage.h>
#include <vector>
#include "Player.h"
#include "Obstacle.h"

class Enemy {
public:
    Enemy(unsigned short id, float x, float y, float speed, int health, float animationSpeed = 5.0f, float eWidth = 50.0f, float eHeight = 50.0f);
    virtual ~Enemy();

    void Update(float frameTime, float playerX, float plyaerY, const std::vector<Obstacle*>& obstacles);
    void Draw(HDC hdc, float offsetX, float offsetY);

    void TakeDamage(int damage);
    bool IsDead() const;

    float GetX() const;
    float GetY() const;
    unsigned short GetAimPlayerID() const;
    float GetWidth() const;
    float GetHeight() const;

    // void SetBounds(float width, float height);

protected:
    void DrawDeathEffect(HDC hdc, float offsetX, float offsetY);

    float x, y;
    float eWidth, eHeight;
    float speed;
    int health;
    float boundWidth, boundHeight;
    int aimPlayerID;

    int currentFrame;
    float frameTimeAccumulator;
    float animationSpeed;

    std::vector<CImage> idleImages;

    bool CheckCollision(float newX, float newY, const std::vector<Obstacle*>& obstacles) const;

    bool isDying;
    float deathEffectDuration;
    float deathEffectStart;
    std::vector<CImage> deathEffectImages;
};

// BrainMonster
class BrainMonster : public Enemy {
public:
    BrainMonster(float x, float y, float speed, int health = 50, float eWidth = 27.0f, float eHeight = 36.0f);
};

// EyeMonster
class EyeMonster : public Enemy {
public:
    EyeMonster(float x, float y, float speed, int health = 50, float eWidth = 37.0f, float eHeight = 29.0f);
};

// BigBoomer
class BigBoomer : public Enemy {
public:
    BigBoomer(float x, float y, float speed, int health = 500, float eWidth = 45.0f, float eHeight = 51.0f);
};

// Lamprey
class Lamprey : public Enemy {
public:
    Lamprey(float x, float y, float speed, int health = 1000, float eWidth = 50.0f, float eHeight = 50.0f);
};

