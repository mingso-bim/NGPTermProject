#pragma once

#include "Bullet.h"
#include "Packet.h"

class BulletServer : public Bullet {
public:
    BulletServer(float x, float y, float targetX, float targetY, int damage, float speed);

    // ���� ������Ʈ
    void Update(float frameTime);

    // �浹 ó��
    bool CheckCollision(float enemyX, float enemyY, float enemyWidth, float enemyHeight) const;

    // �Ѿ� ��Ȱ��ȭ ����
   // bool IsInactive() const;
    bool IsOutOfBounds(float width, float height) const;
    // ���� ��Ŷ ����
    s_bulletPacket GenerateStatePacket() const;

private:
    bool isActive; // �Ѿ� Ȱ��ȭ ����
};
