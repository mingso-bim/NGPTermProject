#pragma once
#include <vector>
#include <queue>
#include <winsock2.h> // 올바른 위치로 이동
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
    GameThread(); // 기본 생성자 추가
    GameThread(SOCKET serverSocket);
    ~GameThread();

    void run();  // 게임 루프를 실행
    void stop(); // 게임 루프를 종료

    void updatePlayerStatus();
    bool CheckCollision(const Player& player);
private:
    
    void waitUntilNextFrame(const std::chrono::time_point<std::chrono::steady_clock>& frameStartTime); // 프레임 간 동기화

    bool gameRunning; // 게임 루프 실행 여부
    std::queue<Event> eventQueue; // 이벤트 큐

    // 게임 객체 목록
    std::vector<Player> players;
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    std::vector<Obstacle*> obstacles; 

    // 서버 통신 관련 소켓
    SOCKET serverSocket; // 수정된 부분
};
