#pragma once

#include "Bullet.h"
#include "Packet.h"

class ClientBullet : public Bullet {
public:
    ClientBullet(float x, float y, float targetX, float targetY, int damage, float speed);

    // �����κ��� ���� ����
    void ReceiveStateFromServer(const s_bulletPacket& packet);

    bool IsActive() const;

    void Update(float frameTime);
    void Draw(HDC hdc, float offsetX, float offsetY);
    bool IsOutOfBounds(float width, float height) const;
    bool CheckCollision(float enemyX, float enemyY, float enemyWidth, float enemyHeight) const;
    int GetDamage() const;

    bool isHit;
    bool isEffectFinished() const;

private:
    void UpdateHitEffect(float frameTime);
    void DrawHitEffect(HDC hdc, float offsetX, float offsetY);

    //bool isActive; // �Ѿ� Ȱ�� ����

    CImage bulletImage;

    float hitEffectDuration;
    float hitEffectTime;
    std::vector<CImage> hitEffectImages;
};
class RevolverBullet : public Bullet {
public:
    RevolverBullet(float x, float y, float targetX, float targetY);
};

class HeadshotGunBullet : public Bullet {
public:
    HeadshotGunBullet(float x, float y, float targetX, float targetY);
};

class ClusterGunBullet : public Bullet {
public:
    ClusterGunBullet(float x, float y, float targetX, float targetY);
};

class DualShotgunBullet : public Bullet {
public:
    DualShotgunBullet(float x, float y, float targetX, float targetY, float spreadAngle);
    float directionX, directionY;
};
