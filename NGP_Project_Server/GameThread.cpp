//#include "Common.h"
#include "GameThread.h"
#include <iostream>      // 콘솔 입출력
#include <chrono>        // 프레임 간 시간 측정
#include <thread>        // sleep_for 사용
#include "stdafx.h"



void GameThread::run() {
    gameRunning = true;

    while (gameRunning) {
        auto frameStartTime = std::chrono::steady_clock::now(); // 프레임 시작 시간 기록

        // 1. 이벤트 수집
        collectEvents();

        // 2. 이벤트 정렬 및 처리
        sortEventsByFrame();
        // while (!eventQueue.empty()) {
           //  Event currentEvent = eventQueue.front();
           //  processEvent(currentEvent);
         //   eventQueue.pop();
       //  }

         // 3. 충돌 감지
         //checkCollisions();

         // 4. 게임 객체 업데이트
         //updateGameObjects();

         // 5. 클라이언트 상태 동기화
         //sendUpdatedStateToClients();

         // 6. 프레임 간 동기화 (초당 30프레임 유지)
        waitUntilNextFrame(frameStartTime);
    }
}
void GameThread::collectEvents() {
    // 클라이언트로부터 이벤트 수집
    // 여기서 각 클라이언트가 전송하는 패킷을 읽고, 이벤트 큐에 추가
}

void GameThread::sortEventsByFrame() {
    // 이벤트 큐를 프레임 기준으로 정렬 (예: 프레임 번호 또는 타임스탬프)
    // 이를 통해 순서가 보장되도록 정렬합니다.
}
/*void GameThread::processEvent(const Event& event) {
    // 이벤트의 종류에 따라 다른 작업 수행
    switch (event.type) {
   // case EventType::PlayerMove:
        // 플레이어 이동 처리
        break;
  //  case EventType::FireBullet:
        // 총알 발사 처리
        break;
   // case EventType::CollectItem:
        // 아이템 획득 처리
        break;
        // 필요한 경우 더 많은 이벤트 타입 처리
    }
}*/
/*void GameThread::updateGameObjects() {
    for (auto& player : players) {
        player.update(); // 플레이어 업데이트 (위치, 상태 등)
    }

    for (auto& enemy : enemies) {
        enemy.update(); // 적 업데이트
    }

    for (auto& bullet : bullets) {
        bullet.update(); // 총알 업데이트
    }
}*/
/*void GameThread::checkCollisions() {
    // 각 객체 간의 충돌을 검사하고 충돌 이벤트 처리
    for (auto& bullet : bullets) {
        for (auto& enemy : enemies) {
            if (checkCollision(bullet.getPositionX(), bullet.getPositionY(), enemy.getPositionX(), enemy.getPositionY(), 1.0f, 1.0f)) {
                // 충돌 시 처리
                enemy.takeDamage(bullet.getDamage());
                bullet.setInactive(); // 총알 비활성화
            }
        }
    }

    // 플레이어와 장애물, 적과의 충돌 등도 추가적으로 처리
}*/
/*void GameThread::sendUpdatedStateToClients() {
    // 클라이언트들에게 게임 객체의 최신 상태 전송
    for (const auto& player : players) {
        PlayerStatusPacket packet = { player.getId(), player.getHealth(), player.getPositionX(), player.getPositionY() };
        send(serverSocket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
    }
}*/
void GameThread::waitUntilNextFrame(const std::chrono::time_point<std::chrono::steady_clock>& frameStartTime) {
    auto frameEndTime = std::chrono::steady_clock::now();
    auto frameDuration = std::chrono::milliseconds(33); // 30fps 유지 (1000ms / 30)
    auto timeToWait = frameDuration - (frameEndTime - frameStartTime);

    if (timeToWait > std::chrono::milliseconds(0)) {
        std::this_thread::sleep_for(timeToWait);
    }
}
