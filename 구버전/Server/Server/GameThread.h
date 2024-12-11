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

private:
    void collectEvents();  // Ŭ���̾�Ʈ �̺�Ʈ ����
    void sortEventsByFrame(); // �̺�Ʈ�� ������ �������� ����
    void processEvent(const Event& event); // ������ �̺�Ʈ ó��
    void updateGameObjects();  // ���� ��ü ������Ʈ
    void checkCollisions(); // �浹 ���� �� ó��
    void sendUpdatedStateToClients(); // Ŭ���̾�Ʈ�� ���� ����
    void waitUntilNextFrame(const std::chrono::time_point<std::chrono::steady_clock>& frameStartTime); // ������ �� ����ȭ

    bool gameRunning; // ���� ���� ���� ����
    std::queue<Event> eventQueue; // �̺�Ʈ ť

    // ���� ��ü ���
    std::vector<Player> players;
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    std::vector<Obstacle*> obstacles; 

    // ���� ��� ���� ����
    SOCKET serverSocket; // ������ �κ�
};
