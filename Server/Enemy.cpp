#include "Enemy.h"
#include "GameFrameWork.h"
#include <cmath>

#define EnemyWidth 50.0f
#define EnemyHeight 50.0f

Enemy::Enemy(unsigned short id, float x, float y, float speed, int health, float animationSpeed, float eWidth, float eHeight)
    :aimPlayerID(id), x(x), y(y), speed(speed), health(health), boundWidth(0), boundHeight(0),
    currentFrame(0), frameTimeAccumulator(0.0f),
    animationSpeed(animationSpeed), eWidth(eWidth), eHeight(eHeight),
    isDying(false), deathEffectDuration(0.5f), deathEffectStart(0.0f) {


}

Enemy::~Enemy() {
}



void Enemy::Update(float frameTime, float playerX, float playerY, const std::vector<Obstacle*>& obstacles) {

    float dx = playerX - x;
    float dy = playerY - y;
    float distance = sqrt(dx * dx + dy * dy);

    float directionX = dx / distance;
    float directionY = dy / distance;

    float newX = x + directionX * speed * frameTime;
    float newY = y + directionY * speed * frameTime;

    if (!CheckCollision(newX, newY, obstacles)) {
        x = newX;
        y = newY;
    }

    frameTimeAccumulator += frameTime;
    if (frameTimeAccumulator >= animationSpeed) {
        currentFrame = (currentFrame + 1) % idleImages.size();
        frameTimeAccumulator = 0.0f;
    }
}

bool Enemy::CheckCollision(float newX, float newY, const std::vector<Obstacle*>& obstacles) const {
    for (const auto& obstacle : obstacles) {
        float ox = obstacle->GetX();
        float oy = obstacle->GetY();
        float ow = obstacle->GetWidth();
        float oh = obstacle->GetHeight();

        if (newX < ox + ow &&
            newX + eWidth > ox &&
            newY < oy + oh &&
            newY + eHeight > oy) {
            return true;
        }
    }
    return false;
}

void Enemy::Draw(HDC hdc, float offsetX, float offsetY) {
    if (isDying) {
        DrawDeathEffect(hdc, offsetX, offsetY);
        return;
    }

    if (!idleImages[currentFrame].IsNull()) {
        idleImages[currentFrame].Draw(hdc, static_cast<int>(x - offsetX), static_cast<int>(y - offsetY));
    }
}

void Enemy::DrawDeathEffect(HDC hdc, float offsetX, float offsetY) {
    int frame = static_cast<int>((deathEffectStart / deathEffectDuration) * deathEffectImages.size());
    if (frame >= 0 && frame < deathEffectImages.size()) {
        deathEffectImages[frame].Draw(hdc, static_cast<int>(x - offsetX), static_cast<int>(y - offsetY));
    }
}

void Enemy::TakeDamage(int damage) {
    health -= damage;
    if (health <= 0 && !isDying) {
        isDying = true;
        health = 0;
        deathEffectStart = 0.0f;
    }
}

bool Enemy::IsDead() const {
    return health <= 0 && !isDying;
}

float Enemy::GetX() const {
    return x;
}

float Enemy::GetY() const {
    return y;
}

unsigned short Enemy::GetAimPlayerID() const {
    return aimPlayerID;
}

float Enemy::GetWidth() const {
    return eWidth;
}

float Enemy::GetHeight() const {
    return eHeight;
}

// BrainMonster
BrainMonster::BrainMonster(float x, float y, float speed, int health, float eWidth, float eHeight)
    : Enemy(x, y, 20.0f, health, 0.2f, eWidth, eHeight) {
}

// EyeMonster
EyeMonster::EyeMonster(float x, float y, float speed, int health, float eWidth, float eHeight)
    : Enemy(x, y, 40.0f, health, 0.2f, eWidth, eHeight) {
}


// BigBoomer
BigBoomer::BigBoomer(float x, float y, float speed, int health, float eWidth, float eHeight)
    : Enemy(x, y, 30.0f, health, 0.2f, eWidth, eHeight) {
}

// Lamprey
Lamprey::Lamprey(float x, float y, float speed, int health, float eWidth, float eHeight)
    : Enemy(x, y, 40.0f, health, 0.2f, eWidth, eHeight) {
}
