#pragma once

#include "stdafx.h"

// Function
void InitWindow(HINSTANCE hInstance, int nCmdShow);
void Destroy();
WPARAM Running(HINSTANCE hInstance);

// �޼��� ó��
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main.cpp
void InitScene();
void DestroyScene();
void Update();
void Render();