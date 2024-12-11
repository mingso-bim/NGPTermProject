#pragma once

#include "Player.h"
#include <string>
#include "Packet.h"

class PlayerServer : public Player {
public:
    PlayerServer(int id, const std::string& name, float x, float y, float speed, int maxHealth);

    // �Է� ó��
    void ProcessInput(const c_inputPacket& input);

    // ���� ��Ŷ ����
    s_playerPacket GenerateStatePacket() const;

private:
    int id;                // �÷��̾� ID
    std::string name;      // �÷��̾� �̸�
};
