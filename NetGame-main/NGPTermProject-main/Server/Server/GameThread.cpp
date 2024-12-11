//#include "Common.h"
#include "GameThread.h"
#include <iostream>      // �ܼ� �����
#include <chrono>        // ������ �� �ð� ����
#include <thread>        // sleep_for ���
#include "stdafx.h"
#include  "list"
#include "Client.h"
std::vector<PlayerStatusPacket> PlayerStatus;  // ���� ��Ŷ ����Ʈ ����

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


   
        waitUntilNextFrame(frameStartTime);
    }
}

void GameThread::updatePlayerStatus() {
    EnterCriticalSection(&cs); // ���� �ڿ� ���� ����ȭ

    // �÷��̾� ���� ������Ʈ �� �浹 üũ
    for (auto& player : players) {
        // Ŭ���̾�Ʈ�κ��� ���� ���� ������ ��ġ ������Ʈ
        PlayerStatusPacket clientStatus = player.GetReceivedStatus();
        float newX = clientStatus.posX;
        float newY = clientStatus.posY;

        // �ٸ� �÷��̾� ����Ʈ ���� (���� �÷��̾� ����)
        std::vector<Player*> otherPlayers;
        for (auto& other : players) {
            if (&player != &other) { // �ڱ� �ڽ� ����
                otherPlayers.push_back(&other);
            }
        }

        // �浹 üũ
        bool collision = player.CheckCollision(newX, newY, obstacles, otherPlayers);
        if (!collision) {
            // �浹�� ������ ��ġ ������Ʈ
            player.SetPosition(newX, newY);
        }
        else {
            // �浹�� ������ ���� ó�� (��: ��ġ �ѹ�, ������ ���� ��)
            player.TakeDamage(1);
        }

        // �÷��̾� ���� ������Ʈ
        player.SetHealth(clientStatus.health);

        // ������Ʈ�� ���¸� ����ȭ�� ��Ŷ ����
        PlayerStatusPacket updatedStatus;
        updatedStatus.playerId = player.GetID();
        updatedStatus.posX = player.GetX();
        updatedStatus.posY = player.GetY();
        updatedStatus.health = player.GetHealth();

        // ���� ��Ŷ ����Ʈ�� �߰� (Ŭ���̾�Ʈ�� ���� �غ�)
        PlayerStatus.push_back(updatedStatus);
    }

    LeaveCriticalSection(&cs); // ����ȭ ����
}





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