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

IDXGISwapChain* SwapChain;	// 더블버퍼

// pc 자원 생성, CPU와 RAM의 영역, 기능 지원 점검과 자원 할당
ID3D11Device* Device;

// 도화지, 그래픽 카드 영역, 그려주기 위한 총괄 DC,
// 렌더 대상 설정, GPU가 수행할 랜더링 명령 지시
ID3D11DeviceContext* DeviceContext;

// 실제 도화지, WindowAPI의 HDC 역할
ID3D11RenderTargetView* RTV;

ID3D11VertexShader* VertexShader;
ID3D11PixelShader* PixelShader;

// Blob : Binary large object
// 멀티미디어 데이터를 다룰때 사용(바이트 코드를 담기위함?)
ID3D10Blob* VsBlob;
ID3D10Blob* PsBlob;

void InitDirect3D(HINSTANCE hInstance)
{
	// SwapChain
	/// SwapChain.Buffer
	DXGI_MODE_DESC bufferDesc; // 후면버퍼의 속성들을 서술하는 구조체
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	bufferDesc.RefreshRate.Numerator = 60; // Refresh Rate : 화면 재생 빈도
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 디스플레이 스캔 라인 모드
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	// 어떤 방식의 텍스쳐를 사용할 것인가
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	/*
	UNORM : 0과 1로 한정된 값. float과는 다른 정수 값  8bit UNORM이면,
	그 값이 127일때 이는 0.5로 인식된다.
	*/

	/// SwapChain.Sampling.. etc
	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	// 이중버퍼링을 할지 삼중버퍼링을 할지 정수의 숫자를 넣는다.
	swapDesc.BufferCount = 1;

	swapDesc.BufferDesc = bufferDesc;

	// 어디에 쓸것인가를 지정
	// DXGI_USAGE_RENDER_TARGET_OUTPUT : 표면, 자원을 출력 렌더 타겟으로 사용
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	swapDesc.OutputWindow = Hwnd;

	// 픽셀당 추출할 표본의 개수
	// 멀티 샘플링을 사용하지 않으면 count=1, 품질수준 0으로 설정
	swapDesc.SampleDesc.Count = 1;

	swapDesc.SampleDesc.Quality = 0; //원하는 품질 수준 지정

	// 버퍼를 언제 비워줄것인지
	// DXGI_SWAP_EFFECT_DISCARD : 전에 그렸던거 폐기
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	swapDesc.Windowed = TRUE;

	// Featured Level
	vector<D3D_FEATURE_LEVEL> featured_level =
	{
		D3D_FEATURE_LEVEL_9_1,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_11_0,
		//D3D_FEATURE_LEVEL_11_1
	};

	// Create 

	// HRESULT : 함수의 호출 결과를 나타내는 32비트 값을 표현
	HRESULT hr = D3D11CreateDeviceAndSwapChain
	(
		NULL, // 비디오카드 인터페이스 설정, NULL : 최초 발견된 디바이스
		D3D_DRIVER_TYPE_HARDWARE,
		NULL, // 래스터라이저가 구현되어 있는 DLL이 핸들을 지정.
		NULL, // 사용할 DX11의 API 레이어
		featured_level.data(), // 피처 레벨 배열 설정, 높은순 11_1 -> 9_1
		featured_level.size(), // 피처 레벨 배열 개수
		D3D11_SDK_VERSION, // 사용중인 SDK 버전
		&swapDesc, // swapChain 설정값(사양서)
		&SwapChain, // 생성된 swapChain 포인터를 담을 변수
		&Device, // 생성된 Device 포인터를 담을 변수
		NULL,
		&DeviceContext // 생성된 DeviceContext 포인터를 담을 변수
	);
	assert(SUCCEEDED(hr));

	// BackBuffer
	ID3D11Texture2D* BackBuffer;

	// __uuidof(ID3D11Texture2D) : 오브젝트에 대한 식별자를 가져온다?
	// Getbuffer : 스왑체인의 0번째 버퍼를 얻는다.
	hr = SwapChain->GetBuffer
	(
		0,
		__uuidof(ID3D11Texture2D),
		(void**)&BackBuffer
	);
	assert(SUCCEEDED(hr));

	// Create RTV 
	// BackBuffer에 렌더링, 렌더링 뷰 정보 설정 구조체 ( NULL : 기본 뷰)
	// , 생성한 뷰의 포인터를 반환
	hr = Device->CreateRenderTargetView(BackBuffer, NULL, &RTV);
	assert(SUCCEEDED(hr));
	BackBuffer->Release();

	// OM : 출력을 위해 부가적인 요소(스텐실, 깊이)등을 병합해줌
	// 후면 버퍼에 기록, 블랜딩 등
	DeviceContext->OMSetRenderTargets(1, &RTV, NULL);


	// Create Shader
	{
		/// Vertex Blob
		HRESULT hr;

		// D3DX11CompileFromFile : 쉐이더를 컴파일할때 사용하는 함수
		hr = D3DX11CompileFromFile
		(
			L"effect.fx",
			0, // 특별히 사용 안함0
			0, // Shader에서 include 정보를 다룰때 사용?
			"VS", // Shader File의 함수 진입점
			"vs_4_0_level_9_1", // 사용할 Shader Version
			0, // Shader Code의 컴파일 방식, 0 : 최적화 적용
			0, // 특별히 사용 안함, 0
			0, // Shader를 비동기적으로 컴파일 하기위한 옵션
			&VsBlob, // 컴파일된 Shader를 담는 구조체 가리키는 포인터
			0, // Error Message를 담은 구조체 가리키는 포인터
			0  // 비동기 컴파일시 에러 코드를 조회, 0 : 비동기 사용 x
		);
		assert(SUCCEEDED(hr));

		/// Pixel Blob
		hr = D3DX11CompileFromFile
		(
			L"Effect.fx",
			0,
			0,
			"PS",
			"ps_4_0_level_9_1",
			0,
			0,
			0,
			&PsBlob,
			0,
			0
		);
		assert(SUCCEEDED(hr));

		/// Create VertexShader

		// 컴파일된 쉐이더를 가지고 쉐이더 변수 만들기.
		hr = Device->CreateVertexShader
		(
			VsBlob->GetBufferPointer(), // 컴파일된 쉐이더 포인터
			VsBlob->GetBufferSize(), // 컴파일된 버텍스 쉐이더 크기
			NULL,
			&VertexShader // 버텍스 쉐이더의 주소
		);
		assert(SUCCEEDED(hr));

		/// Create PixelShader
		hr = Device->CreatePixelShader
		(
			PsBlob->GetBufferPointer(),
			PsBlob->GetBufferSize(),
			NULL,
			&PixelShader
		);
		assert(SUCCEEDED(hr));


		/// Set Shader
		DeviceContext->VSSetShader(VertexShader, 0, 0);
		DeviceContext->PSSetShader(PixelShader, 0, 0);
	}

	// Create ViewPort
	{
		// 플레이어의 시야, 픽셀화 시켜서 보여줌?
		// 렌더 타겟에 렌더링을 하기 위해서는 뷰포트의 설정도 필요
		// (ViewPort == 렌더타겟의 렌더링 영역에 관한 설정)
		// 렌더 타겟이 8개면 뷰포트 또한 8개 설정

		/*
		버텍스 셰이더)'에서 출력되는 정점의 위치좌표(3D)는
		주사변환이 완료된 주사변환 좌표계의 값. (-1<=x<=1, -1<=y<=1, 0<=z<=1)

		래스터라이저(RS)는 뷰포트 변환을 통해서,
		이 3D 좌표 위치로부터 2D 스크린 좌표(0<=x<=1, 0<=y<=1)을 계산

		스크린 좌표 : 왼쪽 위(0,0), 오른쪽 아래(1,1)
		*/

		D3D11_VIEWPORT viewPort;
		ZeroMemory(&viewPort, sizeof(D3D11_VIEWPORT));
		viewPort.TopLeftX = 0; // 뷰포트 영역의 좌상 X 좌표
		viewPort.TopLeftY = 0; // 뷰포트 영역의 좌상 Y 좌표
		viewPort.Width = Width; // 뷰포트 영역의 넓이 (오른쪽 하단 X)
		viewPort.Height = Height; // 뷰포트 영역의 높이 (오른쪽 하단 Y)
		viewPort.MaxDepth = 0.0f; // 뷰포트 영역의 깊이값 최대값
		viewPort.MinDepth = 0.0f; // 뷰포트 영역의 깊이값 최소값

		// 뷰포트 설정하는 함수
		// (렌더타겟 개수, 뷰포트를 설정할 뷰포트 구조체 배열)
		DeviceContext->RSSetViewports(1, &viewPort);
	}
}

void Destroy() // 마지막에 생성한것을 먼저 Release() 해준다.
{
	PixelShader->Release();
	VertexShader->Release();
	PsBlob->Release();
	VsBlob->Release();

	RTV->Release();
	DeviceContext->Release();
	SwapChain->Release();
	Device->Release();
}

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

	InitDirect3D(hInstance);
	ShowWindow(Hwnd, nCmdShow);
	UpdateWindow(Hwnd);
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