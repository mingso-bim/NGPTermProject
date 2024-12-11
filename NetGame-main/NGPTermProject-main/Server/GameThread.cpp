//#include "Common.h"
#include "GameThread.h"
#include <iostream>      // 콘솔 입출력
#include <chrono>        // 프레임 간 시간 측정
#include <thread>        // sleep_for 사용
#include "stdafx.h"
#include  "list"
#include "Client.h"
std::vector<PlayerStatusPacket> PlayerStatus;  // 서버 패킷 리스트 선언

extern CRITICAL_SECTION cs;         // Critical Section 전역 변수 정의
// 30FPS 기준
#define FRAME_TIME 0.033f

GameThread::GameThread() {
    // 기본 생성자 정의 (필요에 따라 초기화 작업 추가)
    serverSocket = INVALID_SOCKET;
    std::cout << "기본 GameThread 생성자 호출" << std::endl;
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
    EnterCriticalSection(&cs); // 공유 자원 접근 동기화

    // 플레이어 상태 업데이트 및 충돌 체크
    for (auto& player : players) {
        // 클라이언트로부터 받은 상태 정보로 위치 업데이트
        PlayerStatusPacket clientStatus = player.GetReceivedStatus();
        float newX = clientStatus.posX;
        float newY = clientStatus.posY;

        // 다른 플레이어 리스트 생성 (현재 플레이어 제외)
        std::vector<Player*> otherPlayers;
        for (auto& other : players) {
            if (&player != &other) { // 자기 자신 제외
                otherPlayers.push_back(&other);
            }
        }

        // 충돌 체크
        bool collision = player.CheckCollision(newX, newY, obstacles, otherPlayers);
        if (!collision) {
            // 충돌이 없으면 위치 업데이트
            player.SetPosition(newX, newY);
        }
        else {
            // 충돌이 있으면 예외 처리 (예: 위치 롤백, 데미지 적용 등)
            player.TakeDamage(1);
        }

        // 플레이어 상태 업데이트
        player.SetHealth(clientStatus.health);

        // 업데이트된 상태를 동기화할 패킷 생성
        PlayerStatusPacket updatedStatus;
        updatedStatus.playerId = player.GetID();
        updatedStatus.posX = player.GetX();
        updatedStatus.posY = player.GetY();
        updatedStatus.health = player.GetHealth();

        // 서버 패킷 리스트에 추가 (클라이언트로 전송 준비)
        PlayerStatus.push_back(updatedStatus);
    }

    LeaveCriticalSection(&cs); // 동기화 종료
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