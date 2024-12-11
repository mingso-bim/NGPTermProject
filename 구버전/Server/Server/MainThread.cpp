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

list<c_inputPacket> sharedInputList;
list<Client> waitClientList; // ����(gameThread ���� �̿��ϱ� ����) �����ʿ�
 CRITICAL_SECTION cs;                // �������� Critical Section ���� //Client Ŭ���� ������Ϸ� �и� 
 int nextID = 1; // ���� ���� nextID ����

//list<Client> waitClientList;
HANDLE hGameStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
bool gameOver = false;
//CRITICAL_SECTION cs;


// ���� ������ �ޱ�
void receiveGameData(SOCKET s);
// ���� ������ ����
void sendGameData(SOCKET s);
// ���� ��� ����
void sendResult(SOCKET s, int result);



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

		// ���� ���� ��ȣ ����
		s_UIPacket gameStart(GAMESTART);
		retval = send(clientSock, (char*)&gameStart, sizeof(gameStart), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send - s_UIPacket(gameStart)");
		}
		else{
			cout << "Game start " << endl;
		}

		// initPacket ����
		s_initPacket init;
		retval = send(clientSock, (char*)&init, sizeof(init), 0);
		if (retval == SOCKET_ERROR) { 
			err_display("send - initPacket"); 
		}
		else {
			cout << "send - initPacket success - " << client.ID << endl;
		}
		// ���� ����
		
		while (!gameOver)
		{
			cout << "Game In " << endl;
			// Ŭ���̾�Ʈ�κ��� �Է� ����
			c_inputPacket inputPacket = {};
			int retval = recv(clientSock, (char*)&inputPacket, sizeof(inputPacket), 0);
			if (retval == SOCKET_ERROR) {
				std::cerr << "Failed to receive input from client. Error: " << WSAGetLastError() << std::endl;
				break;
			}
			else {
				std::cout << "Received input from client: moveLeft=" << inputPacket.moveLeft
					<< ", moveRight=" << inputPacket.moveRight
					<< ", moveUp=" << inputPacket.moveUp
					<< ", moveDown=" << inputPacket.moveDown << std::endl;
			}

			

			// ���� �Է��� ���� ����Ʈ�� �߰�
			EnterCriticalSection(&cs);
			sharedInputList.push_back(inputPacket);
			std::cout << "Input received: moveLeft=" << inputPacket.moveLeft
				<< ", moveRight=" << inputPacket.moveRight
				<< ", moveUp=" << inputPacket.moveUp
				<< ", moveDown=" << inputPacket.moveDown << std::endl;
			LeaveCriticalSection(&cs);

			// ���� ������ �ޱ�
			receiveGameData(clientSock);

			// ���� ������ ����
			sendGameData(clientSock);
		}
		// ���� ��� ����
		sendResult(clientSock, 0);
		
	}
	closesocket(clientSock);
	nextID--;
}

void receiveGameData(SOCKET s)
{
	int retval;

	// c_playerPacket �ޱ�
	c_playerPacket playerPacket;
	retval = recv(s, (char*)&playerPacket, sizeof(playerPacket), 0);
	if (retval == SOCKET_ERROR) err_display("receive - c_playetPacket");

	// c_bulletPacket �ޱ�
	c_bulletPacket bulletPacket;
	retval = recv(s, (char*)&bulletPacket, sizeof(bulletPacket), 0);
	if (retval == SOCKET_ERROR) err_display("receive - c_bulletPacket");

	// c_inputPacket �ޱ�
	c_inputPacket inputPacket;
	retval = recv(s, (char*)&inputPacket, sizeof(inputPacket), 0);
	if (retval == SOCKET_ERROR) {
		err_display("receive - c_inputPacket");
	}
	else {
		// ���� ������ �����ϰ� waitClientList�� ù ��° Ŭ���̾�Ʈ�� ��� �����ؾ��� �κ�
		EnterCriticalSection(&cs);
		if (!waitClientList.empty()) {
			waitClientList.front().inputPacket = inputPacket; // ù ��° Ŭ���̾�Ʈ�� �Է� ��Ŷ ����
		}
		LeaveCriticalSection(&cs);
	}
	
}

void sendGameData(SOCKET s)
{
	int retval;

	s_enemyPacket enemyPacket;
	retval = send(s, (char*)&enemyPacket, sizeof(enemyPacket), 0);
	if (retval == SOCKET_ERROR) {
		int errorCode = WSAGetLastError();
		std::cerr << "Send error: " << errorCode << std::endl;
	}
	else {
		std::cout << "Send success: " << retval << " bytes" << std::endl;
	}

	s_itemPacket itemPacket;
	retval = send(s, (char*)&itemPacket, sizeof(itemPacket), 0);
	if (retval == SOCKET_ERROR) err_display("send - itemPacket");

	s_bulletPacket bulletPacket;
	retval = send(s, (char*)&bulletPacket, sizeof(bulletPacket), 0);
	if (retval == SOCKET_ERROR) err_display("send - bulletPacket");

	s_obstaclePacket obstaclePacket;
	retval = send(s, (char*)&obstaclePacket, sizeof(obstaclePacket), 0);
	if (retval == SOCKET_ERROR) err_display("send - obstaclePacket");

	s_UIPacket UIPacket;
	retval = send(s, (char*)&UIPacket, sizeof(UIPacket), 0);
	if (retval == SOCKET_ERROR) err_display("send - UIPacket");

	s_playerPacket playerPacket;
	retval = send(s, (char*)&playerPacket, sizeof(playerPacket), 0);
	if (retval == SOCKET_ERROR) { // �̰͸� �ϴ� ����
		int errorCode = WSAGetLastError();
		std::cerr << "Send error: " << errorCode << std::endl;
	}
	else {
		std::cout << "Send success: " << retval << " bytes" << std::endl;
	}
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
		if (waitClientList.size() == 1) {
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
