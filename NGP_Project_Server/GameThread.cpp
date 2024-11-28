//#include "Common.h"
#include "GameThread.h"
#include <iostream>      // 콘솔 입출력
#include <chrono>        // 프레임 간 시간 측정
#include <thread>        // sleep_for 사용
#include "stdafx.h"
#include  "list"
#include "Client.h"

extern list<Client> waitClientList; // 클라이언트 리스트 전역 변수 정의
extern CRITICAL_SECTION cs;         // Critical Section 전역 변수 정의
// 30FPS 기준
#define FRAME_TIME 0.033f

GameThread::GameThread(SOCKET serverSocket)
    : serverSocket(serverSocket), gameRunning(false) {}

GameThread::~GameThread() {
    stop();
}


void GameThread::run() {
    gameRunning = true;

    while (gameRunning) {
        auto frameStartTime = std::chrono::steady_clock::now();

        // 1. 이벤트 수집
        //collectEvents();

        // 2. 이벤트 정렬 및 처리
      //  sortEventsByFrame();
       // while (!eventQueue.empty()) {
       //     Event currentEvent = eventQueue.front();
       //     processEvent(currentEvent);
      //      eventQueue.pop();
      //  }

        // 3. 게임 객체 업데이트
        updateGameObjects();

        // 4. 충돌 감지
        //checkCollisions();

        // 5. 클라이언트 상태 동기화
        sendUpdatedStateToClients();

        // 6. 프레임 간 동기화
        waitUntilNextFrame(frameStartTime);
    }
}

void GameThread::stop() {
    gameRunning = false;
}

void GameThread::collectEvents() {
    // 클라이언트로부터 이벤트 수집
    // 각 클라이언트에서 받은 패킷을 이벤트 큐에 추가
}

void GameThread::sortEventsByFrame() {
    // 이벤트 큐를 타임스탬프 또는 프레임 기준으로 정렬
}

/*
void GameThread::processEvent(const Event& event) {
    switch (event.type) {
    case EventType::PlayerMove:
        for (auto& player : players) {
            if (player.GetID() == event.playerID) {
                player.ProcessInput(event.inputData); // 입력 처리
            }
        }
        break;

    case EventType::FireBullet:
        bullets.emplace_back(event.bulletData.startX, event.bulletData.startY,
            event.bulletData.targetX, event.bulletData.targetY,
            event.bulletData.damage, event.bulletData.speed);
        break;

        // 추가 이벤트 처리 가능
    }
}
*/
void GameThread::updateGameObjects() {
    for (auto& player : players) {
        // 1. 플레이어 입력 패킷 가져오기
        EnterCriticalSection(&cs); // cs 시작
        if (!waitClientList.empty()) {
            const c_inputPacket& inputPacket = waitClientList.front().inputPacket;
            // 디버깅 로그 // 왜 안될까요...
            std::cout << "Input Packet: moveLeft=" << inputPacket.moveLeft
                << " moveRight=" << inputPacket.moveRight
                << " moveUp=" << inputPacket.moveUp
                << " moveDown=" << inputPacket.moveDown << std::endl;

            // 플레이어 업데이트
            player.Update(FRAME_TIME, inputPacket, obstacles);
        }
        LeaveCriticalSection(&cs); // csn 종료
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