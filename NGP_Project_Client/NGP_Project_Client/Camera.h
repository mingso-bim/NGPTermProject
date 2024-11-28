#pragma once

class Camera {
public:
    Camera(float width, float height);
    ~Camera();

    void Update(float playerX, float playerY);
    float GetOffsetX() const;
    float GetOffsetY() const;

    void SetBounds(float width, float height); // ��踦 �����ϴ� �޼��� �߰�

private:
    float width, height;
    float offsetX, offsetY;
    float boundWidth, boundHeight; // ��踦 ���� ����
};
