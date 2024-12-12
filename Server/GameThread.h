#pragma once
#include <vector>
#include <queue>
#include <winsock2.h> // �ùٸ� ��ġ�� �̵�
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Event.h"
#include "Packet.h"
#include "Utility.h"
#include "Obstacle.h" 
#include <chrono>
#include"stdafx.h"

class GameThread {
public:
    GameThread(); // �⺻ ������ �߰�
    GameThread(SOCKET serverSocket);
    ~GameThread();

    void run();  // ���� ������ ����
    void stop(); // ���� ������ ����

    void updateBulletStatus();
    void updatePlayerStatus();
    bool CheckCollision(const Player& player);

    // ���� ��ü ���
    std::vector<Player> players;
    std::vector<Enemy*> enemies;
    std::vector<Bullet> bullets;
    std::vector<Obstacle*> obstacles;
private:
    
    void waitUntilNextFrame(const std::chrono::time_point<std::chrono::steady_clock>& frameStartTime); // ������ �� ����ȭ

    bool gameRunning; // ���� ���� ���� ����
    std::queue<Event> eventQueue; // �̺�Ʈ ť


    // enemy �����Լ� (��������)
    void SpawnEnemy(float, Player);
    void SpawnBrainMonster(Player);
    void SpawnEyeMonster(Player);
    void SpawnBigBoomer(Player);
    void SpawnLamprey(Player);

    // enemy ���� ���� ����
    float enemySpawnTimer;
    float bigBoomerSpawnTimer;
    float lampreySpawnTimer;
    const float enemySpawnInterval = 10.0f;
    const float bigBoomerSpawnInterval = 30.0f;
    const float lampreySpawnInterval = 45.0f;
    const float yogSpawnInterval = 60.0f;

    // enemy update �Լ�
    void updateEnemy(float);

   

    // ��Ŷ ����
    vector<PlayerStatusPacket> makeSendPlayerPacket();

    // ���� ��� ���� ����
    SOCKET serverSocket; // ������ �κ�
};
