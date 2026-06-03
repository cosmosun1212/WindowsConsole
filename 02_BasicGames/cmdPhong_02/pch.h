#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <locale.h>
#include <math.h>

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
#include <fmod.h>

#include "PhongLogic.h"
#include "WorkSpace.h"

using namespace CmdUI;

enum SOUND_LIST 
{
    SND_MENU_CLICK = 0,
    SND_GAME_HIT = 1,
    SND_GAME_WIN = 2,
    SND_GAME_LOSE = 3,
    SND_TEST_BGM = 4,
    SND_TEST_COIN = 5
};

extern INPUTVALUE g_Input;
extern TIMEMANAGER g_Time;

enum GameState { STATE_MENU, STATE_SETTING, STATE_PLAYING };
enum Difficulty { DIFF_EASY, DIFF_NORMAL, DIFF_HARD };

extern GameState currentState;
extern GameState previousState;
extern Difficulty currentDiff;

extern bool bIsGameRun;
extern float totalPlayTime;

extern float g_BgmVolume;
extern float g_SfxVolume;
extern bool g_IsMuted;

extern Rect playerPaddle;
extern Rect enemyPaddle;
extern Rect ball;

extern float paddleSpeed;
extern float aiSpeed;
extern float ballSpeedX;
extern float ballSpeedY;

extern int playerScore;
extern int enemyScore;
extern int playerSkin;

extern TEXT* txtMenuTitle;
extern BUTTON* btnStart;
extern BUTTON* btnSetting;
extern BUTTON* btnExit;

extern TEXT* txtSettingTitle;
extern TEXT* volumeStatusText;
extern SLIDER* bgmVolumeSlider;
extern SLIDER* sfxVolumeSlider;
extern CHECKBOX* bgmMuteCheckBox;
extern BUTTON* btnPrevSkin;
extern BUTTON* btnNextSkin;
extern BUTTON* btnTestSFX;
extern BUTTON* btnBack;