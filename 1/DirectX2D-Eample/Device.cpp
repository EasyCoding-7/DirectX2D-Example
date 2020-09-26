#include "stdafx.h"
#include "Device.h"

int APIENTRY WinMain
(
	HINSTANCE hInstance,	// ���α׷� ��ü �ڵ�
	HINSTANCE preInstance,
	LPSTR lpszCmdparam,
	int nCmdShow		// ��� ���������� �ϴ°�.( �ִ�ȭ �ּ�ȭ ��)
)
{
	InitWindow(hInstance, nCmdShow);	// ������ �ʱ�ȭ

	Running(hInstance); 				// while(true)
	Destroy();

	return 0;
}


// extern ������
HWND Hwnd = NULL;

UINT Width = 1024;
UINT Height = 768;

wstring Title{ L"Direct2D Title" };

void InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Register Window
	{
		WNDCLASSEX wc; // ������ Ŭ���� ����
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW; // ������ ũ�� ����->�ٽñ׸�
		wc.lpfnWndProc = WndProc; // ������ �޼��� ó���� ���� �Լ���
		wc.cbClsExtra = NULL; // ���� �޸�
		wc.cbWndExtra = NULL;
		wc.hInstance = hInstance; // ���� ���α׷� �ν��Ͻ�
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); // �ε��ϴ� ������
		wc.hCursor = LoadCursor(NULL, IDC_ARROW); // �ε��ϴ� Ŀ��
		wc.hbrBackground = static_cast<HBRUSH>(WHITE_BRUSH); // ��� ��
		wc.lpszMenuName = NULL; // �޴�
		wc.lpszClassName = Title.c_str(); // ������ Ŭ���� �̸�
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		// RegisterClassEx : ������ Ŭ���� Ŀ�ο� ���
			// (������ ������ ������ 0, �����ϸ� 1 ��ȯ)
		WORD check = RegisterClassEx(&wc);
		assert(check != NULL);
	}

	// Create Handle
	{
		Hwnd = CreateWindowEx		// �����츦 ����� �Լ�
		(
			NULL,
			Title.c_str(),		// ������ Ŭ���� �̸�
			Title.c_str(),		// Ÿ��Ʋ�� �̸�
			WS_OVERLAPPEDWINDOW, // �ּ�, �ִ�, �ݱ�, ������ ���콺 ��
			CW_USEDEFAULT,		// ������ â�� ��Ÿ�� x��ǥ
			CW_USEDEFAULT,		// ������ â�� ��Ÿ�� y ��ǥ
			Width,			// ���� ũ��
			Height,			// ���� ũ��
			NULL,
			NULL,
			hInstance,
			NULL
		);
		assert(Hwnd != NULL);
	}

	ShowWindow(Hwnd, nCmdShow);
	UpdateWindow(Hwnd);
}

void Destroy()
{

}

WPARAM Running(HINSTANCE hInstance)
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	InitScene();

	while (true)
	{
		// PeekMessage : �޼����� ���� �ִ������� �˻� �� �ٷ� ����

		/*
		PM_REMOVE : �޼����� ������� GetMessage()�� ���� �ൿ
		�޼����� ������� PeekMessage(���ӿ� ����)
		*/

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);	// �ؼ�
			DispatchMessage(&msg);	// ����
		}
		else
		{
			// Main.cpp
			Update(); // ó��
			Render(); // DC���� ������ ������ Render�� �ѷ���
		}
	}

	////////////////////////////////////////

	UnregisterClass(Title.c_str(), hInstance);
	DestroyWindow(Hwnd);
	DestroyScene();		// Main.cpp

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}