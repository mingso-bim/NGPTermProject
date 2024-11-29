// Enemy.cpp
#include "Enemy.h"

Enemy::Enemy(int id, float x, float y, int health) : id(id), posX(x), posY(y), health(health) {}

void Enemy::update() {
    // 적 상태 업데이트 로직 구현
}

int Enemy::getId() const { return id; }

float Enemy::getPositionX() const { return posX; }

float Enemy::getPositionY() const { return posY; }

int Enemy::getHealth() const { return health; }

void Enemy::takeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
}
