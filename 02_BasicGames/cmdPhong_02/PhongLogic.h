#pragma once

void InitLocaleAndTitle();
void InitEngineSystems();
void LoadAudioAssets();
void CreateUIObjects();
void InitGameSettings();

void UpdateMenuState();
void UpdateSettingState();
void UpdatePlayingState();
void UpdateMatchRules();

void RenderMenuState();
void RenderSettingState();
void RenderPlayingState();
void RenderDebugInfo();

void ReleaseUIObjects();

void ApplyDifficulty(enum Difficulty diff);
void ResetBall(int direction);
void DrawPlayerPaddle(int x, int y, int w, int h, int skinIdx);