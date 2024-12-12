//#include "Common.h"
#include "GameThread.h"
#include <iostream>      // �ܼ� �����
#include <chrono>        // ������ �� �ð� ����
#include <thread>        // sleep_for ���
#include "stdafx.h"
#include  "list"
#include "Client.h"
#include<vector>

//std::vector<c_playerPacket> sharedPlayerPackets;
std::vector<c_bulletPacket> sharedBulletPackets = {};
extern CRITICAL_SECTION cs;         // Critical Section ���� ���� ����
// 30FPS ����
#define FRAME_TIME 0.033f

GameThread::GameThread() {
    // �⺻ ������ ���� (�ʿ信 ���� �ʱ�ȭ �۾� �߰�)
    serverSocket = INVALID_SOCKET;
    std::cout << "�⺻ GameThread ������ ȣ��" << std::endl;
}

GameThread::GameThread(SOCKET serverSocket)
    : serverSocket(serverSocket), gameRunning(false) {}

GameThread::~GameThread() {
    stop();
}


void GameThread::run() {
    gameRunning = true;
    cout << "gameThread run()" << endl;
    while (gameRunning) {
        auto frameStartTime = std::chrono::steady_clock::now();

    //1. updateEnemys
    //2. updateBullet
    //3. collisionAction
       //cout << "updatePlayerStatus()" << endl;
       //updatePlayerStatus();
       //updateBulletStatus();

        updateEnemy(0.016f);
        for (auto player : players) {
            SpawnEnemy(0.016f, player);
        }
        cout << enemies.size() << "���� enemy spawn" << endl;

       Sleep(32);
   
        waitUntilNextFrame(frameStartTime);
    }
}
//
//id GameThread::updatePlayerStatus() {
//  EnterCriticalSection(&cs); // ����ȭ
//  for (const auto& c_player : receivedPlayerPackets) {
//      s_playerPacket s_player = {};
//      s_player.s_playerID = c_player.c_playerID;
//      s_player.s_playerPosX = c_player.c_playerPosX;
//      s_player.s_playerPosY = c_player.c_playerPosY;
//
//      
//      sendPlayers.push_back(s_player); // ���� ���Ϳ� �߰�
//      std::cout 
//          << "[LOG(GameTread)] ID=" << s_player.s_playerID
//          << ", PosX=" << s_player.s_playerPosX
//          << ", PosY=" << s_player.s_playerPosY << std::endl;
//      
//  }
//  receivedPlayerPackets.clear(); // ó���� ������
//  LeaveCriticalSection(&cs); // ����ȭ ����
//
//
//id GameThread::updateBulletStatus()
//
//  EnterCriticalSection(&cs); // ����ȭ
//  for (const auto& c_bullet : receivedBulletPackets) {
//      s_bulletPacket s_bullet = {};
//      s_bullet.s_playerX = c_bullet.c_playerX;
//      s_bullet.s_playerY = c_bullet.c_playerY;
//      s_bullet.s_targetX = c_bullet.c_targetX;
//      s_bullet.s_targetY = c_bullet.c_targetY;
//
//      // ������Ʈ�� s_bulletPacket�� bullets ���Ϳ� �߰�
//      Sendbullets.push_back(s_bullet);
//
//      std::cout << "[LOG(GameThread)] Bullet Packet Received: PlayerX=" << s_bullet.s_playerX
//          << ", PlayerY=" << s_bullet.s_playerY
//          << ", DirX=" << s_bullet.s_targetX
//          << ", DirY=" << s_bullet.s_targetY << std::endl;
//  }
//  receivedBulletPackets.clear(); // ó���� ������ ����
//  LeaveCriticalSection(&cs); // ����ȭ ����
//




void GameThread::waitUntilNextFrame(const std::chrono::time_point<std::chrono::steady_clock>& frameStartTime) {
    auto frameEndTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsed = frameEndTime - frameStartTime;

    if (elapsed.count() < FRAME_TIME) {
        std::this_thread::sleep_for(std::chrono::duration<float>(FRAME_TIME - elapsed.count()));
    }
}

void GameThread::stop() {
    gameRunning = false;
}

vector<PlayerStatusPacket> GameThread::makeSendPlayerPacket()
{
    // �ۼ� �ʿ�
    return vector<PlayerStatusPacket>();
}



// s_playerPacket�� ������Ʈ
//vector<s_playerPacket> GameThread::makePlayerPacket()
//{
//    s_playerPacket packet = {};
//    for (const Player& p : players)
//    {
//        packet.s_playerID = p.GetID();
//        packet.s_playerPosX = p.GetX();
//        packet.s_playerPosY = p.GetY();
//        packet.s_playerSpeed = p.speed;
//        packet.s_playerHealth = p.GetHealth();
//        packet.s_playerLevel = p.level;
//        packet.s_playerEXP = p.experience;
//    }
//    return vector<s_playerPacket>();
//}


// enemy ����

void GameThread::SpawnEnemy(float frameTime, Player player) {
    enemySpawnTimer += frameTime;
    if (enemySpawnTimer >= enemySpawnInterval) {
        for (int i = 0; i < 10; i++) {
            SpawnBrainMonster(player);
            SpawnEyeMonster(player);
        }
        enemySpawnTimer = 0.0f;
    }

    bigBoomerSpawnTimer += frameTime;
    if (bigBoomerSpawnTimer >= bigBoomerSpawnInterval) {
        SpawnBigBoomer(player);
        SpawnBigBoomer(player);
        SpawnBigBoomer(player);
        bigBoomerSpawnTimer = 0.0f;
    }

    lampreySpawnTimer += frameTime;
    if (lampreySpawnTimer >= lampreySpawnInterval) {
        SpawnLamprey(player);
        SpawnLamprey(player);
        lampreySpawnTimer = 0.0f;
    }
}

void GameThread::SpawnBrainMonster(Player player) {
    float spawnRadius = 600.0f;

    float angle = (rand() % 360) * 3.14159265358979323846 / 180.0;
    float spawnX = player.GetX() + spawnRadius * cos(angle);
    float spawnY = player.GetY() + spawnRadius * sin(angle);

    enemies.push_back(new BrainMonster(player.GetID(), spawnX, spawnY, 5.0f));
}

void GameThread::SpawnEyeMonster(Player player) {
    float spawnRadius = 600.0f;

    float angle = (rand() % 360) * 3.14159265358979323846 / 180.0;
    float spawnX = player.GetX() + spawnRadius * cos(angle);
    float spawnY = player.GetY() + spawnRadius * sin(angle);

    enemies.push_back(new EyeMonster(spawnX, spawnY, 5.0f));
}

void GameThread::SpawnBigBoomer(Player player) {
    float spawnRadius = 600.0f;

    float angle = (rand() % 360) * 3.14159265358979323846 / 180.0;
    float spawnX = player.GetX() + spawnRadius * cos(angle);
    float spawnY = player.GetY() + spawnRadius * sin(angle);

    enemies.push_back(new BigBoomer(spawnX, spawnY, 5.0f));
}

void GameThread::SpawnLamprey(Player player) {
    float spawnRadius = 600.0f;

    for (int i = 0; i < 2; ++i) {
        float angle = (rand() % 360) * 3.14159265358979323846 / 180.0;
        float spawnX = player.GetX() + spawnRadius * cos(angle);
        float spawnY = player.GetY() + spawnRadius * sin(angle);

        enemies.push_back(new Lamprey(spawnX, spawnY, 5.0f));
    }
}


void GameThread::updateEnemy(float frameTime) {
    auto enemyIter = enemies.begin();
    while (enemyIter != enemies.end()) {
        Enemy* enemy = *enemyIter;
        for (auto player : players) {
            if (player.GetID() == enemy->GetAimPlayerID()) {
                enemy->Update(frameTime, player.GetX(), player.GetY(), obstacles);
            }
        }
        if (enemy->IsDead()) {
            delete enemy;
            enemyIter = enemies.erase(enemyIter);
        }
        else {
            ++enemyIter;
        }
    }
}