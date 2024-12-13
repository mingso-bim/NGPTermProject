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
list<Client> waitClientList; // ����(gameThread ���� �̿��ϱ� ����) �����ʿ�
 CRITICAL_SECTION cs;                // �������� Critical Section ���� //Client Ŭ���� ������Ϸ� �и� 
 int nextID = 1; // ���� ���� nextID ����

//list<Client> waitClientList;
HANDLE hGameStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
bool gameOver = false;
//CRITICAL_SECTION cs;



vector<c_bulletPacket> c_bullets = {};
c_bulletPacket recv_bullet;
vector<c_inputPacket> c_inputs = {};


// ���� ������ �ޱ�
void receiveGameData(SOCKET s);
// ���� ������ ����
void sendGameData(SOCKET s);
// ���� ��� ����
void sendResult(SOCKET s, int result);
void initializePlayerPackets();


DWORD WINAPI networkThread(LPVOID arg)
{
	SOCKET clientSock = (SOCKET)arg;
	int retval;
	Client client;

	// �г��� �ޱ�
	retval = recv(clientSock, client.name, sizeof(client.name), 0);
	if (retval == SOCKET_ERROR) {
		err_display("receive - clientName");
	}
	else {
		cout << "recv - name success - " << client.name << endl;
	}

	// ID �Ҵ�
	retval = send(clientSock, (char*)&client.ID, sizeof(client.ID), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send - clientID");
	}
	else {
		cout << "send - ID success - " << client.ID << endl;
	}
	while (true)
	{
		// ��Ī ��ȣ ����
		unsigned short start;
		retval = recv(clientSock, (char*)&start, sizeof(start), MSG_WAITALL);
		if (retval == SOCKET_ERROR) err_display("receive - c_playetPacket");
		if (start != GAMESTART) {
			err_display("receive - start");
		}
		else {
			cout << client.ID << " - Matching start" << endl;
		}

		// ����Ʈ�� ����
		EnterCriticalSection(&cs);		// ũ��Ƽ�ü������� �ٸ�������� ����ȭ
		waitClientList.push_back(client);
		LeaveCriticalSection(&cs);
		cout << "enter List" << endl;
		cout << waitClientList.size()<< "���� Ŭ���̾�Ʈ �����" << endl;


		// ���� ���� �� ���� ���
		WaitForSingleObject(hGameStartEvent, INFINITE);
		waitClientList.remove(client);

		cout << "��Ī �Ϸ�" << endl;

		

		EnterCriticalSection(&cs); // ����ȭ
		initializePlayerPackets();
		// �÷��̾� ������ ����
		for (const auto& player : sharedPlayerPackets) {
			int retval = send(clientSock, (char*)&player, sizeof(c_playerPacket), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send - playerPacket");
				LeaveCriticalSection(&cs);
				//return;
			}

			std::cout << "[LOG(Server)] Sent Player Packet: ID=" << player.c_playerID
				<< ", PosX=" << player.c_playerPosX
				<< ", PosY=" << player.c_playerPosY << std::endl;
		}

		LeaveCriticalSection(&cs); // ����ȭ ����
		
		// ���� ����
		
		while (!gameOver)
		{
			cout << "Game In " << endl;

			
			sendGameData(clientSock);
			// 1. Ŭ���̾�Ʈ�κ��� ������ ����
			receiveGameData(clientSock);
			// 2. ���� ������ ����
			

			// ������ ����
			Sleep(32);
		}
		// ���� ��� ����
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

	//// s_enemyPacket ����
	//dataSize = enemies.size() * sizeof(s_enemyPacket);
	//retval = send(s, (char*)&dataSize, sizeof(int), 0);
	//if (retval == SOCKET_ERROR) { err_display("send - enemyPacketSize"); }
	//retval = send(s, (char*)enemies.data(), dataSize, 0);
	//if (retval == SOCKET_ERROR) { err_display("send - enemyPacket"); }

	//// s_itemPacket ����
	//dataSize = items.size() * sizeof(s_itemPacket);
	//retval = send(s, (char*)&dataSize, sizeof(int), 0);
	//if (retval == SOCKET_ERROR) { err_display("send - itemPacketSize"); }
	//retval = send(s, (char*)items.data(), dataSize, 0);
	//if (retval == SOCKET_ERROR) { err_display("send - itemPacket"); }

	//// s_obstaclePacket ����
	//dataSize = obstacles.size() * sizeof(s_obstaclePacket);
	//retval = send(s, (char*)&dataSize, sizeof(int), 0);
	//if (retval == SOCKET_ERROR) { err_display("send - obstaclePacketSize"); }
	//retval = send(s, (char*)obstacles.data(), dataSize, 0);
	//if (retval == SOCKET_ERROR) { err_display("send - obstaclePacket"); }

	//// s_bulletPacket ����
	//dataSize = bullets.size() * sizeof(s_bulletPacket);
	//retval = send(s, (char*)&dataSize, sizeof(int), 0);
	//if (retval == SOCKET_ERROR) { err_display("send - bulletPacketSize"); }
	//retval = send(s, (char*)bullets.data(), dataSize, 0);
	//if (retval == SOCKET_ERROR) { err_display("send - bulletPacket"); }

	EnterCriticalSection(&cs); // ����ȭ

	const int fixedBulletCount = 5; // �Ѿ� ������ ������ ����
	std::vector<c_bulletPacket> fixedBulletPackets(fixedBulletCount);

	// ���� �����͸� ����
	for (int i = 0; i < sharedBulletPackets.size() && i < fixedBulletCount; ++i) {
		fixedBulletPackets[i] = sharedBulletPackets[i];
	}

	// ������ ũ��� ����
	 dataSize = fixedBulletCount * sizeof(c_bulletPacket);
	 retval = send(s, (char*)fixedBulletPackets.data(), dataSize, 0);
	if (retval == SOCKET_ERROR) {
		err_display("send - bulletPacket");
		LeaveCriticalSection(&cs);
		return;
	}

	std::cout << "[LOG(Server)] Sent Bullet Packet X: " << fixedBulletPackets[0].c_playerX 
		<< " y : " << fixedBulletPackets[0].c_playerY<< std::endl;

	sharedBulletPackets.clear(); // ���� ���� �ʱ�ȭ
	LeaveCriticalSection(&cs); // ����ȭ ����

	
	// s_playerPacket ����
	//dataSize = sizeof(PlayerStatusPacket) * 3;
	//retval = send(s, (char*)sendPlayers.data(), dataSize, 0);
	//if (retval == SOCKET_ERROR) { err_display("send - playerPacket"); }
	//cout << "send game data" << endl;
	EnterCriticalSection(&cs); // ����ȭ

	// �÷��̾� ������ ����
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

	LeaveCriticalSection(&cs); // ����ȭ ����
}

void receiveGameData(SOCKET s)
{
	if (s == INVALID_SOCKET) err_display("invalid socket");

	int retval, dataSize{}, vSize;

	// c_playerPacket �ޱ�
	c_playerPacket c_player = {};

	retval = recv(s, (char*)&c_player, sizeof(c_playerPacket), MSG_WAITALL);
	if (retval == SOCKET_ERROR) err_display("receive - c_playetPacket");


	EnterCriticalSection(&cs); // ����ȭ

	// ID�� �´� ������ ������Ʈ
	if (c_player.c_playerID >= 1 && c_player.c_playerID <= 3) {
		sharedPlayerPackets[c_player.c_playerID - 1] = c_player; // ID�� �°� ����
		std::cout << "[LOG(Server)] Updated Player Packet: ID=" << c_player.c_playerID
			<< ", PosX=" << c_player.c_playerPosX
			<< ", PosY=" << c_player.c_playerPosY << std::endl;
	}
	else {
		std::cout << "[LOG(Server)] Invalid Player Packet ID: " << c_player.c_playerID << std::endl;
	}

	LeaveCriticalSection(&cs); // ����ȭ ����
	std::cout << "[LOG] Player Packet Received: Name=" << c_player.c_playerName
		<< ", ID=" << c_player.c_playerID
		<< ", PosX=" << c_player.c_playerPosX
		<< ", PosY=" << c_player.c_playerPosY << std::endl;


	c_bulletPacket recv_bullet = {};
	// c_bulletPacket �ޱ�
	retval = recv(s, (char*)&recv_bullet, sizeof(c_bulletPacket), MSG_WAITALL);
	if (retval == SOCKET_ERROR) err_display("receive - c_bulletPacket");

	EnterCriticalSection(&cs); // ����ȭ
	sharedBulletPackets.push_back(recv_bullet); // ���ο� bullet �߰�
	LeaveCriticalSection(&cs); // ����ȭ ����

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
	std::cout << "GameThread ����" << std::endl;

	// ��Ī ����
	bool Matching = true;
	while (Matching) {
		EnterCriticalSection(&cs);
		if (waitClientList.size() == 2) {
			std::cout << "GameThread - Matching finish" << std::endl;
			Matching = false;
		}
		LeaveCriticalSection(&cs);
	}

	// ��Ī �Ϸ� �� ���� ���� �̺�Ʈ ����
	SetEvent(hGameStartEvent);
	std::cout << "gameThread start" << std::endl;

	// GameThread ��ü ���� �� ���� ����
	GameThread game;  // GameThread ��ü ����
	std::cout << "gameThread run in" << std::endl;
	game.run();       // ���� ����

	return 0;
}

int main(int argc, char* argv[])
{
	InitializeCriticalSection(&cs);
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	printf("[�˸�] ���� �ʱ�ȭ ����\n");

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

	// ������ ��� ����
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	HANDLE hThread_g, hThread;
	// ���ӽ����� ����
	CreateThread(NULL, 0, gameThread, NULL, 0, NULL); //4q

	while (1) {

		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			addr, ntohs(clientaddr.sin_port));

		// ��Ʈ��ũ������ ����
		hThread = CreateThread(NULL, 0, networkThread, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL) { closesocket(client_sock); }
		else { CloseHandle(hThread); }

	}

	// ���� �ݱ�
	closesocket(listen_sock);
	DeleteCriticalSection(&cs);

	// ���� ����
	WSACleanup();
	return 0;
}
void initializePlayerPackets() {
	for (int i = 0; i < 3; ++i) {
		sharedPlayerPackets[i].c_playerID = i + 1; // ID: 1, 2, 3
		sharedPlayerPackets[i].c_playerPosX = 4800;
		sharedPlayerPackets[i].c_playerPosY = 4800;

		std::cout << "[LOG(Server)] Initialized Player Packet: ID=" << sharedPlayerPackets[i].c_playerID
			<< ", PosX=" << sharedPlayerPackets[i].c_playerPosX
			<< ", PosY=" << sharedPlayerPackets[i].c_playerPosY << std::endl;
	}
}