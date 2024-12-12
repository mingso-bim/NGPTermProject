#pragma once

#include <atlimage.h>
#include <vector>
#include "Obstacle.h"
#include "Enemy.h"      
#include "Camera.h"
#include <string>
#include "Packet.h"
class GameFramework;

class Player {
public:
    Player(unsigned short id, float x, float y, float speed, float animationSpeed, GameFramework* gameFramework);
    ~Player();

    void Update(float frameTime,  const std::vector<Obstacle*>& obstacles, const std::vector<Player*>& otherPlayers);
    void Move(float dx, float dy, const std::vector<Obstacle*>& obstacles, const std::vector<Player*>& otherPlayers);

    float GetX() const;
    float GetY() const;

    void SetBounds(float width, float height);
  //  void DrawBoundingBox(HDC hdc, float offsetX, float offsetY) const;

   // void LoadImages();
    void Draw(HDC hdc, float offsetX, float offsetY);

    void SetDirectionLeft(bool isLeft);
    bool IsDirectionLeft() const;

    void AddExperience(int amount);
   // void DrawExperienceBar(HDC hdc, RECT clientRect);

    void LevelUp();
    bool CheckCollision(float newX, float newY, const std::vector<Obstacle*>& obstacles, const std::vector<Player*>& otherPlayers) const;

    void ApplyUpgrade(const std::wstring& upgrade);

    unsigned short id;
    float x, y;
    float speed;
    float animationSpeed;
    int currentFrame;
    float frameTimeAccumulator;
    bool moveLeft, moveRight, moveUp, moveDown;
    bool isMoving;
    bool directionLeft;

    float boundWidth, boundHeight;

    int level;
    int experience;
    int experienceToNextLevel;

    CImage idleImages[5];
    CImage runImages[4];
    CImage r_idleImages[5];
    CImage r_runImages[4];

    std::vector<CImage> levelUpEffectImages;
    float levelUpEffectTime;
    const float levelUpEffectDuration = 1.5f;

    // New health-related attributes
    unsigned short health;
    unsigned short maxHealth;
    float invincibilityTime;
    float currentInvincibilityTime;

    // Heart animation frames
    std::vector<CImage> heartImages;
    int heartAnimationFrame;
    float heartAnimationSpeed;
    float heartAnimationAccumulator;

    // New methods
    void TakeDamage(int amount);
   // void DrawHealth(HDC hdc, float offsetX, float offsetY);
   // void DrawInvincibilityIndicator(HDC hdc, float offsetX, float offsetY);
    bool IsInvincible() const;
    void UpdateInvincibility(float frameTime);

    GameFramework* gameFramework;

    // 입력 처리
    void ProcessInput(const c_inputPacket& input, const std::vector<Player*>& otherPlayers);

    // 상태 패킷 생성
    s_playerPacket GenerateStatePacket() const;
    PlayerStatusPacket receivedStatus;

    PlayerStatusPacket GetReceivedStatus() const;
    void SetReceivedStatus(const PlayerStatusPacket& status);

    // 위치 관련
    void SetPosition(float x, float y);

    // 체력 관련
    void SetHealth(unsigned short health);
    unsigned short GetHealth() const;

    // ID 관련
    unsigned short GetID() const;
};
