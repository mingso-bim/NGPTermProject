// Bullet.cpp
#include "Bullet.h"

Bullet::Bullet(int id, float x, float y, float damage) : id(id), posX(x), posY(y), damage(damage), active(true) {}

void Bullet::update() {
    // �Ѿ� ���� ������Ʈ ���� ����
}

int Bullet::getId() const { return id; }

float Bullet::getPositionX() const { return posX; }

float Bullet::getPositionY() const { return posY; }

float Bullet::getDamage() const { return damage; }

void Bullet::setInactive() { active = false; }

bool Bullet::isActive() const { return active; }
