//#include "Common.h"
#include "GameThread.h"
#include <iostream>      // �ܼ� �����
#include <chrono>        // ������ �� �ð� ����
#include <thread>        // sleep_for ���
#include "stdafx.h"



void GameThread::run() {
    gameRunning = true;

    while (gameRunning) {
        auto frameStartTime = std::chrono::steady_clock::now(); // ������ ���� �ð� ���

        // 1. �̺�Ʈ ����
        collectEvents();

        // 2. �̺�Ʈ ���� �� ó��
        sortEventsByFrame();
        // while (!eventQueue.empty()) {
           //  Event currentEvent = eventQueue.front();
           //  processEvent(currentEvent);
         //   eventQueue.pop();
       //  }

         // 3. �浹 ����
         //checkCollisions();

         // 4. ���� ��ü ������Ʈ
         //updateGameObjects();

         // 5. Ŭ���̾�Ʈ ���� ����ȭ
         //sendUpdatedStateToClients();

         // 6. ������ �� ����ȭ (�ʴ� 30������ ����)
        waitUntilNextFrame(frameStartTime);
    }
}
void GameThread::collectEvents() {
    // Ŭ���̾�Ʈ�κ��� �̺�Ʈ ����
    // ���⼭ �� Ŭ���̾�Ʈ�� �����ϴ� ��Ŷ�� �а�, �̺�Ʈ ť�� �߰�
}

void GameThread::sortEventsByFrame() {
    // �̺�Ʈ ť�� ������ �������� ���� (��: ������ ��ȣ �Ǵ� Ÿ�ӽ�����)
    // �̸� ���� ������ ����ǵ��� �����մϴ�.
}
/*void GameThread::processEvent(const Event& event) {
    // �̺�Ʈ�� ������ ���� �ٸ� �۾� ����
    switch (event.type) {
   // case EventType::PlayerMove:
        // �÷��̾� �̵� ó��
        break;
  //  case EventType::FireBullet:
        // �Ѿ� �߻� ó��
        break;
   // case EventType::CollectItem:
        // ������ ȹ�� ó��
        break;
        // �ʿ��� ��� �� ���� �̺�Ʈ Ÿ�� ó��
    }
}*/
/*void GameThread::updateGameObjects() {
    for (auto& player : players) {
        player.update(); // �÷��̾� ������Ʈ (��ġ, ���� ��)
    }

    for (auto& enemy : enemies) {
        enemy.update(); // �� ������Ʈ
    }

    for (auto& bullet : bullets) {
        bullet.update(); // �Ѿ� ������Ʈ
    }
}*/
/*void GameThread::checkCollisions() {
    // �� ��ü ���� �浹�� �˻��ϰ� �浹 �̺�Ʈ ó��
    for (auto& bullet : bullets) {
        for (auto& enemy : enemies) {
            if (checkCollision(bullet.getPositionX(), bullet.getPositionY(), enemy.getPositionX(), enemy.getPositionY(), 1.0f, 1.0f)) {
                // �浹 �� ó��
                enemy.takeDamage(bullet.getDamage());
                bullet.setInactive(); // �Ѿ� ��Ȱ��ȭ
            }
        }
    }

    // �÷��̾�� ��ֹ�, ������ �浹 � �߰������� ó��
}*/
/*void GameThread::sendUpdatedStateToClients() {
    // Ŭ���̾�Ʈ�鿡�� ���� ��ü�� �ֽ� ���� ����
    for (const auto& player : players) {
        PlayerStatusPacket packet = { player.getId(), player.getHealth(), player.getPositionX(), player.getPositionY() };
        send(serverSocket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
    }
}*/
void GameThread::waitUntilNextFrame(const std::chrono::time_point<std::chrono::steady_clock>& frameStartTime) {
    auto frameEndTime = std::chrono::steady_clock::now();
    auto frameDuration = std::chrono::milliseconds(33); // 30fps ���� (1000ms / 30)
    auto timeToWait = frameDuration - (frameEndTime - frameStartTime);

    if (timeToWait > std::chrono::milliseconds(0)) {
        std::this_thread::sleep_for(timeToWait);
    }
}
