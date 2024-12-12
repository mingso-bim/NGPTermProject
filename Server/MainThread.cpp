#include <iostream>
#include <string>

#include "Player.h"
#include "Enemy.h"
#include "Item.h"
#include "Bullet.h"
#include "Obstacle.h"
#include "Common.h"
#include "Packet.h"
#include <random>
#include <vector>
#include <list>
#include"Client.h"
#include "GameThread.h"
#define SERVERPORT 9000
#define BUFSIZE    512

using namespace std;
extern std::vector<c_playerPacket> sharedPlayerPackets(3);
extern std::vector<c_bulletPacket> sharedBulletPackets;
list<c_inputPacket> sharedInputList;
list<Client> waitClientList; // 전역(gameThread 에서 이용하기 위해) 수정필요
 CRITICAL_SECTION cs;                // 전역으로 Critical Section 선언 //Client 클래스 헤더파일로 분리 
 int nextID = 1; // 전역 변수 nextID 정의

//list<Client> waitClientList;
HANDLE hGameStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
bool gameOver = false;
//CRITICAL_SECTION cs;



vector<c_bulletPacket> c_bullets = {};
c_bulletPacket recv_bullet;
vector<c_inputPacket> c_inputs = {};


// 게임 데이터 받기
void receiveGameData(SOCKET s);
// 게임 데이터 전송
void sendGameData(SOCKET s);
// 게임 결과 전송
void sendResult(SOCKET s, int result);
void initializePlayerPackets();


DWORD WINAPI networkThread(LPVOID arg)
{
	SOCKET clientSock = (SOCKET)arg;
	int retval;
	Client client;

	// 닉네임 받기
	retval = recv(clientSock, client.name, sizeof(client.name), 0);
	if (retval == SOCKET_ERROR) {
		err_display("receive - clientName");
	}
	else {
		cout << "recv - name success - " << client.name << endl;
	}

	// ID 할당
	retval = send(clientSock, (char*)&client.ID, sizeof(client.ID), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send - clientID");
	}
	else {
		cout << "send - ID success - " << client.ID << endl;
	}
	while (true)
	{
		// 매칭 신호 수신
		unsigned short start;
		retval = recv(clientSock, (char*)&start, sizeof(start), MSG_WAITALL);
		if (retval == SOCKET_ERROR) err_display("receive - c_playetPacket");
		if (start != GAMESTART) {
			err_display("receive - start");
		}
		else {
			cout << client.ID << " - Matching start" << endl;
		}

		// 리스트에 입장
		EnterCriticalSection(&cs);		// 크리티컬섹션으로 다른스레드와 동기화
		waitClientList.push_back(client);
		LeaveCriticalSection(&cs);
		cout << "enter List" << endl;
		cout << waitClientList.size()<< "개의 클라이언트 대기중" << endl;


		// 게임 잡힐 때 까지 대기
		WaitForSingleObject(hGameStartEvent, INFINITE);
		waitClientList.remove(client);

		cout << "매칭 완료" << endl;

		


		// initPacket 전송
		
		s_initPacket init;
		retval = send(clientSock, (char*)&init, sizeof(init), 0);
		if (retval == SOCKET_ERROR) { 
			err_display("send - initPacket"); 
		}
		else {
			cout << "send - initPacket success - " << client.ID << endl;
		}
		
		// 게임 루프
		initializePlayerPackets();
		while (!gameOver)
		{
			cout << "Game In " << endl;

			
			sendGameData(clientSock);
			// 1. 클라이언트로부터 데이터 수신
			receiveGameData(clientSock);
			// 2. 게임 데이터 전송
			

			// 프레임 고정
			Sleep(32);
		}
		// 게임 결과 전송
		sendResult(clientSock, 0);
		cout << "Game End " << endl;
	}
	closesocket(clientSock);
	nextID--;
}

void sendGameData(SOCKET s)
{
	if (s == INVALID_SOCKET) err_display("invalid socket");

	int retval, dataSize;

	//// s_enemyPacket 전송
	//dataSize = enemies.size() * sizeof(s_enemyPacket);
	//retval = send(s, (char*)&dataSize, sizeof(int), 0);
	//if (retval == SOCKET_ERROR) { err_display("send - enemyPacketSize"); }
	//retval = send(s, (char*)enemies.data(), dataSize, 0);
	//if (retval == SOCKET_ERROR) { err_display("send - enemyPacket"); }

	//// s_itemPacket 전송
	//dataSize = items.size() * sizeof(s_itemPacket);
	//retval = send(s, (char*)&dataSize, sizeof(int), 0);
	//if (retval == SOCKET_ERROR) { err_display("send - itemPacketSize"); }
	//retval = send(s, (char*)items.data(), dataSize, 0);
	//if (retval == SOCKET_ERROR) { err_display("send - itemPacket"); }

	//// s_obstaclePacket 전송
	//dataSize = obstacles.size() * sizeof(s_obstaclePacket);
	//retval = send(s, (char*)&dataSize, sizeof(int), 0);
	//if (retval == SOCKET_ERROR) { err_display("send - obstaclePacketSize"); }
	//retval = send(s, (char*)obstacles.data(), dataSize, 0);
	//if (retval == SOCKET_ERROR) { err_display("send - obstaclePacket"); }

	//// s_bulletPacket 전송
	//dataSize = bullets.size() * sizeof(s_bulletPacket);
	//retval = send(s, (char*)&dataSize, sizeof(int), 0);
	//if (retval == SOCKET_ERROR) { err_display("send - bulletPacketSize"); }
	//retval = send(s, (char*)bullets.data(), dataSize, 0);
	//if (retval == SOCKET_ERROR) { err_display("send - bulletPacket"); }

	EnterCriticalSection(&cs); // 동기화

	const int fixedBulletCount = 5; // 총알 데이터 개수를 고정
	std::vector<c_bulletPacket> fixedBulletPackets(fixedBulletCount);

	// 기존 데이터를 복사
	for (int i = 0; i < sharedBulletPackets.size() && i < fixedBulletCount; ++i) {
		fixedBulletPackets[i] = sharedBulletPackets[i];
	}

	// 고정된 크기로 전송
	 dataSize = fixedBulletCount * sizeof(c_bulletPacket);
	 retval = send(s, (char*)fixedBulletPackets.data(), dataSize, 0);
	if (retval == SOCKET_ERROR) {
		err_display("send - bulletPacket");
		LeaveCriticalSection(&cs);
		return;
	}

	std::cout << "[LOG(Server)] Sent Bullet Packet X: " << fixedBulletPackets[0].c_playerX 
		<< " y : " << fixedBulletPackets[0].c_playerY<< std::endl;

	sharedBulletPackets.clear(); // 기존 벡터 초기화
	LeaveCriticalSection(&cs); // 동기화 해제

	
	// s_playerPacket 전송
	//dataSize = sizeof(PlayerStatusPacket) * 3;
	//retval = send(s, (char*)sendPlayers.data(), dataSize, 0);
	//if (retval == SOCKET_ERROR) { err_display("send - playerPacket"); }
	//cout << "send game data" << endl;
	EnterCriticalSection(&cs); // 동기화

	// 플레이어 데이터 전송
	for (const auto& player : sharedPlayerPackets) {
		int retval = send(s, (char*)&player, sizeof(c_playerPacket), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send - playerPacket");
			LeaveCriticalSection(&cs);
			return;
		}

		std::cout << "[LOG(Server)] Sent Player Packet: ID=" << player.c_playerID
			<< ", PosX=" << player.c_playerPosX
			<< ", PosY=" << player.c_playerPosY << std::endl;
	}

	LeaveCriticalSection(&cs); // 동기화 해제
}

void receiveGameData(SOCKET s)
{
	if (s == INVALID_SOCKET) err_display("invalid socket");

	int retval, dataSize{}, vSize;

	// c_playerPacket 받기
	c_playerPacket c_player = {};

	retval = recv(s, (char*)&c_player, sizeof(c_playerPacket), MSG_WAITALL);
	if (retval == SOCKET_ERROR) err_display("receive - c_playetPacket");

	EnterCriticalSection(&cs); // 동기화

	// ID에 맞는 데이터 업데이트
	if (c_player.c_playerID >= 1 && c_player.c_playerID <= 3) {
		sharedPlayerPackets[c_player.c_playerID - 1] = c_player; // ID에 맞게 갱신
		std::cout << "[LOG(Server)] Updated Player Packet: ID=" << c_player.c_playerID
			<< ", PosX=" << c_player.c_playerPosX
			<< ", PosY=" << c_player.c_playerPosY << std::endl;
	}
	else {
		std::cout << "[LOG(Server)] Invalid Player Packet ID: " << c_player.c_playerID << std::endl;
	}

	LeaveCriticalSection(&cs); // 동기화 해제
	std::cout << "[LOG] Player Packet Received: Name=" << c_player.c_playerName
		<< ", ID=" << c_player.c_playerID
		<< ", PosX=" << c_player.c_playerPosX
		<< ", PosY=" << c_player.c_playerPosY << std::endl;


	c_bulletPacket recv_bullet = {};
	// c_bulletPacket 받기
	retval = recv(s, (char*)&recv_bullet, sizeof(c_bulletPacket), MSG_WAITALL);
	if (retval == SOCKET_ERROR) err_display("receive - c_bulletPacket");

	EnterCriticalSection(&cs); // 동기화
	sharedBulletPackets.push_back(recv_bullet); // 새로운 bullet 추가
	LeaveCriticalSection(&cs); // 동기화 해제

	std::cout << "[LOG] Bullet Packet Received: PlayerX=" << recv_bullet.c_playerX
		<< ", PlayerY=" << recv_bullet.c_playerY
		<< ", PosX=" << recv_bullet.c_targetX
		<< ", PosY=" << recv_bullet.c_targetY << std::endl;

}

void sendResult(SOCKET s, int result)
{
	int retval;

	s_UIPacket UIPacket(result);
	retval = send(s, (char*)&UIPacket, sizeof(UIPacket), 0);
	if (retval == SOCKET_ERROR) err_display("send - UIPacket");
}


DWORD WINAPI gameThread(LPVOID arg) {
	std::cout << "GameThread 생성" << std::endl;

	// 매칭 로직
	bool Matching = true;
	while (Matching) {
		EnterCriticalSection(&cs);
		if (waitClientList.size() == 1) {
			std::cout << "GameThread - Matching finish" << std::endl;
			Matching = false;
		}
		LeaveCriticalSection(&cs);
	}

	// 매칭 완료 후 게임 시작 이벤트 설정
	SetEvent(hGameStartEvent);
	std::cout << "gameThread start" << std::endl;

	// GameThread 객체 생성 및 게임 실행
	GameThread game;  // GameThread 객체 생성
	std::cout << "gameThread run in" << std::endl;
	game.run();       // 게임 실행

	return 0;
}

int main(int argc, char* argv[])
{
	InitializeCriticalSection(&cs);
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	printf("[알림] 윈속 초기화 성공\n");

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit("socket()");

	// bind
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("bind()");

	// listen
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit("listen()");

	// 데이터 통신 변수
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	HANDLE hThread_g, hThread;
	// 게임스레드 생성
	CreateThread(NULL, 0, gameThread, NULL, 0, NULL); //4q

	while (1) {

		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			addr, ntohs(clientaddr.sin_port));

		// 네트워크스레드 생성
		hThread = CreateThread(NULL, 0, networkThread, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL) { closesocket(client_sock); }
		else { CloseHandle(hThread); }

	}

	// 소켓 닫기
	closesocket(listen_sock);
	DeleteCriticalSection(&cs);

	// 윈속 종료
	WSACleanup();
	return 0;
}
void initializePlayerPackets() {
	for (int i = 0; i < 3; ++i) {
		sharedPlayerPackets[i].c_playerID = i + 1; // ID: 1, 2, 3
		sharedPlayerPackets[i].c_playerPosX = 0.0f;
		sharedPlayerPackets[i].c_playerPosY = 0.0f;

		std::cout << "[LOG(Server)] Initialized Player Packet: ID=" << sharedPlayerPackets[i].c_playerID
			<< ", PosX=" << sharedPlayerPackets[i].c_playerPosX
			<< ", PosY=" << sharedPlayerPackets[i].c_playerPosY << std::endl;
	}
}