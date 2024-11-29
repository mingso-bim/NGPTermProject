//#include "Common.h"
#include "GameThread.h"
#include <iostream>      // �ܼ� �����
#include <chrono>        // ������ �� �ð� ����
#include <thread>        // sleep_for ���
#include "stdafx.h"
#include  "list"
#include "Client.h"
extern list<c_inputPacket> sharedInputList; // �������� �ѹ� �Ẹ��
extern list<Client> waitClientList; // Ŭ���̾�Ʈ ����Ʈ ���� ���� ����
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
    //4. updatePlayerStatus
       // cout << "updateGameObjects()" << endl;
        // 3. ���� ��ü ������Ʈ
        updateGameObjects();

        // 4. �浹 ����
        //checkCollisions();

        // 5. Ŭ���̾�Ʈ ���� ����ȭ
        sendUpdatedStateToClients();

        // 6. ������ �� ����ȭ
        waitUntilNextFrame(frameStartTime);
    }
}

void GameThread::stop() {
    gameRunning = false;
}

void GameThread::collectEvents() {
    // Ŭ���̾�Ʈ�κ��� �̺�Ʈ ����
    // �� Ŭ���̾�Ʈ���� ���� ��Ŷ�� �̺�Ʈ ť�� �߰�
}

void GameThread::sortEventsByFrame() {
    // �̺�Ʈ ť�� Ÿ�ӽ����� �Ǵ� ������ �������� ����
}

/*
void GameThread::processEvent(const Event& event) {
    switch (event.type) {
    case EventType::PlayerMove:
        for (auto& player : players) {
            if (player.GetID() == event.playerID) {
                player.ProcessInput(event.inputData); // �Է� ó��
            }
        }
        break;

    case EventType::FireBullet:
        bullets.emplace_back(event.bulletData.startX, event.bulletData.startY,
            event.bulletData.targetX, event.bulletData.targetY,
            event.bulletData.damage, event.bulletData.speed);
        break;

        // �߰� �̺�Ʈ ó�� ����
    }
}
*/
void GameThread::updateGameObjects() {
    EnterCriticalSection(&cs);
    if (!sharedInputList.empty()) {
        // ���� �Է� ����Ʈ���� �Է� ��Ŷ ��������
        c_inputPacket inputPacket = sharedInputList.front();
        sharedInputList.pop_front();  // ������ �Է� ��Ŷ ����
        LeaveCriticalSection(&cs);

        // �� �÷��̾� ������Ʈ ������Ʈ
        for (auto& player : players) {
            // �÷��̾� �Է��� �ݿ��� ������Ʈ
            player.Update(FRAME_TIME, inputPacket, obstacles);

            // ����� �α�
            std::cout << "���� ���� ó��: moveLeft=" << inputPacket.moveLeft
                << ", moveRight=" << inputPacket.moveRight
                << ", moveUp=" << inputPacket.moveUp
                << ", moveDown=" << inputPacket.moveDown << std::endl;
        }
    }
    else {
        //std::cerr << "Warning: sharedInputList is empty!" << std::endl;
        LeaveCriticalSection(&cs);
    }


    for (auto& enemy : enemies) {
     //   enemy.Update(FRAME_TIME);
    }

    for (auto& bullet : bullets) {
    //   bullet.Update(FRAME_TIME);
    }
}
/*
void GameThread::checkCollisions() {
    for (auto& bullet : bullets) {
        for (auto& enemy : enemies) {
            if (bullet.CheckCollision(enemy.GetX(), enemy.GetY(), enemy.GetWidth(), enemy.GetHeight())) {
                enemy.TakeDamage(bullet.getDamage());
                bullet.SetInactive();
            }
        }
    }
}
*/
void GameThread::sendUpdatedStateToClients() {
    for (const auto& player : players) {
        s_playerPacket playerPacket = player.GenerateStatePacket();
        send(serverSocket, reinterpret_cast<char*>(&playerPacket), sizeof(playerPacket), 0);
    }

    /*
    for (const auto& bullet : bullets) {
        if (!bullet.IsInactive()) {
            s_bulletPacket bulletPacket = bullet.GenerateStatePacket();
            send(serverSocket, reinterpret_cast<char*>(&bulletPacket), sizeof(bulletPacket), 0);
        }
    }

    for (const auto& enemy : enemies) {
        s_enemyPacket enemyPacket = enemy.GenerateStatePacket();
        send(serverSocket, reinterpret_cast<char*>(&enemyPacket), sizeof(enemyPacket), 0);
    }
    */
}


void GameThread::waitUntilNextFrame(const std::chrono::time_point<std::chrono::steady_clock>& frameStartTime) {
    auto frameEndTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsed = frameEndTime - frameStartTime;

    if (elapsed.count() < FRAME_TIME) {
        std::this_thread::sleep_for(std::chrono::duration<float>(FRAME_TIME - elapsed.count()));
    }
}