// Packet.h
#pragma once
#include <iostream>
#include <string>
using namespace std;

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 512

extern SOCKET sock;
extern char buf[BUFSIZE + 1];
extern size_t retval;

// UI 패킷용
#define GAMESTART 20040
#define VICTORY 20041
#define DEFEAT 20042

class Client    // 합치면서 수정 예정
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



#pragma pack(1)
struct PlayerStatusPacket {
	unsigned short playerId;
	unsigned short health;
	float posX;
	float posY;
};


struct s_initPacket
{
	float s_obstacleX;
	float s_obstacleY;
	float s_playerInitX;
	float s_playerInitY;
	unsigned short s_playerMaxHealth;
	unsigned short s_initialAmmoCount;
};

struct s_enemyPacket
{
	unsigned short s_enemyID;
	unsigned short s_enemyType;
	float s_enemyPosX;
	float s_enemyPosY;
	unsigned short s_enemyHealth;
};

struct s_itemPacket
{
	unsigned short s_itemID;
	float s_itemPosX;
	float s_itemPosY;
	unsigned short s_itemType;
};

struct s_bulletPacket
{
	unsigned short s_bulletID;
	float s_bulletPosX;
	float s_bulletPosY;
};

struct s_obstaclePacket
{
	float s_obstaclePosX;
	float s_obstaclePosY;
};

struct s_UIPacket
{
	unsigned short s_UIType;

	s_UIPacket() {}
	s_UIPacket(int _type)
	{
		s_UIType = _type;
	}
};

struct s_playerPacket
{
	char s_playerName[20];
	unsigned short s_playerID;
	float s_playerPosX;
	float s_playerPosY;
	float s_playerSpeed;
	unsigned short s_playerHealth;
	unsigned short s_playerLevel;
	int s_playerEXP;
	bool s_isPlayerDead;
};

struct c_playerPacket
{
	char c_playerName[20];
	unsigned short c_playerID;
	float c_playerPosX;
	float c_playerPosY;
};

struct c_bulletPacket
{
	float c_playerX;
	float c_playerY;
	float c_targetX;
	float c_targetY;
};

struct c_inputPacket
{
	char c_key[10];
	float c_mouseX;
	float c_mouseY;
	unsigned short c_playerID;
};
#pragma pack()