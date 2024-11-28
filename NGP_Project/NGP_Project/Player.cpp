#include "Player.h"

Player::Player(int id, float x, float y, int health) : id(id), posX(x), posY(y), health(health) {}

void Player::update() {
    // �÷��̾� ���� ������Ʈ ���� ����
}

int Player::getId() const { return id; }

float Player::getPositionX() const { return posX; }

float Player::getPositionY() const { return posY; }

int Player::getHealth() const { return health; }

void Player::takeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
}
