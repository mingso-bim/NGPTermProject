#pragma once

#include "Player.h"
#include "Packet.h" // ��Ŷ ���� ����
#include <winsock2.h>

class ClientPlayer : public Player {
public:
    ClientPlayer(SOCKET serverSocket, float x, float y, float speed, int maxHealth);

    // ������ �Է� ����
    void SendInputToServer(const std::string& input);

    // �����κ��� ���� ����
    void ReceiveStateFromServer();

private:
    SOCKET serverSocket; // ������ ����� ����
};
