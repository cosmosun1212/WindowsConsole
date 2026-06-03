#include "pch.h"

extern INPUTVALUE g_Input;
extern TIMEMANAGER g_Time;

void InitLocaleAndTitle()
{
    _wsetlocale(LC_ALL, L"korean");
    SetConsoleTitle(L"Dj__Kim's Advanced PONG GAME");
}

void InitEngineSystems()
{
    ScreenInit();
    InputInit();
    TimeInit();
    SoundInit();
}

void LoadAudioAssets()
{
    SoundLoad(SND_MENU_CLICK, "../../SoundAsset/coin.mp3", SOUND_GROUP_MENU, false);
    SoundLoad(SND_GAME_HIT, "../../SoundAsset/game_hit.mp3", SOUND_GROUP_SFX, false);
    SoundLoad(SND_GAME_WIN, "../../SoundAsset/button2.mp3", SOUND_GROUP_MENU, false);
    SoundLoad(SND_GAME_LOSE, "../../SoundAsset/button.mp3", SOUND_GROUP_MENU, false);
    SoundLoad(SND_TEST_BGM, "../../SoundAsset/bgm2.mp3", SOUND_GROUP_BGM, true);
    SoundLoad(SND_TEST_COIN, "../../SoundAsset/coin.mp3", SOUND_GROUP_SFX, false);
}

void CreateUIObjects()
{
    txtMenuTitle = CreateText(25, 3, L"=== PREMIUM PONG GAME ===", YELLOW);
    btnStart = CreateButton(30, 11, 20, 3, L"   게임 시작    ", WHITE, BK_BLUE | CYAN);
    btnSetting = CreateButton(30, 15, 20, 3, L"   환경 설정    ", WHITE, BK_PURPLE | DYELLOW);
    btnExit = CreateButton(30, 19, 20, 3, L"   게임 종료    ", WHITE, BK_DRED | RED);

    txtSettingTitle = CreateText(26, 2, L"⚙️  SYSTEM SETTINGS  ⚙️", CYAN);
    volumeStatusText = CreateText(10, 14, L"BGM Volume: 50%  [PLAYING]", YELLOW, CmdUI::TEXT_DYNAMIC);

    bgmVolumeSlider = CreateSlider(10, 17, 20, L"배경 음악 소리", 0, 100, (int)(g_BgmVolume * 100), WHITE, YELLOW);
    sfxVolumeSlider = CreateSlider(10, 21, 20, L"효과음 소리", 0, 100, (int)(g_SfxVolume * 100), WHITE, YELLOW);
    bgmMuteCheckBox = CreateCheckBox(10, 12, L"게임 오디오 전체 음소거", false, WHITE, CYAN);

    btnPrevSkin = CreateButton(45, 12, 5, 3, L" ◀ ", WHITE, BK_DGRAY | YELLOW);
    btnNextSkin = CreateButton(63, 12, 5, 3, L" ▶ ", WHITE, BK_DGRAY | YELLOW);

    btnTestSFX = CreateButton(10, 23, 12, 1, L"  SFX PLAY ", WHITE, BK_DGRAY | BLACK);
    btnBack = CreateButton(4, 24, 14, 1, L"  뒤로 가기  ", WHITE, BK_GRAY | GREEN);
}

void InitGameSettings()
{
    SoundSetGroupVolume(SOUND_GROUP_BGM, g_BgmVolume);
    SoundSetGroupVolume(SOUND_GROUP_SFX, g_SfxVolume);
    ApplyDifficulty(DIFF_NORMAL);
    bIsGameRun = true;
}

void UpdateMenuState()
{
    bool bStartClicked = CmdUI::Update(btnStart, &g_Input);
    bool bSettingClicked = CmdUI::Update(btnSetting, &g_Input);
    bool bExitClicked = CmdUI::Update(btnExit, &g_Input);

    if (bStartClicked || (g_Input.keyState[2] == KEY_DOWN))
    {
        SoundPlay(SND_MENU_CLICK);
        SoundStop(SND_TEST_BGM);
        previousState = STATE_MENU;
        currentState = STATE_PLAYING;
        playerScore = 0;
        enemyScore = 0;
        ResetBall(1);
    }
    else if (bSettingClicked)
    {
        SoundPlay(SND_MENU_CLICK);
        if (!g_IsMuted)
        {
            SoundPlay(SND_TEST_BGM);
        }
        previousState = STATE_MENU;
        currentState = STATE_SETTING;
    }
    else if (bExitClicked)
    {
        bIsGameRun = false;
    }
}

void UpdateSettingState()
{
    if (CmdUI::Update(btnBack, &g_Input))
    {
        SoundPlay(SND_MENU_CLICK);
        SoundStop(SND_TEST_BGM);
        previousState = STATE_SETTING;
        currentState = STATE_MENU;
        return;
    }

    if (g_Input.isMouseLeftClick)
    {
        if (g_Input.mouseX >= 10 && g_Input.mouseX <= 35)
        {
            if (g_Input.mouseY == 6) { SoundPlay(SND_MENU_CLICK); ApplyDifficulty(DIFF_EASY); }
            if (g_Input.mouseY == 8) { SoundPlay(SND_MENU_CLICK); ApplyDifficulty(DIFF_NORMAL); }
            if (g_Input.mouseY == 10) { SoundPlay(SND_MENU_CLICK); ApplyDifficulty(DIFF_HARD); }
        }
    }

    bool bBgmSliderChanged = CmdUI::Update(bgmVolumeSlider, &g_Input);
    bool bSfxSliderChanged = CmdUI::Update(sfxVolumeSlider, &g_Input);
    bool bMuteChanged = CmdUI::Update(bgmMuteCheckBox, &g_Input);
    bool bPlayClicked = CmdUI::Update(btnTestSFX, &g_Input);

    if (bBgmSliderChanged || bSfxSliderChanged)
    {
        g_BgmVolume = bgmVolumeSlider->currentValue / 100.f;
        g_SfxVolume = sfxVolumeSlider->currentValue / 100.f;

        if (!g_IsMuted)
        {
            SoundSetVolume(SND_TEST_BGM, g_BgmVolume);
            SoundSetGroupVolume(SOUND_GROUP_SFX, g_SfxVolume);
        }
    }

    if (bMuteChanged)
    {
        g_IsMuted = bgmMuteCheckBox->isChecked;
        SoundSetVolume(SND_TEST_BGM, g_IsMuted ? 0.0f : g_BgmVolume);
        SoundSetGroupVolume(SOUND_GROUP_SFX, g_IsMuted ? 0.0f : g_SfxVolume);
    }

    if (bPlayClicked)
    {
        SoundPlay(SND_TEST_COIN);
    }

    bool isLeftSkin = CmdUI::Update(btnPrevSkin, &g_Input);
    bool isRightSkin = CmdUI::Update(btnNextSkin, &g_Input);

    if (isLeftSkin)
    {
        SoundPlay(SND_MENU_CLICK);
        if (--playerSkin < 0) playerSkin = 4;
        while (g_Input.isMouseLeftDown) { InputUpdate(); }
    }
    if (isRightSkin)
    {
        SoundPlay(SND_MENU_CLICK);
        if (++playerSkin > 4) playerSkin = 0;
        while (g_Input.isMouseLeftDown) { InputUpdate(); }
    }

    if (volumeStatusText)
    {
        wchar_t wszStatus[64];
        swprintf_s(wszStatus, L"BGM Volume: %d%%  [%s]", (int)(g_BgmVolume * 100), g_IsMuted ? L"MUTED" : L"PLAYING");
        SetText(volumeStatusText, wszStatus);
    }
}

void UpdatePlayingState()
{
    if (previousState == STATE_MENU)
    {
        if (!g_IsMuted)
        {
            SoundSetVolume(SND_TEST_BGM, g_BgmVolume);
            SoundPlay(SND_TEST_BGM);
        }
        previousState = STATE_PLAYING;
    }

    if (g_Input.keyState[5] == KEY_HOLD) playerPaddle.y -= paddleSpeed * g_Time.deltaTime;
    if (g_Input.keyState[6] == KEY_HOLD) playerPaddle.y += paddleSpeed * g_Time.deltaTime;

    if (playerPaddle.y < 0) playerPaddle.y = 0;
    if (playerPaddle.y > 25 - playerPaddle.h) playerPaddle.y = 25 - playerPaddle.h;

    float targetY = ball.y - (enemyPaddle.h / 2);
    if (enemyPaddle.y < targetY) enemyPaddle.y += aiSpeed * g_Time.deltaTime;
    if (enemyPaddle.y > targetY) enemyPaddle.y -= aiSpeed * g_Time.deltaTime;

    if (enemyPaddle.y < 0) enemyPaddle.y = 0;
    if (enemyPaddle.y > 25 - enemyPaddle.h) enemyPaddle.y = 25 - enemyPaddle.h;

    ball.x += ballSpeedX * g_Time.deltaTime;
    ball.y += ballSpeedY * g_Time.deltaTime;

    if (ball.y <= 0) { ball.y = 0; ballSpeedY *= -1; SoundPlay(SND_GAME_HIT); }
    if (ball.y >= 25 - ball.h) { ball.y = 25 - ball.h; ballSpeedY *= -1; SoundPlay(SND_GAME_HIT); }

    if (IsColliding(ball, playerPaddle))
    {
        ball.x = playerPaddle.x + playerPaddle.w;
        ballSpeedX = fabs(ballSpeedX) + 3.0f;
        SoundPlay(SND_GAME_HIT);
    }
    if (IsColliding(ball, enemyPaddle))
    {
        ball.x = enemyPaddle.x - ball.w;
        ballSpeedX = -fabs(ballSpeedX) - 3.0f;
        SoundPlay(SND_GAME_HIT);
    }

    if (ball.x < 0) { enemyScore++; ResetBall(1); Sleep(300); }
    else if (ball.x > 80) { playerScore++; ResetBall(-1); Sleep(300); }
}

void UpdateMatchRules()
{
    if (playerScore >= 5 || enemyScore >= 5)
    {
        if (playerScore >= 5) SoundPlay(SND_GAME_WIN);
        else                  SoundPlay(SND_GAME_LOSE);

        SoundStop(SND_TEST_BGM);

        MessageBox(NULL, playerScore >= 5 ? L"🎉 MATCH WIN! 🎉" : L"💀 MATCH LOSE...", L"게임 종료", MB_OK);

        currentState = STATE_MENU;
        previousState = STATE_MENU;
        playerScore = 0;
        enemyScore = 0;
    }
}

void RenderMenuState()
{
    if (txtMenuTitle) Draw(txtMenuTitle);

    SetScreenColor(YELLOW);
    ScreenPrint(22, 5, L"  ██████╗  ██████╗ ███╗   ██╗ ██████╗  ");
    ScreenPrint(22, 6, L"  ██╔══██╗██╔═══██╗████╗  ██║██╔════╝  ");
    ScreenPrint(22, 7, L"  ██████╔╝██║   ██║██╔██╗ ██║██║  ███╗ ");
    ScreenPrint(22, 8, L"  ██╔═══╝ ██║   ██║██║╚██╗██║██║   ██║ ");
    ScreenPrint(22, 9, L"  ██║     ╚██████╔╝██║ ╚████║╚██████╔╝ ");
    ScreenPrint(22, 10, L"  ╚═╝      ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝  ");

    Draw(btnStart);
    Draw(btnSetting);
    Draw(btnExit);
}

void RenderSettingState()
{
    if (txtSettingTitle) Draw(txtSettingTitle);

    SetScreenColor(currentDiff == DIFF_EASY ? GREEN : GRAY);
    ScreenPrint(10, 6, currentDiff == DIFF_EASY ? L" [🟢] EASY MODE   (AI 속도: 25%)" : L" [  ] EASY MODE");

    SetScreenColor(currentDiff == DIFF_NORMAL ? YELLOW : GRAY);
    ScreenPrint(10, 8, currentDiff == DIFF_NORMAL ? L" [🟡] NORMAL MODE (AI 속도: 40%)" : L" [  ] NORMAL MODE");

    SetScreenColor(currentDiff == DIFF_HARD ? RED : GRAY);
    ScreenPrint(10, 10, currentDiff == DIFF_HARD ? L" [🔴] HARD MODE   (AI 속도: 70%)" : L" [  ] HARD MODE");

    if (volumeStatusText) Draw(volumeStatusText);
    if (bgmVolumeSlider)  Draw(bgmVolumeSlider);
    if (sfxVolumeSlider)  Draw(sfxVolumeSlider);
    if (bgmMuteCheckBox)  Draw(bgmMuteCheckBox);
    if (btnTestSFX)       Draw(btnTestSFX);

    SetScreenColor(PURPLE);
    ScreenPrint(47, 7, L"👕 SKIN REALTIME PREVIEW");

    DrawRect(51, 9, 11, 8, GRAY);
    DrawPlayerPaddle(55, 10, 2, 6, playerSkin);

    Draw(btnPrevSkin);
    Draw(btnNextSkin);
    Draw(btnBack);
}

void RenderPlayingState()
{
    for (int i = 0; i < 25; i += 2)
    {
        SetScreenColor(DGRAY);
        ScreenPrint(40, i, L"┇");
    }

    wchar_t scoreBuffer[30];
    swprintf_s(scoreBuffer, L"PLAYER  %02d   |   %02d  A.I", playerScore, enemyScore);
    SetScreenColor(YELLOW);
    ScreenPrint(28, 1, scoreBuffer);

    DrawPlayerPaddle((int)playerPaddle.x, (int)playerPaddle.y, playerPaddle.w, playerPaddle.h, playerSkin);
    DrawRect((int)enemyPaddle.x, (int)enemyPaddle.y, enemyPaddle.w, enemyPaddle.h, RED);
    DrawRect((int)ball.x, (int)ball.y, ball.w, ball.h, WHITE);
}

void RenderDebugInfo()
{
    wchar_t dbgMsg[100];
    swprintf_s(dbgMsg, L"MouseX: %d | MouseY: %d | Click: %d | FPS: %02d",
        g_Input.mouseX, g_Input.mouseY, g_Input.isMouseLeftClick, g_Time.fps);

    SetScreenColor(YELLOW);
    ScreenPrint(0, 0, dbgMsg);
}

void ReleaseUIObjects()
{
    if (txtMenuTitle)  Release(txtMenuTitle);
    if (txtSettingTitle)  Release(txtSettingTitle);
    if (volumeStatusText) Release(volumeStatusText);
    if (bgmVolumeSlider)  Release(bgmVolumeSlider);
    if (sfxVolumeSlider)  Release(sfxVolumeSlider);
    if (bgmMuteCheckBox)  Release(bgmMuteCheckBox);
    if (btnStart)         Release(btnStart);
    if (btnSetting)       Release(btnSetting);
    if (btnExit)          Release(btnExit);
    if (btnBack)          Release(btnBack);
    if (btnPrevSkin)      Release(btnPrevSkin);
    if (btnNextSkin)      Release(btnNextSkin);
    if (btnTestSFX)       Release(btnTestSFX);
}

void ApplyDifficulty(Difficulty diff)
{
    currentDiff = diff;
    switch (currentDiff)
    {
    case DIFF_EASY:   aiSpeed = paddleSpeed * 0.25f; break;
    case DIFF_NORMAL: aiSpeed = paddleSpeed * 0.40f; break;
    case DIFF_HARD:   aiSpeed = paddleSpeed * 0.70f; break;
    }
}

void ResetBall(int direction)
{
    ball.x = 40.0f;
    ball.y = 12.0f;
    ballSpeedX = (direction == 1) ? 30.0f : -30.0f;
    ballSpeedY = (rand() % 2 == 0) ? 15.0f : -15.0f;
}

void DrawPlayerPaddle(int x, int y, int w, int h, int skinIdx)
{
    unsigned short colors[5] = { CYAN, GREEN, PURPLE, YELLOW, WHITE };
    unsigned short finalColor = colors[skinIdx];

    DrawRect(x, y, w, h, finalColor);

    switch (skinIdx)
    {
    case 1:
        for (int i = 0; i < h; i += 2) ScreenPrint(x, y + i, L"▒▒");
        break;
    case 2:
        ScreenPrint(x, y, L"▲▲");
        ScreenPrint(x, y + h - 1, L"▼▼");
        break;
    case 3:
        ScreenPrint(x, y + (h / 2), L"▣▣");
        break;
    case 4:
        for (int i = 0; i < h; i++) ScreenPrint(x + 1, y + i, L"||");
        break;
    }
}