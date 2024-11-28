#include "Common.h"
#include "packet.h"
#include <random>
#include <vector>
#include <queue>
using namespace std;

class Client
{
public:
	char* name;
	int ID;

	Client()
	{
		name = NULL;
		ID = -1;
	}
};

queue<Client> waitClientList;
HANDLE hMatchingEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
HANDLE hGameStartEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
bool gameOver = false;

// ���� ID�ο�
int makeRandomID();
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
	retval = recv(clientSock, client.name, 20, 0);
	if (retval == SOCKET_ERROR) err_display("receive - clientName");

	// ID �Ҵ�
	retval = send(clientSock, (char*)makeRandomID(), sizeof(client.ID), 0);
	if (retval == SOCKET_ERROR) err_display("send - clientID");

	while (true)
	{
		// ��Ī ���� �� ���� ���
		WaitForSingleObject(hMatchingEvent, INFINITE);

		// ť�� ����
		waitClientList.push(client);

		// ���� ���� �� ���� ���
		WaitForSingleObject(hGameStartEvent, INFINITE);

		// ���� ���� ��ȣ ����
		s_UIPacket start(GAMESTART);

		retval = send(clientSock, (char*)&start, sizeof(start), 0);
		if (retval == SOCKET_ERROR) err_display("send - s_UIPacket(start)");

		// initPacket ����
		s_initPacket init;

		retval = send(clientSock, (char*)&init, sizeof(init), 0);
		if (retval == SOCKET_ERROR) err_display("send - initPacket");

		// ���� ����
		while (!gameOver)
		{
			// ���� ������ �ޱ�
			receiveGameData(clientSock);

			// ���� ������ ����
			sendGameData(clientSock);
		}
		// ���� ��� ����
		sendResult(clientSock, 0);
	}
}

int makeRandomID()
{
	return rand() / 1000;
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
	if (retval == SOCKET_ERROR) err_display("receive - c_inputPacket");
}

void sendGameData(SOCKET s)
{
	int retval;

	s_enemyPacket enemyPacket;
	retval = recv(s, (char*)&enemyPacket, sizeof(enemyPacket), 0);
	if (retval == SOCKET_ERROR) err_display("send - enemyPacket");

	s_itemPacket itemPacket;
	retval = recv(s, (char*)&itemPacket, sizeof(itemPacket), 0);
	if (retval == SOCKET_ERROR) err_display("send - itemPacket");

	s_bulletPacket bulletPacket;
	retval = recv(s, (char*)&bulletPacket, sizeof(bulletPacket), 0);
	if (retval == SOCKET_ERROR) err_display("send - bulletPacket");

	s_obstaclePacket obstaclePacket;
	retval = recv(s, (char*)&obstaclePacket, sizeof(obstaclePacket), 0);
	if (retval == SOCKET_ERROR) err_display("send - obstaclePacket");

	s_UIPacket UIPacket;
	retval = recv(s, (char*)&UIPacket, sizeof(UIPacket), 0);
	if (retval == SOCKET_ERROR) err_display("send - UIPacket");

	s_playerPacket playerPacket;
	retval = recv(s, (char*)&playerPacket, sizeof(playerPacket), 0);
	if (retval == SOCKET_ERROR) err_display("send - playerPacket");
}

void sendResult(SOCKET s, int result)
{
	int retval;

	s_UIPacket UIPacket(result);
	retval = recv(s, (char*)&UIPacket, sizeof(UIPacket), 0);
	if (retval == SOCKET_ERROR) err_display("send - UIPacket");
}