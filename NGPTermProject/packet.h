#pragma once

// UI ÆÐÅ¶¿ë
#define GAMESTART 20040
#define VICTORY 20041
#define DEFEAT 20042

struct s_initPacket
{
	float s_obstacleX;
	float s_obstacleY;
	float s_playerInitX;
	float s_playerInitY;
	int s_playerMaxHealth;
	int s_initialAmmoCount;
};

struct s_enemyPacket
{
	int s_enemyID;
	int s_enemyType;
	float s_enemyPosX;
	float s_enemyPosY;
	int s_enemyHealth;
};

struct s_itemPacket
{
	int s_itemID;
	float s_itemPosX;
	float s_itemPosY;
	int s_itemType;
};

struct s_bulletPacket
{
	int s_bulletID;
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
	int s_UIType;

	s_UIPacket() {}
	s_UIPacket(int _type)
	{
		s_UIType = _type;
	}
};

struct s_playerPacket
{
	string s_playerName;
	int s_playerID;
	float s_playerPosX;
	float s_playerPosY;
	float s_playerSpeed;
	int s_playerHealth;
	int s_playerLevel;
};

struct c_playerPacket
{
	string c_playerName;
	int c_playerID;
	float c_playerPosX;
	float c_playerPosY;
};

struct c_bulletPacket
{ 
	float c_playerX;
	float c_playerY;
	int c_bulletID;
	float c_targetX;
	float c_targetY;
	int bulletDamage;
};

struct c_inputPacket
{
	string c_key;
	float c_mouseX;
	float c_mouseY;
	int c_playerID;
};