#pragma once
#include <string>

class Bullet {
public:
    Bullet(int id, float x, float y, float damage);
    void update(); // �Ѿ� ���� ������Ʈ
    int getId() const;
    float getPositionX() const;
    float getPositionY() const;
    float getDamage() const;
    void setInactive();
    bool isActive() const;

private:
    int id;
    float posX;
    float posY;
    float damage;
    bool active;
};