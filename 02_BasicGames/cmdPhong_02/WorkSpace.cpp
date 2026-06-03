#include "pch.h"

void Awake()
{
    InitLocaleAndTitle();
}

void Start()
{
    InitEngineSystems();
    LoadAudioAssets();
    CreateUIObjects();
    InitGameSettings();
}

void Update()
{
    InputUpdate();
    TimeUpdate();
    SoundUpdate();

    if (currentState == STATE_MENU)
    {
        UpdateMenuState();
    }
    else if (currentState == STATE_SETTING)
    {
        UpdateSettingState();
    }
    else if (currentState == STATE_PLAYING)
    {
        UpdatePlayingState();
    }
}

void LateUpdate()
{
    if (currentState == STATE_PLAYING)
    {
        UpdateMatchRules();
    }
}

void PreRender()
{
    ScreenClear();
}

void Render()
{
    if (currentState == STATE_MENU)
    {
        RenderMenuState();
    }
    else if (currentState == STATE_SETTING)
    {
        RenderSettingState();
    }
    else
    {
        RenderPlayingState();
    }
}

void PostRender()
{
    RenderDebugInfo();
    ScreenFlipping();
}

void Release()
{
    ReleaseUIObjects();
    SoundRelease();
    ScreenRelease();
}