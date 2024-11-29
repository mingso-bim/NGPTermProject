#include "Player.h"
#include"GameFrameWork.h"

#define PlayerWidth 20.0f
#define PlayerHeight 25.0f

extern HFONT hFont;

Player::Player(float x, float y, float speed, float animationSpeed, GameFramework* gameFramework)
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

void Player::ProcessInput(const c_inputPacket& input) {
    if (input.c_key == "W") Move(0, -speed, {});
    if (input.c_key == "A") Move(-speed, 0, {});
    if (input.c_key == "S") Move(0, speed, {});
    if (input.c_key == "D") Move(speed, 0, {});
}

s_playerPacket Player::GenerateStatePacket() const {
    s_playerPacket packet;
    packet.s_playerName = name;
    packet.s_playerID = id;
    packet.s_playerPosX = x;
    packet.s_playerPosY = y;
    packet.s_playerSpeed = speed;
    packet.s_playerHealth = health;
    packet.s_playerLevel = 1; // �ӽ� ��
    packet.s_playerEXP = 0;   // �ӽ� ��
    packet.s_isPlayerDead = (health <= 0);
    return packet;
}

void Player::Update(float frameTime,const c_inputPacket& input, const std::vector<Obstacle*>& obstacles) {

    // Ŭ���̾�Ʈ�κ��� ������ �Է� ������ ������� �̵� ó��
    moveLeft = input.moveLeft;
    moveRight = input.moveRight;
    moveUp = input.moveUp;
    moveDown = input.moveDown;

    // ���� ������ �� �ִϸ��̼� ������Ʈ
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

    // �̵� ó��
    isMoving = false;
    if (moveLeft) { Move(-speed, 0, obstacles); isMoving = true; }
    if (moveRight) { Move(speed, 0, obstacles); isMoving = true; }
    if (moveUp) { Move(0, -speed, obstacles); isMoving = true; }
    if (moveDown) { Move(0, speed, obstacles); isMoving = true; }
}

void Player::Move(float dx, float dy, const std::vector<Obstacle*>& obstacles) {

    float newX = x + dx;
    float newY = y + dy;

    if (!CheckCollision(newX, newY, obstacles)) {
        x = newX;
        y = newY;
    }

    // ��踦 ����� �ʵ��� ��ġ ����
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x > boundWidth - PlayerWidth) x = boundWidth - PlayerWidth * 2;
    if (y > boundHeight - PlayerHeight) y = boundHeight - PlayerHeight * 2;
}

bool Player::CheckCollision(float newX, float newY, const std::vector<Obstacle*>& obstacles) const {
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
    return false;
}

void Player::SetBounds(float width, float height) {
    boundWidth = width;
    boundHeight = height;
}

float Player::GetX() const {
    return x;
}

float Player::GetY() const {
    return y;
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