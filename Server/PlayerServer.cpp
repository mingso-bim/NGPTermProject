#include "Player.h"
#include"GameFrameWork.h"

#define PlayerWidth 20.0f
#define PlayerHeight 25.0f

extern HFONT hFont;

Player::Player(unsigned short id, float x, float y, float speed, float animationSpeed, GameFramework* gameFramework)
    : x(x), y(y), speed(speed), animationSpeed(animationSpeed), currentFrame(0), frameTimeAccumulator(0.0f),
    moveLeft(false), moveRight(false), moveUp(false), moveDown(false), isMoving(false),
    boundWidth(0), boundHeight(0), directionLeft(false),
    level(1), experience(0), experienceToNextLevel(100), levelUpEffectTime(0.0f),
    health(4), maxHealth(4), invincibilityTime(2.0f), currentInvincibilityTime(0.0f),
    heartAnimationFrame(0), heartAnimationSpeed(0.2f), heartAnimationAccumulator(0.0f),
    gameFramework(gameFramework) {
    //LoadImages();
}

Player::~Player() {
}

PlayerStatusPacket receivedStatus; // ��� ���� ����

PlayerStatusPacket Player::GetReceivedStatus() const {
    return receivedStatus;
}

void Player::SetReceivedStatus(const PlayerStatusPacket& status) {
    receivedStatus = status;
}
void Player::SetPosition(float x, float y) {
    this->x = x;
    this->y = y;
}
void Player::SetHealth(unsigned short health) {
    this->health = health;
    if (this->health > maxHealth) {
        this->health = maxHealth; // ü���� �ִ� ü���� �ʰ����� �ʵ��� ����
    }
}
unsigned short Player::GetID() const {
    return id; // �÷��̾��� ID ��� ���� ��ȯ
}
unsigned short Player::GetHealth() const {
    return health;
}
float Player::GetX() const {
    return x;
}
float Player::GetY() const {
    return y;
}


void Player::Update(float frameTime, const std::vector<Obstacle*>& obstacles, const std::vector<Player*>& otherPlayers) {

    frameTimeAccumulator += frameTime;
    levelUpEffectTime -= frameTime;
    UpdateInvincibility(frameTime);

    if (frameTimeAccumulator >= animationSpeed) {
        if (isMoving) {
            currentFrame = (currentFrame + 1) % 4; // Run �ִϸ��̼��� 4 �������̹Ƿ�
        }
        else {
            currentFrame = (currentFrame + 1) % 5; // Idle �ִϸ��̼��� 5 �������̹Ƿ�
        }
        frameTimeAccumulator = 0.0f;
    }

    isMoving = false;
    if (moveLeft) { Move(-speed, 0, obstacles, otherPlayers); isMoving = true; }
    if (moveRight) { Move(speed, 0, obstacles, otherPlayers); isMoving = true; }
    if (moveUp) { Move(0, -speed, obstacles, otherPlayers); isMoving = true; }
    if (moveDown) { Move(0, speed, obstacles, otherPlayers); isMoving = true; }
}

void Player::Move(float dx, float dy, const std::vector<Obstacle*>& obstacles, const std::vector<Player*>& otherPlayers) {

    float newX = x + dx;
    float newY = y + dy;

    if (!CheckCollision(newX, newY, obstacles, otherPlayers)) {
        x = newX;
        y = newY;
    }

    // ��踦 ����� �ʵ��� ��ġ ����
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x > boundWidth - PlayerWidth) x = boundWidth - PlayerWidth * 2;
    if (y > boundHeight - PlayerHeight) y = boundHeight - PlayerHeight * 2;
}

bool Player::CheckCollision(float newX, float newY, const std::vector<Obstacle*>& obstacles, const std::vector<Player*>& otherPlayers) const {
    // ��ֹ� �浹 üũ
    for (const auto& obstacle : obstacles) {
        float ox = obstacle->GetX();
        float oy = obstacle->GetY();
        float ow = obstacle->GetWidth();
        float oh = obstacle->GetHeight();

        if (newX < ox + ow &&
            newX + PlayerWidth > ox &&
            newY < oy + oh &&
            newY + PlayerHeight > oy) {
            return true;
        }
    }

    // �ٸ� �÷��̾���� �浹 üũ
    for (const auto& otherPlayer : otherPlayers) {
        if (this != otherPlayer) { // �ڱ� �ڽ� ����
            float px = otherPlayer->GetX();
            float py = otherPlayer->GetY();

            if (abs(newX - px) < PlayerWidth && abs(newY - py) < PlayerHeight) {
                return true;
            }
        }
    }

    return false; // �浹 ����
}


void Player::SetBounds(float width, float height) {
    boundWidth = width;
    boundHeight = height;
}


void Player::AddExperience(int amount) {
    experience += amount;
    //PlaySound(L"./resources/sounds/Obtain_Points.wav", NULL, SND_FILENAME | SND_ASYNC);
    while (experience >= experienceToNextLevel) {
        experience -= experienceToNextLevel;
        LevelUp();
    }
}

void Player::LevelUp() {
  //  PlaySound(L"./resources/sounds/LevelUp.wav", NULL, SND_FILENAME | SND_ASYNC);
    level++;
    // ����ġ �䱸�� = *1.5f
    experienceToNextLevel = static_cast<int>(experienceToNextLevel * 1.5f);
    levelUpEffectTime = levelUpEffectDuration;

    // ������ �� �� ����߰� //@@@@@@@@@@@@@@@@�����ӿ�ũ ������ ���߿� �̿�
    //if (gameFramework) {
     //   gameFramework->LevelUpUpgrade();
    //}
}

bool Player::IsInvincible() const {
    return currentInvincibilityTime > 0;
}

void Player::UpdateInvincibility(float frameTime) {
    if (currentInvincibilityTime > 0) {
        currentInvincibilityTime -= frameTime;
        if (currentInvincibilityTime < 0) currentInvincibilityTime = 0;
    }
}

void Player::TakeDamage(int amount) {
    if (!IsInvincible()) {
      //  PlaySound(L"./resources/sounds/hurt.wav", NULL, SND_FILENAME | SND_ASYNC);
        health -= amount;
        if (health < 0) health = 0;
        currentInvincibilityTime = invincibilityTime;
    }
}

void Player::ApplyUpgrade(const std::wstring& upgrade) {
    if (upgrade == L"MaxHp +1") {
        maxHealth += 1;
        health += 1;
    }
    else if (upgrade == L"Add Speed") {
        speed += 0.5f;
    }
    // Other upgrade cases...
}