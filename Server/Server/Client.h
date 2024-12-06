#pragma once
extern int nextID; // ���� ���� nextID ����
class Client
{
public:
	char name[20];
	int ID;
	SOCKET clientSock; // Ŭ���̾�Ʈ ���� �߰�
	c_inputPacket inputPacket; // �Է� ��Ŷ�� �����ϴ� �ʵ� �߰�

	Client()
	{
		*name = NULL;
		ID = nextID++;	//�����ڿ��� ID �ο��ϰ� ����
	}

	bool operator==(const Client& other) const {
		return name == other.name;
	}
};