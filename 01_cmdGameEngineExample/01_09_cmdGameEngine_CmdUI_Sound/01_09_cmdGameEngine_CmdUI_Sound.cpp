// 01_09_cmdGameEngine_CmdUI_Sound.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "ScreenBuffer.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "Collision.h"
#include "CmdUtil.h"
#include "TextObject.h"
#include "EditObject.h"
#include "SliderObject.h"
#include "CheckBoxObject.h"
#include "ButtonObject.h"
#include "SoundManager.h"
#include "../../00.cmdGameEngine/fmodSound_Lib/Header/fmod.h"
#pragma comment(lib, "../../00.cmdGameEngine/fmodSound_Lib/Library/x64/fmod_vc.lib")
using namespace CmdUI;

bool isModalOpen = false; // 팝업창 활성화 여부
int count = 0;            // 뒤에 있는 게임 변수

extern INPUTVALUE g_Input;
extern TIMEMANAGER g_Time;

bool bIsGameRun = false;
float totalPlayTime = 0.0f;

float g_BgmVolume = 0.5f; // 초기 볼륨 50%
float g_SfxVolume = 0.5f; // 초기 볼륨 50%
bool  g_IsMuted = false;

// 사운드 리스트 정의
enum SOUND_LIST { SND_BGM, SND_COIN, SND_GAMEOVER };


TEXT* titleText = nullptr;
TEXT* volumeStatusText = nullptr;
SLIDER* sfxVolumeSlider = nullptr;
SLIDER* bgmVolumeSlider = nullptr;
CHECKBOX* bgmMuteCheckBox = nullptr;
EDIT* edtID = nullptr;
BUTTON* playSfxButton = nullptr;
BUTTON* exitButton = nullptr;

bool HandleWideInput(EDIT* edit)
{
    if (edit == NULL || !edit->isFocused) return false;

    // 윈도우 메시지 큐를 직접 확인하여 결합된 와이드 문자(한글/영문) 추출
    MSG msg;
    while (PeekMessageW(&msg, NULL, WM_CHAR, WM_CHAR, PM_REMOVE))
    {
        wchar_t ch = (wchar_t)msg.wParam;

        // 백스페이스 처리 (글자 지우기)
        if (ch == L'\b')
        {
            if (edit->textLength > 0)
            {
                edit->textLength--;
                edit->text[edit->textLength] = L'\0';
                return true;
            }
        }
        // 엔터키나 ESC 입력 시 포커스 해제 등의 예외 처리
        else if (ch == L'\r' || ch == L'\n')
        {
            // 엔터 입력 시 처리 로직이 필요하다면 여기에 기술
            return true;
        }
        // 일반 유니코드 문자 (한글, 영문, 숫자 전체 대응)
        else if (ch >= 32)
        {
            if (edit->textLength < edit->maxLen)
            {
                edit->text[edit->textLength] = ch;
                edit->textLength++;
                edit->text[edit->textLength] = L'\0';
                return true;
            }
        }
    }
    return false;
}


void Awake()
{

}

void Start()
{
    ScreenInit();
    InputInit();
    TimeInit();
    SoundInit();

    SoundLoad(SND_BGM, "../../Sound/bgm.mp3", SOUND_GROUP_BGM, true);
    SoundLoad(SND_COIN, "../../Sound/coin.mp3", SOUND_GROUP_SFX, false);

    SoundPlay(SND_BGM);

    titleText = CreateText(10, 2, L"=== 게임 사운드 테스트 프로그램 ===", WHITE);
    volumeStatusText = CreateText(10, 4, L"배경 음악 소리: 50% [PLAYING]", YELLOW, CmdUI::TEXT_DYNAMIC);
    bgmVolumeSlider = CreateSlider(10, 6, 20, L"배경 음악 소리", 0, 100, (int)(g_BgmVolume * 100), WHITE, YELLOW);
    sfxVolumeSlider = CreateSlider(10, 8, 20, L"효과음 소리", 0, 100, (int)(g_BgmVolume * 100), WHITE, YELLOW);
    bgmMuteCheckBox = CreateCheckBox(10, 12, L"게임 사운드 끄기", false, WHITE, CYAN);
    edtID = CreateEdit(10, 15, 20, 20, L"ID : ", false, WHITE, DYELLOW, BK_DYELLOW | BLACK);
    playSfxButton = CreateButton(10, 16, 15, 3, L"코인 소리", WHITE, CYAN);
    exitButton = CreateButton(28, 16, 8, 3, L"EXIT", WHITE, RED);

    bIsGameRun = true;
}

void Update()
{
    InputUpdate();
    TimeUpdate();
    SoundUpdate();

    bool bBgmSliderChanged = false;
    bool bSfxSliderChanged = false;
    bool bMuteChanged = false;
    bool bEdtIDChanged = false;
    bool bPlayClicked = false;
    bool bExitClicked = false;

    if (bgmVolumeSlider)  bBgmSliderChanged = Update(bgmVolumeSlider, &g_Input);
    if (sfxVolumeSlider)  bSfxSliderChanged = Update(sfxVolumeSlider, &g_Input);
    if (bgmMuteCheckBox)  bMuteChanged = Update(bgmMuteCheckBox, &g_Input);
    if (edtID)            bEdtIDChanged = Update(edtID, &g_Input);
    if (playSfxButton)    bPlayClicked = Update(playSfxButton, &g_Input);
    if (exitButton)       bExitClicked = Update(exitButton, &g_Input);


    if ((bBgmSliderChanged && bgmVolumeSlider) || (bSfxSliderChanged && sfxVolumeSlider))
    {
        g_BgmVolume = bgmVolumeSlider->currentValue / 100.f;
        g_SfxVolume = sfxVolumeSlider->currentValue / 100.f;
        if (!g_IsMuted)
        {
            SoundSetVolume(SND_BGM, g_BgmVolume);
            SoundSetGroupVolume(SOUND_GROUP_SFX, g_SfxVolume);
        }
    }

    if (bMuteChanged && bgmMuteCheckBox)
    {
        g_IsMuted = bgmMuteCheckBox->isChecked;
        SoundSetVolume(SND_BGM, g_IsMuted ? 0.0f : g_BgmVolume);
        SoundSetGroupVolume(SOUND_GROUP_SFX, g_IsMuted ? 0.0f : g_SfxVolume);
    }

    if (bPlayClicked || (g_Input.keyState[0] == KEY_DOWN))
    {
        SoundPlay(SND_COIN);
    }

    if (bExitClicked)
    {
        bIsGameRun = false;
    }

    if (volumeStatusText)
    {
        wchar_t wszStatus[64];
        swprintf_s(wszStatus, L"BGM Volume: %d%%  [%s]", (int)(g_BgmVolume * 100), g_IsMuted ? L"MUTED" : L"PLAYING");
        SetText(volumeStatusText, wszStatus);
    }
}

void LateUpdate()
{

}

void PreRender()
{
    ScreenClear();
}

int currentTab = 0;

void Render()
{
    if (titleText)        Draw(titleText);
    if (volumeStatusText) Draw(volumeStatusText);
    if (bgmVolumeSlider)  Draw(bgmVolumeSlider);
    if (sfxVolumeSlider)  Draw(sfxVolumeSlider);
    if (bgmMuteCheckBox)  Draw(bgmMuteCheckBox);
    if (edtID)            Draw(edtID);
    if (playSfxButton)    Draw(playSfxButton);
    if (exitButton)       Draw(exitButton);
}

void PostRender()
{

    wchar_t dbgMsg[100];

    swprintf_s(dbgMsg, L"MouseX: %d | MouseY: %d | Click: %d",
        g_Input.mouseX, g_Input.mouseY, g_Input.isMouseLeftClick);

    SetScreenColor(YELLOW);
    ScreenPrint(0, 0, dbgMsg);

    ScreenFlipping();
}

void Release()
{
    if (titleText)        Release(titleText);
    if (volumeStatusText) Release(volumeStatusText);
    if (bgmVolumeSlider)  Release(bgmVolumeSlider);
    if (sfxVolumeSlider)  Release(sfxVolumeSlider);
    if (bgmMuteCheckBox)  Release(bgmMuteCheckBox);
    if (edtID)            Release(edtID);
    if (playSfxButton)    Release(playSfxButton);
    if (exitButton)       Release(exitButton);
    SoundRelease();
    ScreenRelease();
}

int main(void)
{
    Awake();
    Start();

    while (bIsGameRun)
    {
        Update();
        LateUpdate();

        PreRender();
        Render();
        PostRender();

        WaitForNextFrame();
        //Sleep(g_Time.targetFrameTime);

        totalPlayTime += g_Time.deltaTime;
        //if ((totalPlayTime += g_Time.deltaTime) >= 10.0f) bIsGameRun = false;
    }
    Release();
    return 0;
}
// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
