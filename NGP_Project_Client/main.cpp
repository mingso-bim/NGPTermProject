#include "Common.h"
#include "Packet.h"

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000

class Client
{
public:
	char name[20];
	int ID;

	Client()
	{
		*name = NULL;
		ID = -1;
	}
};

// 변수
bool gameOver = false;

// 게임 데이터 전송
void sendGameData(SOCKET s);

// 게임 데이터 받기
void receiveGameData(SOCKET s);

// 게임 결과 받기
void receiveResult(SOCKET s);

int main()
{
	size_t retval{};

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket");

	// connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect");

	// 닉네임 입력 받기 (수정 필요)
	Client client;
	scanf("%s", client.name);

	// 닉네임 전송
	retval = send(sock, client.name, sizeof(client.name), 0);
	if (retval == SOCKET_ERROR) err_display("send - clientName");

	// ID 할당 받기
	retval = recv(sock, (char*)client.ID, sizeof(client.ID), 0);
	if (retval == SOCKET_ERROR) err_display("receive - clientID");

	while (true)
	{
		// 게임 시작 이벤트 감지 (작성 필요)

		// 게임 매칭 신호 전송
		unsigned short matchingStart = GAMESTART;
		retval = send(sock, (char*)matchingStart, sizeof(matchingStart), 0);
		if (retval == SOCKET_ERROR) err_display("send - matchingStart");

		// 게임 시작 신호 수신
		s_UIPacket gameStart;

		retval = recv(sock, (char*)&gameStart, sizeof(gameStart), 0);
		if (retval == SOCKET_ERROR) err_display("receive - s_UIPacket(gameStart)");
		if (gameStart.s_UIType != GAMESTART) err_display("receive - s_UIPacket(gameStart)");

		// initPacket 수신
		s_initPacket init;

		retval = send(sock, (char*)&init, sizeof(init), 0);
		if (retval == SOCKET_ERROR) err_display("receive - initPacket");

		while (!gameOver)
		{
			// 게임 데이터 전송
			sendGameData(sock);

			// 게임 데이터 받기
			receiveGameData(sock);
		}

		receiveResult(sock);
	}

	closesocket(sock);

	WSACleanup();
	return 0;

}

void sendGameData(SOCKET s)
{
	int retval;

	// c_playerPacket 전송
	c_playerPacket playerPacket;
	// 정보 수집 필요
	retval = send(s, (char*)&playerPacket, sizeof(playerPacket), 0);
	if (retval == SOCKET_ERROR) err_display("receive - c_playetPacket");

	// c_bulletPacket 전송
	c_bulletPacket bulletPacket;
	// 정보 수집 필요
	retval = send(s, (char*)&bulletPacket, sizeof(bulletPacket), 0);
	if (retval == SOCKET_ERROR) err_display("receive - c_bulletPacket");

	// c_inputPacket 전송
	c_inputPacket inputPacket;
	// 정보 수집 필요
	retval = send(s, (char*)&inputPacket, sizeof(inputPacket), 0);
	if (retval == SOCKET_ERROR) err_display("receive - c_inputPacket");
}

void receiveGameData(SOCKET s)
{
	int retval;

	s_enemyPacket enemyPacket;
	retval = recv(s, (char*)&enemyPacket, sizeof(enemyPacket), 0);
	if (retval == SOCKET_ERROR) err_display("receive - enemyPacket");

	s_itemPacket itemPacket;
	retval = recv(s, (char*)&itemPacket, sizeof(itemPacket), 0);
	if (retval == SOCKET_ERROR) err_display("receive - itemPacket");

	s_bulletPacket bulletPacket;
	retval = recv(s, (char*)&bulletPacket, sizeof(bulletPacket), 0);
	if (retval == SOCKET_ERROR) err_display("receive - bulletPacket");

	s_obstaclePacket obstaclePacket;
	retval = recv(s, (char*)&obstaclePacket, sizeof(obstaclePacket), 0);
	if (retval == SOCKET_ERROR) err_display("receive - obstaclePacket");

	s_playerPacket playerPacket;
	retval = recv(s, (char*)&playerPacket, sizeof(playerPacket), 0);
	if (retval == SOCKET_ERROR) err_display("receive - playerPacket");
}

void receiveResult(SOCKET s)
{
	int retval;

	s_UIPacket UIPacket;
	retval = recv(s, (char*)&UIPacket, sizeof(UIPacket), 0);
	if (retval == SOCKET_ERROR) err_display("receive - UIPacket");
}