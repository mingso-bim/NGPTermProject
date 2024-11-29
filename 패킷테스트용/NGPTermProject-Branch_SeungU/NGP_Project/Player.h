#pragma once
#include <string>

class Player {
public:
    Player(int id, float x, float y, int health);
    void update(); // �÷��̾� ���� ������Ʈ
    int getId() const;
    float getPositionX() const;
    float getPositionY() const;
    int getHealth() const;
    void takeDamage(int damage);

private:
    int id;
    float posX;
    float posY;
    int health;
};