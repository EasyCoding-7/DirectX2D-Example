#include "stdafx.h"
#include "Device.h"

int APIENTRY WinMain
(
	HINSTANCE hInstance,	// 프로그램 자체 핸들
	HINSTANCE preInstance,
	LPSTR lpszCmdparam,
	int nCmdShow		// 어떻게 보여졌으면 하는가.( 최대화 최소화 등)
)
{
	InitWindow(hInstance, nCmdShow);	// 윈도우 초기화

	Running(hInstance); 				// while(true)
	Destroy();

	return 0;
}


// extern 변수들
HWND Hwnd = NULL;

UINT Width = 1024;
UINT Height = 768;

wstring Title{ L"Direct2D Title" };

void InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Register Window
	{
		WNDCLASSEX wc; // 윈도우 클래스 생성
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW; // 윈도우 크기 변경->다시그림
		wc.lpfnWndProc = WndProc; // 윈도우 메세지 처리를 위한 함수명
		wc.cbClsExtra = NULL; // 여분 메모리
		wc.cbWndExtra = NULL;
		wc.hInstance = hInstance; // 응용 프로그램 인스턴스
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); // 로드하는 아이콘
		wc.hCursor = LoadCursor(NULL, IDC_ARROW); // 로드하는 커서
		wc.hbrBackground = static_cast<HBRUSH>(WHITE_BRUSH); // 배경 색
		wc.lpszMenuName = NULL; // 메뉴
		wc.lpszClassName = Title.c_str(); // 윈도우 클래스 이름
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		// RegisterClassEx : 윈도우 클래스 커널에 등록
			// (생성에 문제가 있으면 0, 성공하면 1 반환)
		WORD check = RegisterClassEx(&wc);
		assert(check != NULL);
	}

	// Create Handle
	{
		Hwnd = CreateWindowEx		// 윈도우를 만드는 함수
		(
			NULL,
			Title.c_str(),		// 윈도우 클래스 이름
			Title.c_str(),		// 타이틀바 이름
			WS_OVERLAPPEDWINDOW, // 최소, 최대, 닫기, 오른쪽 마우스 등
			CW_USEDEFAULT,		// 윈도우 창이 나타날 x좌표
			CW_USEDEFAULT,		// 윈도우 창이 나타날 y 좌표
			Width,			// 가로 크기
			Height,			// 세로 크기
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
		// PeekMessage : 메세지가 들어와 있는지만을 검사 후 바로 리턴

		/*
		PM_REMOVE : 메세지가 있을경우 GetMessage()와 같이 행동
		메세지가 없을경우 PeekMessage(게임에 적합)
		*/

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);	// 해석
			DispatchMessage(&msg);	// 보고
		}
		else
		{
			// Main.cpp
			Update(); // 처리
			Render(); // DC에서 가져온 정보를 Render에 뿌려줌
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