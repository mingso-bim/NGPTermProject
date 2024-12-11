
#include "GameFramework.h"
#include <random>
#include <mmsystem.h>
#include <string>

#include "Packet.h"
#include "ErrDisplay.h"

#pragma comment(lib, "winmm.lib")


extern GameFramework gameframework;
extern HFONT hFont;
std::vector<Enemy*> enemies;

Client client;

GameFramework::GameFramework()
    : m_hdcBackBuffer(nullptr),
    m_hBitmap(nullptr), m_hOldBitmap(nullptr),
    player(nullptr), camera(nullptr),
    showClickImage(false), clickImageTimer(0.0f),
    enemySpawnTimer(0.0f), bigBoomerSpawnTimer(0.0f), lampreySpawnTimer(0.0f),  yogSpawnTimer(0.0f),
    currentGun(&revolver),
    frameTime(0.0f), gameTimeSeconds(0),
    isPaused(false),
    isShowingUpgradePanel(false), 
    isMainMenu(true), menuAnimationFrame(0), menuAnimationAccumulator(0.0f), selectedMenuItem(0),
    isMainMenuMusicPlaying(false){// isBackgroundMusicPlaying(false) {
    Clear();

   

    mapImage.Load(L"./resources/background/background.png");
    pauseUIImage.Load(L"./resources/ui/T_PauseMenu.png");

    int mapWidth = mapImage.GetWidth();
    int mapHeight = mapImage.GetHeight();

    player = new Player(mapWidth / 2.0f, mapHeight / 2.0f, 2.0f, 0.2f, this); // gameFramework ������ ����
    players.push_back(player); // players ���Ϳ� player �߰�
    // xPos, yPos, speed, animationSpeed, gameframeworkPtr
    player->SetBounds(mapWidth, mapHeight);

    camera = new Camera(800, 600);
    camera->SetBounds(mapWidth, mapHeight);

    cursorImage.Load(L"./resources/ui/icon_TakeAim.png");
    clickImage.Load(L"./resources/ui/T_CursorSprite.png");

    StartCreateEnemies();
    srand(static_cast<unsigned int>(time(NULL)));
    StartCreateObstacles();

    bulletUI.Load(L"./resources/ui/bullet_ui.png");
    bulletUsedUI.Load(L"./resources/ui/bullet_used_ui.png");

    menuImages[0].Load(L"./resources/background/Title_0.png");
    menuImages[1].Load(L"./resources/background/Title_1.png");
    menuImages[2].Load(L"./resources/background/Title_2.png");
    
}

HFONT hFont = nullptr;

void InitializeFont() {

    AddFontResourceEx(L"./resources/font/ChevyRay - Lantern.ttf", FR_PRIVATE, nullptr);

    hFont = CreateFont(
        -30,                      // Height of the font
        0,                        // Width of the font
        0,                        // Escapement angle
        0,                        // Orientation angle
        FW_NORMAL,                // Font weight
        FALSE,                    // Italic attribute
        FALSE,                    // Underline attribute
        FALSE,                    // Strikeout attribute
        ANSI_CHARSET,             // Character set identifier
        OUT_TT_PRECIS,            // Output precision
        CLIP_DEFAULT_PRECIS,      // Clipping precision
        ANTIALIASED_QUALITY,      // Output quality
        DEFAULT_PITCH | FF_DONTCARE,  // Pitch and family
        L"ChevyRay - Lantern"     // Font name
    );
}

GameFramework::~GameFramework() {
    CleanupDoubleBuffering();

    if (hFont) { // ��Ʈ Release
        DeleteObject(hFont);
        hFont = nullptr;
    }

    delete camera;
    delete player;

    for (Obstacle* obstacle : obstacles) {
        delete obstacle;
    }
    obstacles.clear();

    for (Enemy* enemy : enemies) {
        delete enemy;
    }
    enemies.clear();

    for (Item* item : items) {
        delete item;
    }
    items.clear();
}

void GameFramework::ResetGame() {
    

    // �÷��̾� �����
    delete player;
    player = new Player(mapImage.GetWidth() / 2.0f, mapImage.GetHeight() / 2.0f, 2.0f, 0.2f, this);
    player->SetBounds(mapImage.GetWidth(), mapImage.GetHeight());

    // �� ����
    for (Enemy* enemy : enemies) {
        delete enemy;
    }
    enemies.clear();

    // ������ ����
    for (Item* item : items) {
        delete item;
    }
    items.clear();

    // ��ֹ� ����
    for (Obstacle* obstacle : obstacles) {
        delete obstacle;
    }
    obstacles.clear();

    // �ð� �ʱ�ȭ
    gameTimeSeconds = 0;

    // ��Ÿ �ʿ��� �ʱ�ȭ �۾�
    enemySpawnTimer = 0.0f;
    bigBoomerSpawnTimer = 0.0f;
    lampreySpawnTimer = 0.0f;
    yogSpawnTimer = 0.0f;

    // ī�޶� �ʱ�ȭ
    camera->SetBounds(mapImage.GetWidth(), mapImage.GetHeight());

    // �� �� ��ֹ� �����
    StartCreateEnemies();
    StartCreateObstacles();
    CreateObstacles(0);

    // initPacket ����
    s_initPacket init;
    retval = send(sock, (char*)&init, sizeof(init), 0);
    if (retval == SOCKET_ERROR) err_display("receive - initPacket");

}

void GameFramework::PlayGameSound(LPCWSTR soundFile) {
    PlaySound(soundFile, NULL, SND_FILENAME | SND_ASYNC);
}


void GameFramework::PlayBackgroundMusic() { //���� ����
    if (!isBackgroundMusicPlaying) {
        mciSendString(L"open ./resources/sounds/backgroundmusic.wav type waveaudio alias bgm", NULL, 0, NULL);
        mciSendString(L"play bgm repeat", NULL, 0, NULL);
        isBackgroundMusicPlaying = true;
    }
}

void GameFramework::StopBackgroundMusic() { //���� ����
    if (isBackgroundMusicPlaying) {
        mciSendString(L"stop bgm", NULL, 0, NULL);
        mciSendString(L"close bgm", NULL, 0, NULL);
        isBackgroundMusicPlaying = false;
    }
}


void GameFramework::PlayMainMenuMusic() {
    if (!isMainMenuMusicPlaying) {
        PlaySound(L"./resources/sounds/Pretty Dungeon LOOP.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        isMainMenuMusicPlaying = true;
    }
}

void GameFramework::StopMainMenuMusic() {
    if (isMainMenuMusicPlaying) {
        PlaySound(NULL, 0, 0); // ���� ��� ���� ���� ����
        isMainMenuMusicPlaying = false;
    }
}

void GameFramework::DrawMainMenu(HDC hdc) {
    if (!hFont) {
        InitializeFont();
    }
    RECT clientRect;
    GetClientRect(m_hWnd, &clientRect);

    menuAnimationAccumulator += frameTime;
    if (menuAnimationAccumulator >= 1.0f) {
        menuAnimationFrame = (menuAnimationFrame + 1) % 3; 
        menuAnimationAccumulator = 0.0f;
    }

    if (!menuImages[menuAnimationFrame].IsNull()) {
        menuImages[menuAnimationFrame].Draw(hdc, 0, 0, clientRect.right, clientRect.bottom);
    }

    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    SetBkMode(hdc, TRANSPARENT);
    RECT startRect = { 300, 300, 500, 350 };
    RECT quitRect = { 300, 360, 500, 410 };

    SetTextColor(hdc, selectedMenuItem == 0 ? RGB(255, 255, 0) : RGB(255, 255, 255));
    DrawText(hdc, L"START", -1, &startRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SetTextColor(hdc, selectedMenuItem == 1 ? RGB(255, 255, 0) : RGB(255, 255, 255));
    DrawText(hdc, L"QUIT", -1, &quitRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, hOldFont);

    PlayMainMenuMusic();

   
}

void GameFramework::HandleMenuInput(WPARAM wParam) {
    switch (wParam) {
        
    case VK_UP:
        selectedMenuItem = (selectedMenuItem - 1 + 2) % 2;
        break;
    case VK_DOWN:
        selectedMenuItem = (selectedMenuItem + 1) % 2;
        break;
    case VK_RETURN:
        if (selectedMenuItem == 0) {//selectedMenuItem == 0
           /* // ���� ��Ī ��ȣ ����
            unsigned short matchingStart = GAMESTART;
            retval = send(sock, (char*)&matchingStart, sizeof(matchingStart), 0);
            if (retval == SOCKET_ERROR) err_display("send - matchingStart");

            // ���� ���� ��ȣ ����
            bool recvStart = false;
            while (!recvStart) {
                s_UIPacket gameStart = {};
                retval = recv(sock, (char*)&gameStart, sizeof(gameStart), MSG_WAITALL);
                if (retval == SOCKET_ERROR) {
                    err_display("receive - s_UIPacket(gameStart)");
                }
                if (gameStart.s_UIType == GAMESTART) {
                    recvStart = true;

                    // ������ Ȯ�� ��ȣ ����
                    unsigned short confirmSignal = GAMESTART;
                    retval = send(sock, (char*)&confirmSignal, sizeof(confirmSignal), 0);
                    if (retval == SOCKET_ERROR) {
                        err_display("send - confirmSignal");
                    }
                }
            }*/
            ToggleMainMenu(); // This will also stop the music
            ResetGame();

        }
        else if (selectedMenuItem == 1) {
            PostMessage(m_hWnd, WM_CLOSE, 0, 0);
        }
        break;
    }
}


void GameFramework::ToggleMainMenu() {
    isMainMenu = !isMainMenu;
    if (isMainMenu) {
        ResetGame();
        //StopBackgroundMusic(); //������
        PlayMainMenuMusic(); 
    }
    else {
        StopMainMenuMusic(); 
        //PlayBackgroundMusic(); 
    }
}
void GameFramework::TogglePause() {
    isPaused = !isPaused;
}

void GameFramework::LevelUpUpgrade() {
    ShowUpgradePanel();
}

void GameFramework::ShowUpgradePanel() {
    // �̹����� �ѹ��� �ε��ϵ���
    if (!isUpgradePanelLoaded) {
        selectedPanelImage.Load(L"./resources/ui/T_SelectedPanel.png");
        unselectedPanelImage.Load(L"./resources/ui/T_UnSelectedPanel.png");
        isUpgradePanelLoaded = true;
    }

    isShowingUpgradePanel = true;

    // �������� ���׷��̵� �׸� ����
    std::vector<UpgradeOptions> allUpgrades = { MaxHp, MaxAmmo, AddSpeed, UpgradeGun };
    std::random_shuffle(allUpgrades.begin(), allUpgrades.end());
    upgradeOptions[0] = allUpgrades[0];
    upgradeOptions[1] = allUpgrades[1];

    selectedUpgradePanel = 0; // Start with the left panel selected
}

std::wstring GameFramework::GetUpgradeOptionText(UpgradeOptions option) {
    switch (option) {
    case MaxHp: return L"MaxHp +1";
    case MaxAmmo: return L"Max Ammo +1";
    case AddSpeed: return L"Add Speed";
    case UpgradeGun: return L"Upgrade Gun";
    default: return L"Unknown";
    }
}


void GameFramework::HideUpgradePanel() {
    isShowingUpgradePanel = false;
}

void GameFramework::StartCreateEnemies() {
    for (int i = 0; i < 10; ++i) {
        SpawnBrainMonster();
        SpawnEyeMonster();
    }
}

void GameFramework::SpawnBrainMonster() {
    float playerX = player->GetX();
    float playerY = player->GetY();
    float spawnRadius = 600.0f;


    float angle = (rand() % 360) * 3.14159265358979323846 / 180.0;
    float spawnX = playerX + spawnRadius * cos(angle);
    float spawnY = playerY + spawnRadius * sin(angle);

    enemies.push_back(new BrainMonster(spawnX, spawnY, 5.0f));
}

void GameFramework::SpawnEyeMonster() {
    float playerX = player->GetX();
    float playerY = player->GetY();
    float spawnRadius = 600.0f;

    float angle = (rand() % 360) * 3.14159265358979323846 / 180.0;
    float spawnX = playerX + spawnRadius * cos(angle);
    float spawnY = playerY + spawnRadius * sin(angle);

    enemies.push_back(new EyeMonster(spawnX, spawnY, 5.0f));
}

void GameFramework::SpawnBigBoomer() {
    float playerX = player->GetX();
    float playerY = player->GetY();
    float spawnRadius = 600.0f;

    float angle = (rand() % 360) * 3.14159265358979323846 / 180.0;
    float spawnX = playerX + spawnRadius * cos(angle);
    float spawnY = playerY + spawnRadius * sin(angle);

    enemies.push_back(new BigBoomer(spawnX, spawnY, 5.0f));
}

void GameFramework::SpawnLamprey() {
    float playerX = player->GetX();
    float playerY = player->GetY();
    float spawnRadius = 600.0f;

    for (int i = 0; i < 2; ++i) {
        float angle = (rand() % 360) * 3.14159265358979323846 / 180.0;
        float spawnX = playerX + spawnRadius * cos(angle);
        float spawnY = playerY + spawnRadius * sin(angle);

        enemies.push_back(new Lamprey(spawnX, spawnY, 5.0f));
    }
}

void GameFramework::SpawnYog() {
    float playerX = player->GetX();
    float playerY = player->GetY();
    float spawnRadius = 600.0f;

    float angle = (rand() % 360) * 3.14159265358979323846 / 180.0;
    float spawnX = playerX + spawnRadius * cos(angle);
    float spawnY = playerY + spawnRadius * sin(angle);

    enemies.push_back(new Yog(spawnX, spawnY, 5.0f));
}

void GameFramework::SpawnWingedMonster() {
    float playerX = player->GetX();
    float playerY = player->GetY();
    float spawnRadius = 600.0f;

    float angle = (rand() % 360) * 3.14159265358979323846 / 180.0;
    float spawnX = playerX + spawnRadius * cos(angle);
    float spawnY = playerY + spawnRadius * sin(angle);

    enemies.push_back(new WingedMonster(spawnX, spawnY, 5.0f));
}

void GameFramework::SpawnWingedMonsterNearPlayer() {
    PlaySound(L"./resources/sounds/BossSpawn.wav", NULL, SND_FILENAME | SND_ASYNC);
    float playerX = player->GetX();
    float playerY = player->GetY();
    float spawnRadius = 100.0f; 

    float angle = (rand() % 360) * 3.14159265358979323846 / 180.0;
    float spawnX = playerX + spawnRadius * cos(angle);
    float spawnY = playerY + spawnRadius * sin(angle);

    enemies.push_back(new WingedMonster(spawnX, spawnY, 5.0f));
}

void GameFramework::ClearEnemies() {
    for (Enemy* enemy : enemies) {
        delete enemy;
    }
    enemies.clear();
}

void GameFramework::HandleCheatKeys() {
    if (GetAsyncKeyState(VK_F9) & 0x8000) {
        if (!f9Pressed) {
            ClearEnemies();
            SpawnWingedMonsterNearPlayer();
            f9Pressed = true;
        }
    }
    else {
        f9Pressed = false;
    }
}

void GameFramework::DrawBulletUI(HDC hdc) {
    int x = 10;
    int y = 80;
    for (int i = 0; i < currentGun->maxAmmo; i++) {
        if (i < currentGun->currentAmmo) {
            bulletUI.Draw(m_hdcBackBuffer, x + i * 20, y);
        }
        else {
            bulletUsedUI.Draw(m_hdcBackBuffer, x + i * 20, y);
        }
    }
}

void GameFramework::DrawReloadingUI(HDC hdc) {
    if (currentGun->IsReloading()) {
        RECT rect; 
        rect.left = static_cast<LONG>(player->GetX() - camera->GetOffsetX() - 15);
        rect.top = static_cast<LONG>(player->GetY() - camera->GetOffsetY() - 20);
        rect.right = rect.left + 50;
        rect.bottom = rect.top + 5;

        // ��� ���
        HBRUSH whiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
        FillRect(m_hdcBackBuffer, &rect, whiteBrush);

        // ������ ���� ��
        int width = static_cast<int>((currentGun->reloadTimer / currentGun->reloadTime) * 50);
        rect.right = rect.left + width;
        HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
        FillRect(m_hdcBackBuffer, &rect, redBrush);
        DeleteObject(redBrush);  // �귯�� ����
    }
}

void GameFramework::SpawnItem(float x, float y) {
    items.push_back(new Item(x, y));
}

void GameFramework::Update(float frameTime, SOCKET s) {

    
    if (isMainMenu) {

        PlayMainMenuMusic();
        return;
    }
    else {
        PlayBackgroundMusic();
    }
   
    if (isPaused) return;

    if (isShowingUpgradePanel) {
        HandleUpgradeInput();
        return;
    }

    this->frameTime = frameTime;  // ������ Ÿ�� ����

    static float timeAccumulator = 0.0f;
    timeAccumulator += frameTime;
    if (timeAccumulator >= 1.0f) {
        gameTimeSeconds += static_cast<int>(timeAccumulator);
        timeAccumulator = 0.0f;
    }

    // f9ġƮŰ
    HandleCheatKeys();

   
    // �÷��̾� ������Ʈ
    player->Update(frameTime, obstacles);
    camera->Update(player->GetX(), player->GetY());

    // ������ �Է� ���� ����
    //player->sendInputToServer(s);

    // ü�� 0 ���� ����
    if (player->health <= 0) {
        //receiveResult(sock);

        ResetGame();
        return; 

    }

    // ���� ������ ����
    //sendGameData(sock);




    // �÷��̾�� �� �浹üũ
    for (Enemy* enemy : enemies) {
        if (!player->IsInvincible() &&
            abs(player->GetX() - enemy->GetX()) < (20.0f + enemy->GetWidth()) / 2 &&
            abs(player->GetY() - enemy->GetY()) < (25.0f + enemy->GetHeight()) / 2) {
            player->TakeDamage(1);
        }
    }
    
    // ������ ������Ʈ �� ����
    auto itemIter = items.begin();
    while (itemIter != items.end()) {
        Item* item = *itemIter;
        item->Update(frameTime);

        // �÷��̾�� �������� �浹 �˻� �� ����
        if (abs(player->GetX() - item->GetX()) < 20.0f && abs(player->GetY() - item->GetY()) < 25.0f) {
            player->AddExperience(10);
            delete item;
            itemIter = items.erase(itemIter);
        }
        else {
            ++itemIter;
        }
    }

    // �� ������Ʈ
    auto enemyIter = enemies.begin();
    while (enemyIter != enemies.end()) {
        Enemy* enemy = *enemyIter;
        //���� ���� 
        WingedMonster* wingedMonster = dynamic_cast<WingedMonster*>(enemy);
        if (wingedMonster) {
            wingedMonster->UpdateBoss(frameTime, player->GetX(), player->GetY(), obstacles);
        }
        else {
            enemy->Update(frameTime, player->GetX(), player->GetY(), obstacles);
        }

        if (enemy->IsDead()) {
            // ���� ���� �� ������ ���
            SpawnItem(enemy->GetX(), enemy->GetY());

            delete enemy;
            enemyIter = enemies.erase(enemyIter);
        }
        else {
            ++enemyIter;
        }
    }

    // �Ѿ� ������Ʈ
    auto bulletIter = bullets.begin();
    while (bulletIter != bullets.end()) {
        Bullet* bullet = *bulletIter;
        bullet->Update(frameTime);

        bool bulletRemoved = false;
        if (bullet->IsOutOfBounds(mapImage.GetWidth(), mapImage.GetHeight())) {
            bulletRemoved = true;
        }
        else if (bullet->isHit) {
            if (bullet->isEffectFinished()) {
                bulletRemoved = true;
            }
        }
        else {
            for (Enemy* enemy : enemies) {
                if (bullet->CheckCollision(enemy->GetX(), enemy->GetY(), enemy->GetWidth(), enemy->GetHeight())) {
                    enemy->TakeDamage(bullet->GetDamage());
                    bullet->isHit = true; // Set bullet hit
                    break;
                }
            }
        }

        if (bulletRemoved) {
            delete bullet;
            bulletIter = bullets.erase(bulletIter);
        }
        else {
            ++bulletIter;
        }
    }

    // �� ���� �ý���
    enemySpawnTimer += frameTime;
    if (enemySpawnTimer >= enemySpawnInterval) {
        for (int i = 0; i < 10; i++) {
            SpawnBrainMonster();
            SpawnEyeMonster();
        }
        enemySpawnTimer = 0.0f;
    }

    bigBoomerSpawnTimer += frameTime;
    if (bigBoomerSpawnTimer >= bigBoomerSpawnInterval) {
        SpawnBigBoomer();
        SpawnBigBoomer();
        SpawnBigBoomer();
        bigBoomerSpawnTimer = 0.0f;
    }

    lampreySpawnTimer += frameTime;
    if (lampreySpawnTimer >= lampreySpawnInterval) {
        SpawnLamprey();
        SpawnLamprey();
        lampreySpawnTimer = 0.0f;
    }

    yogSpawnTimer += frameTime;
    if (yogSpawnTimer >= yogSpawnInterval) {
        SpawnYog();
        yogSpawnTimer = 0.0f;
    }

    // Ŀ�� �̹��� ������Ʈ
    if (showClickImage) {
        clickImageTimer -= frameTime;
        if (clickImageTimer <= 0.0f) {
            showClickImage = false;
        }
    }

    // �� ���� ������Ʈ
    currentGun->UpdateReload(frameTime);

    // ���� ������ �ޱ�
    //receiveGameData(sock);

    // ����� Ű ������Ʈ
    HandleDebugKeys();
}

void GameFramework::HandleDebugKeys() {
    if (GetAsyncKeyState(VK_F1) & 0x8000) {
        if (!f1Pressed) {
            player->maxHealth += 1;
            player->health += 1;
            f1Pressed = true;
        }
    }
    else {
        f1Pressed = false;
    }

    if (GetAsyncKeyState(VK_F2) & 0x8000) {
        if (!f2Pressed) {
            revolver.maxAmmo += 1;
            headshotGun.maxAmmo += 1;
            clusterGun.maxAmmo += 1;
            dualShotgun.maxAmmo += 1;
            f2Pressed = true;
        }
    }
    else {
        f2Pressed = false;
    }

    if (GetAsyncKeyState(VK_F3) & 0x8000) {
        if (!f3Pressed) {
            player->speed += 1.0f;
            f3Pressed = true;
        }
    }
    else {
        f3Pressed = false;
    }

    if (GetAsyncKeyState(VK_F4) & 0x8000) {
        if (!f4Pressed) {
            // �� ���׷��̵�
            if (currentGun == &revolver) {
                currentGun = &headshotGun;
            }
            else if (currentGun == &headshotGun) {
                currentGun = &clusterGun;
            }
            else if (currentGun == &clusterGun) {
                currentGun = &dualShotgun;
            }
            f4Pressed = true;
        }
    }
    else {
        f4Pressed = false;
    }
}

void GameFramework::CreateObstacles(int count) {
    int mapWidth = mapImage.GetWidth();
    int mapHeight = mapImage.GetHeight();

    std::vector<std::wstring> imagePaths = {
        L"./resources/background/T_TempleTallColumn.png",
        L"./resources/background/Tree_0.png",
        L"./resources/background/Tree_1.png",
        L"./resources/background/Tree_2.png",
        L"./resources/background/Tree_3.png"
    };

    srand(static_cast<unsigned int>(time(NULL))); // Seed the random number generator

    for (int i = 0; i < count; ++i) {
        float x = static_cast<float>(rand() % mapWidth);
        float y = static_cast<float>(rand() % mapHeight);
        const std::wstring& imagePath = imagePaths[rand() % imagePaths.size()];
        obstacles.push_back(new Obstacle(x, y, imagePath));
    }
}

void GameFramework::StartCreateObstacles() {
    CreateObstacles(100);  // Create 10 obstacles
}


void GameFramework::FireBullet(float x, float y, float targetX, float targetY) {
    if (currentGun->FireBullet()) {

        PlayGameSound(L"./resources/sounds/single_shot.wav");

        if (dynamic_cast<Revolver*>(currentGun)) {
            bullets.push_back(new RevolverBullet(x, y, targetX, targetY));
            firedBullet = new RevolverBullet(x, y, targetX, targetY);
        }
        else if (dynamic_cast<HeadshotGun*>(currentGun)) {
            bullets.push_back(new HeadshotGunBullet(x, y, targetX, targetY));
            firedBullet = new HeadshotGunBullet(x, y, targetX, targetY);

        }
        else if (dynamic_cast<ClusterGun*>(currentGun)) {
            bullets.push_back(new ClusterGunBullet(x, y, targetX, targetY));
            bullets.push_back(new ClusterGunBullet(x, y, targetX, targetY + 10));
            firedBullet = new ClusterGunBullet(x, y, targetX, targetY);
            firedBullet = new ClusterGunBullet(x, y, targetX, targetY + 10);
        }
        else if (dynamic_cast<DualShotgun*>(currentGun)) {
            int numBullets = 5; // �߻��� �Ѿ��� ����
            float spreadAngle = 10.0f * (3.14159265358979323846 / 180.0f); // �������� ����(���� ������ ��ȯ)
            float baseAngle = atan2(targetY - y, targetX - x);

            for (int i = 0; i < numBullets; ++i) {
                float angle = baseAngle + spreadAngle * (i - numBullets / 2);
                float newTargetX = x + cos(angle) * 100;
                float newTargetY = y + sin(angle) * 100;
                bullets.push_back(new DualShotgunBullet(x, y, newTargetX, newTargetY, 0));
                firedBullet = new DualShotgunBullet(x, y, newTargetX, newTargetY, 0);
            }
        }
    }
}

void GameFramework::DrawGameTime(HDC hdc) {
    // Initialize the font if it hasn't been initialized
    if (!hFont) {
        InitializeFont();
    }

    // ���� ��Ʈ ����
    HFONT hOldFont = (HFONT)SelectObject(m_hdcBackBuffer, hFont);

    RECT rect;
    rect.left = 625;  // �߾� ���
    rect.top = 40;
    rect.right = rect.left + 200;
    rect.bottom = rect.top + 40;

    int minutes = gameTimeSeconds / 60;
    int seconds = gameTimeSeconds % 60;

    wchar_t gameTimeText[100];
    swprintf_s(gameTimeText, L"%02d:%02d", minutes, seconds);

    SetBkMode(m_hdcBackBuffer, TRANSPARENT);  // ��� �����ϰ� ����
    SetTextColor(m_hdcBackBuffer, RGB(255, 255, 255));  // ��� �۾�
    DrawText(m_hdcBackBuffer, gameTimeText, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(m_hdcBackBuffer, hOldFont);
}

void GameFramework::DrawPauseMenu(HDC hdc) {
    if (!hFont) {
        InitializeFont();
    }

    HFONT hOldFont = (HFONT)SelectObject(m_hdcBackBuffer, hFont);

    SetBkMode(m_hdcBackBuffer, TRANSPARENT);

    RECT resumeRect = { 300, 200, 500, 250 };
    RECT mainMenuRect = { 300, 260, 500, 310 };
    RECT quitRect = { 300, 320, 500, 370 };

    SetTextColor(m_hdcBackBuffer, selectedMenuIndex == 0 ? RGB(255, 255, 0) : RGB(255, 255, 255));
    DrawText(m_hdcBackBuffer, L"Resume", -1, &resumeRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SetTextColor(m_hdcBackBuffer, selectedMenuIndex == 1 ? RGB(255, 255, 0) : RGB(255, 255, 255));
    DrawText(m_hdcBackBuffer, L"Main Menu", -1, &mainMenuRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SetTextColor(m_hdcBackBuffer, selectedMenuIndex == 2 ? RGB(255, 255, 0) : RGB(255, 255, 255));
    DrawText(m_hdcBackBuffer, L"Quit", -1, &quitRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(m_hdcBackBuffer, hOldFont);
}

void GameFramework::DrawUpgradePanel(HDC hdc) {
    if (!isShowingUpgradePanel) return;

    int screenWidth = 800;
    int screenHeight = 600;
    int panelWidth = 300;
    int panelHeight = 300;
    int panelSpacing = 100;

    int leftPanelX = (screenWidth / 2) - panelWidth - (panelSpacing / 2);
    int rightPanelX = (screenWidth / 2) + (panelSpacing / 2);
    int panelY = (screenHeight / 2) - (panelHeight / 2);

    // Draw the left panel
    if (selectedUpgradePanel == 0) {
        selectedPanelImage.Draw(hdc, leftPanelX, panelY, panelWidth, panelHeight);
    }
    else {
        unselectedPanelImage.Draw(hdc, leftPanelX, panelY, panelWidth, panelHeight);
    }

    // Draw the right panel
    if (selectedUpgradePanel == 1) {
        selectedPanelImage.Draw(hdc, rightPanelX, panelY, panelWidth, panelHeight);
    }
    else {
        unselectedPanelImage.Draw(hdc, rightPanelX, panelY, panelWidth, panelHeight);
    }

    // Draw the upgrade options text
    if (!hFont) {
        InitializeFont();
    }

    HFONT hOldFont = (HFONT)SelectObject(m_hdcBackBuffer, hFont);

    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);

    RECT leftTextRect = { leftPanelX, panelY, leftPanelX + panelWidth, panelY + panelHeight };
    DrawText(hdc, GetUpgradeOptionText(upgradeOptions[0]).c_str(), -1, &leftTextRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    RECT rightTextRect = { rightPanelX, panelY, rightPanelX + panelWidth, panelY + panelHeight };
    DrawText(hdc, GetUpgradeOptionText(upgradeOptions[1]).c_str(), -1, &rightTextRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(m_hdcBackBuffer, hOldFont);
}

void GameFramework::Draw(HDC hdc) {
    if (!m_hdcBackBuffer) {
        InitializeDoubleBuffering(hdc);
    }

    RECT clientRect;
    GetClientRect(m_hWnd, &clientRect);

    FillRect(m_hdcBackBuffer, &clientRect, (HBRUSH)(COLOR_WINDOW + 1));

    float offsetX = camera->GetOffsetX();
    float offsetY = camera->GetOffsetY();

    if (isMainMenu) {
        DrawMainMenu(m_hdcBackBuffer);
        BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, m_hdcBackBuffer, 0, 0, SRCCOPY);
        return;
    }

    mapImage.Draw(m_hdcBackBuffer, -static_cast<int>(offsetX), -static_cast<int>(offsetY));

    for (Player* p : players)
    {
        p->Draw(m_hdcBackBuffer, offsetX, offsetY);
    }
    //player->Draw(m_hdcBackBuffer, offsetX, offsetY);
    //player->DrawBoundingBox(m_hdcBackBuffer, offsetX, offsetY);

    for (Enemy* enemy : enemies) {
        enemy->Draw(m_hdcBackBuffer, offsetX, offsetY);
    }


    for (Obstacle* obstacle : obstacles) {
        obstacle->Draw(m_hdcBackBuffer, offsetX, offsetY);
    }

    for (Bullet* bullet : bullets) {
        bullet->Draw(m_hdcBackBuffer, offsetX, offsetY);
    }

    for (Item* item : items) {
        item->Draw(m_hdcBackBuffer, offsetX, offsetY);
    }

    currentGun->Draw(m_hdcBackBuffer, player->GetX() - offsetX, player->GetY() - offsetY, cursorPos.x, cursorPos.y, player->IsDirectionLeft());

    // Draw UI
    player->DrawHealth(m_hdcBackBuffer, offsetX, offsetY);
    player->DrawExperienceBar(m_hdcBackBuffer, clientRect);
    player->DrawInvincibilityIndicator(m_hdcBackBuffer, offsetX, offsetY);

    int cursorWidth = cursorImage.GetWidth();
    int cursorHeight = cursorImage.GetHeight();
    int clickWidth = clickImage.GetWidth();
    int clickHeight = clickImage.GetHeight();

    DrawBulletUI(m_hdcBackBuffer);
    DrawReloadingUI(m_hdcBackBuffer);
    DrawGameTime(m_hdcBackBuffer);

    if (isShowingUpgradePanel) { // ���׷��̵� UI
        DrawUpgradePanel(m_hdcBackBuffer);
    }

    if (isPaused) { // ����ȭ�� UI
        int panelWidth = pauseUIImage.GetWidth();
        int panelHeight = pauseUIImage.GetHeight();
        int x = (clientRect.right - panelWidth) / 2;
        int y = (clientRect.bottom - panelHeight) / 2;
        pauseUIImage.Draw(m_hdcBackBuffer, x, y);
        DrawPauseMenu(m_hdcBackBuffer);
    }

    // Ŀ�� �̹��� Draw
    if (showClickImage) {
        clickImage.Draw(m_hdcBackBuffer, cursorPos.x - clickWidth / 2, cursorPos.y - clickHeight / 2);
    }
    else {
        cursorImage.Draw(m_hdcBackBuffer, cursorPos.x - cursorWidth / 2, cursorPos.y - cursorHeight / 2);
    }

    BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, m_hdcBackBuffer, 0, 0, SRCCOPY);
}

// ���׷��̵� â Ű �Է�
void GameFramework::HandleUpgradeInput() {
    if (!isShowingUpgradePanel) return;

    if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        selectedUpgradePanel = 0;
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        selectedUpgradePanel = 1;
    }
    if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
        // ���׷��̵� �׸� ����
        switch (upgradeOptions[selectedUpgradePanel]) {
        case MaxHp:
            player->maxHealth += 1;
            player->health += 1;
            break;
        case MaxAmmo:
            currentGun->maxAmmo += 1;
            break;
        case AddSpeed:
            player->speed += 0.3f;
            break;
        case UpgradeGun:
            if (currentGun == &revolver) {
                currentGun = &headshotGun;
            }
            else if (currentGun == &headshotGun) {
                currentGun = &clusterGun;
            }
            else if (currentGun == &clusterGun) {
                currentGun = &dualShotgun;
            }
            else {
                currentGun = &dualShotgun;
            }
            break;
        }
        HideUpgradePanel();
    }
}

// ����ȭ�� �޴� �ε���
void GameFramework::PauseMenuSelect() {
    switch (selectedMenuIndex) {
    case 0:
        TogglePause();
        break;
    case 1:
        isPaused = false;
        isMainMenu = true;
        ResetGame();
        break;
    case 2:
        PostMessage(m_hWnd, WM_CLOSE, 0, 0);
        break;
    }
}

// ����ȭ�� Ű �ٿ� �̺�Ʈ ó��
void GameFramework::PauseKeyDown(WPARAM wParam) {
    switch (wParam) {
    case VK_UP:
        selectedMenuIndex = (selectedMenuIndex - 1 + 3) % 3; // �޴� �׸� ���� ���� ����
        break;
    case VK_DOWN:
        selectedMenuIndex = (selectedMenuIndex + 1) % 3;
        break;
    case VK_RETURN:
        PauseMenuSelect();
        break;
    }
}

// Ű �� �̺�Ʈ ó�� (�ʿ�� ����)
void GameFramework::OnKeyUp(WPARAM wParam) {
    // �ʿ��� ��� Ű �� �̺�Ʈ ó��
}

void GameFramework::OnKeyBoardProcessing(UINT iMessage, WPARAM wParam, LPARAM lParam, SOCKET s) {
    if (isPaused) {
        switch (iMessage) {
        case WM_KEYDOWN:
            PauseKeyDown(wParam);
            break;
        case WM_KEYUP:
            OnKeyUp(wParam);
            break;
        }
    }
    else if (isMainMenu) {
        switch (iMessage) {
        case WM_KEYDOWN:
            HandleMenuInput(wParam);
            break;
        case WM_KEYUP:
            OnKeyUp(wParam);
            break;
        }
    }
    else {
        switch (iMessage) {
        case WM_KEYDOWN:
            if (wParam == 'Q') {
                SendMessage(m_hWnd, WM_DESTROY, 0, 0);
                return;
            }
            switch (wParam) {
            case 'A':
            case 'a':
                player->moveLeft = true;
                break;
            case 'D':
            case 'd':
                player->moveRight = true;
                break;
            case 'W':
            case 'w':
                player->moveUp = true;
                break;
            case 'S':
            case 's':
                player->moveDown = true;
                break;
             //player->sendInputToServer(s);  // Ű�� ������ �� �Է� ���¸� ������ ����
              
            case '1':
                currentGun = &revolver;
                break;
            case '2':
                currentGun = &headshotGun;
                break;
            case '3':
                currentGun = &clusterGun;
                break;
            case '4':
                currentGun = &dualShotgun;
                break;
            }
            break;
           //player->sendInputToServer();
            case WM_KEYUP:
            switch (wParam) {
            case 'A':
            case 'a':
                player->moveLeft = false;
                break;
            case 'D':
            case 'd':
                player->moveRight = false;
                break;
            case 'W':
            case 'w':
                player->moveUp = false;
                break;
            case 'S':
            case 's':
                player->moveDown = false;
                break;
            }
           // player->sendInputToServer(s);   // Ű�� ������ �� �Է� ���¸� ������ ����
            
            break;
        }
    }
}

void GameFramework::OnMouseProcessing(UINT iMessage, WPARAM wParam, LPARAM lParam) {
    switch (iMessage) {
    case WM_MOUSEMOVE: {
        cursorPos.x = LOWORD(lParam);
        cursorPos.y = HIWORD(lParam);

        float playerScreenX = player->GetX() - camera->GetOffsetX();
        if (cursorPos.x < playerScreenX) {
            player->SetDirectionLeft(true);
        }
        else {
            player->SetDirectionLeft(false);
        }
        break;
    }
    case WM_LBUTTONDOWN: {
        showClickImage = true;
        clickImageTimer = 0.2f;
        cursorPos.x = LOWORD(lParam);
        cursorPos.y = HIWORD(lParam);

        if (isPaused || isShowingUpgradePanel || isMainMenu) {
            return;
        }
        else {
            FireBullet(player->GetX(), player->GetY(), cursorPos.x + camera->GetOffsetX(), cursorPos.y + camera->GetOffsetY());
        }
        break;
    }
    }
}

void GameFramework::InitializeDoubleBuffering(HDC hdc) {
    RECT clientRect;
    GetClientRect(m_hWnd, &clientRect);

    m_hdcBackBuffer = CreateCompatibleDC(hdc);
    m_hBitmap = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
    m_hOldBitmap = (HBITMAP)SelectObject(m_hdcBackBuffer, m_hBitmap);
}

void GameFramework::CleanupDoubleBuffering() {
    if (m_hdcBackBuffer) {
        SelectObject(m_hdcBackBuffer, m_hOldBitmap);
        DeleteObject(m_hBitmap);
        DeleteDC(m_hdcBackBuffer);
        m_hdcBackBuffer = nullptr;
    }
}

void GameFramework::Clear() {
    // �ʿ��� �ڿ� ���� ���� �߰�
    closesocket(sock);
    WSACleanup();

}

void GameFramework::Create(HWND hWnd) {
    m_hWnd = hWnd;
}

void GameFramework::sendGameData(SOCKET s)
{
    int retval, dataSize{};

    // player_packet ����
    c_playerPacket c_player = {};
    strcpy_s(c_player.c_playerName, player->name);
    c_player.c_playerID = player->ID;
    c_player.c_playerPosX = player->GetX();
    c_player.c_playerPosY = player->GetY();

    // c_playerPacket ����
    retval = send(s, (char*)&c_player, sizeof(c_playerPacket), 0);
    if (retval == SOCKET_ERROR) err_display("send - c_playetPacket");

    // bullet_packet ����
    c_bulletPacket c_bullet = {};

    if (firedBullet != nullptr) 
    {
        c_bullet.c_playerX = firedBullet->x;
        c_bullet.c_playerY = firedBullet->y;
        c_bullet.c_targetX = firedBullet->directionX;
        c_bullet.c_targetY = firedBullet->directionY;
    }

    // c_bulletPacket ����
    retval = send(s, (char*)&c_bullet, sizeof(c_bulletPacket), 0);
    if (retval == SOCKET_ERROR) err_display("send - c_bulletPacket");
}

void GameFramework::receiveGameData(SOCKET s)
{
    int retval, dataSize, vSize;

    // s_enemyPacket ����
    vector<s_enemyPacket> recv_enemies = {};
    retval = recv(s, (char*)&dataSize, sizeof(int), 0);
    if (retval == SOCKET_ERROR) err_display("receive - enemyPacketSize");
    vSize = dataSize / sizeof(s_enemyPacket);
    recv_enemies.resize(vSize);
    retval = recv(s, (char*)recv_enemies.data(), dataSize, 0);
    if (retval == SOCKET_ERROR) { err_display("recv - enemyPacket"); }
    // ���� ��Ŷ gameframework�� ����


    // s_itemPacket ����
    vector<s_itemPacket> recv_items = {};
    retval = recv(s, (char*)&dataSize, sizeof(int), 0);
    if (retval == SOCKET_ERROR) err_display("receive - itemPacketSize");
    vSize = dataSize / sizeof(s_itemPacket);
    recv_items.resize(vSize);
    retval = recv(s, (char*)recv_items.data(), dataSize, 0);
    if (retval == SOCKET_ERROR) err_display("receive - itemPacket");

    // s_obstaclePacket ����
    vector<s_obstaclePacket> recv_obstacles = {};
    retval = recv(s, (char*)&dataSize, sizeof(int), 0);
    if (retval == SOCKET_ERROR) err_display("receive - obstaclePacketSize");
    vSize = dataSize / sizeof(s_obstaclePacket);
    obstacles.resize(vSize);
    retval = recv(s, (char*)obstacles.data(), dataSize, 0);
    if (retval == SOCKET_ERROR) err_display("receive - obstaclePacket");


    // s_bulletPacket ����
    vector<s_bulletPacket> recv_bullets = {};
    retval = recv(s, (char*)&dataSize, sizeof(int), 0);
    if (retval == SOCKET_ERROR) err_display("receive - bulletPacketSize");
    vSize = dataSize / sizeof(s_bulletPacket);
    recv_bullets.resize(vSize);
    retval = recv(s, (char*)recv_bullets.data(), dataSize, 0);
    if (retval == SOCKET_ERROR) err_display("receive - bulletPacket");

    // s_playerPacket ����
    vector<s_playerPacket> recv_players = {};
    retval = recv(s, (char*)&dataSize, sizeof(int), 0);
    if (retval == SOCKET_ERROR) err_display("receive - playerPacketSize");
    vSize = dataSize / sizeof(s_playerPacket);
    recv_players.resize(vSize);
    retval = recv(s, (char*)recv_players.data(), dataSize, 0);
    if (retval == SOCKET_ERROR) err_display("receive - playerPacket");
    
    UpdatePlayerInfo(recv_players);

    cout << "receive game data" << endl;
}

void GameFramework::receiveResult(SOCKET s)
{
    int retval;

    s_UIPacket UIPacket;
    retval = recv(s, (char*)&UIPacket, sizeof(UIPacket), 0);
    if (retval == SOCKET_ERROR) err_display("receive - UIPacket");
}

void GameFramework::UpdatePlayerInfo(vector<s_playerPacket> packet)
{
    for (int i = 0; i < packet.size(); ++i)
    {
        if (players[i]->ID == player->ID)
        {
            player->name = packet[i].s_playerName;
            player->ID = packet[i].s_playerID;
            player->x = packet[i].s_playerPosX;
            player->y = packet[i].s_playerPosY;                          
            player->speed = packet[i].s_playerSpeed;                     
            player->health = packet[i].s_playerHealth;                   
            player->level = packet[i].s_playerLevel;                     
            player->experience = packet[i].s_playerEXP;                  
        }                                                                
        players[i]->name = packet[i].s_playerName;                       
        players[i]->ID = packet[i].s_playerID;                           
        players[i]->x = packet[i].s_playerPosX;                          
        players[i]->y = packet[i].s_playerPosY;                          
        players[i]->speed = packet[i].s_playerSpeed;                     
        players[i]->health = packet[i].s_playerHealth;                   
        players[i]->level = packet[i].s_playerLevel;                     
        players[i]->experience = packet[i].s_playerEXP;
    }
    // isDead ó�� �ʿ�
}