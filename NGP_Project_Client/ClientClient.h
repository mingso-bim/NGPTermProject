#pragma once

#include <vector>
#include <winsock2.h>
#include "ClientPlayer.h"
#include "ClientBullet.h"
#include "Packet.h"

class ClientClient {
public:
    ClientClient(SOCKET serverSocket);
    void updateFromServer(); // �������� ���� ������Ʈ
    void renderGameObjects(); // ���� ��ü ������ (����)

private:
    SOCKET serverSocket;                    // ���� ����
    ClientPlayer player;                    // �÷��̾�
    std::vector<ClientBullet> bullets;      // �Ѿ� ����Ʈ

    void updatePlayerFromServer(const s_playerPacket& packet);
    void updateBulletsFromServer(const s_bulletPacket& packet);
};
