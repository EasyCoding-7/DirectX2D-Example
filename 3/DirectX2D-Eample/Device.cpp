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

IDXGISwapChain* SwapChain;	// �������

// pc �ڿ� ����, CPU�� RAM�� ����, ��� ���� ���˰� �ڿ� �Ҵ�
ID3D11Device* Device;

// ��ȭ��, �׷��� ī�� ����, �׷��ֱ� ���� �Ѱ� DC,
// ���� ��� ����, GPU�� ������ ������ ��� ����
ID3D11DeviceContext* DeviceContext;

// ���� ��ȭ��, WindowAPI�� HDC ����
ID3D11RenderTargetView* RTV;

ID3D11VertexShader* VertexShader;
ID3D11PixelShader* PixelShader;

// Blob : Binary large object
// ��Ƽ�̵�� �����͸� �ٷ궧 ���(����Ʈ �ڵ带 �������?)
ID3D10Blob* VsBlob;
ID3D10Blob* PsBlob;

void InitDirect3D(HINSTANCE hInstance)
{
	// SwapChain
	/// SwapChain.Buffer
	DXGI_MODE_DESC bufferDesc; // �ĸ������ �Ӽ����� �����ϴ� ����ü
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	bufferDesc.RefreshRate.Numerator = 60; // Refresh Rate : ȭ�� ��� ��
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// ���÷��� ��ĵ ���� ���
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	// � ����� �ؽ��ĸ� ����� ���ΰ�
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	/*
	UNORM : 0�� 1�� ������ ��. float���� �ٸ� ���� ��  8bit UNORM�̸�,
	�� ���� 127�϶� �̴� 0.5�� �νĵȴ�.
	*/

	/// SwapChain.Sampling.. etc
	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	// ���߹��۸��� ���� ���߹��۸��� ���� ������ ���ڸ� �ִ´�.
	swapDesc.BufferCount = 1;

	swapDesc.BufferDesc = bufferDesc;

	// ��� �����ΰ��� ����
	// DXGI_USAGE_RENDER_TARGET_OUTPUT : ǥ��, �ڿ��� ��� ���� Ÿ������ ���
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	swapDesc.OutputWindow = Hwnd;

	// �ȼ��� ������ ǥ���� ����
	// ��Ƽ ���ø��� ������� ������ count=1, ǰ������ 0���� ����
	swapDesc.SampleDesc.Count = 1;

	swapDesc.SampleDesc.Quality = 0; //���ϴ� ǰ�� ���� ����

	// ���۸� ���� ����ٰ�����
	// DXGI_SWAP_EFFECT_DISCARD : ���� �׷ȴ��� ���
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

	// HRESULT : �Լ��� ȣ�� ����� ��Ÿ���� 32��Ʈ ���� ǥ��
	HRESULT hr = D3D11CreateDeviceAndSwapChain
	(
		NULL, // ����ī�� �������̽� ����, NULL : ���� �߰ߵ� ����̽�
		D3D_DRIVER_TYPE_HARDWARE,
		NULL, // �����Ͷ������� �����Ǿ� �ִ� DLL�� �ڵ��� ����.
		NULL, // ����� DX11�� API ���̾�
		featured_level.data(), // ��ó ���� �迭 ����, ������ 11_1 -> 9_1
		featured_level.size(), // ��ó ���� �迭 ����
		D3D11_SDK_VERSION, // ������� SDK ����
		&swapDesc, // swapChain ������(��缭)
		&SwapChain, // ������ swapChain �����͸� ���� ����
		&Device, // ������ Device �����͸� ���� ����
		NULL,
		&DeviceContext // ������ DeviceContext �����͸� ���� ����
	);
	assert(SUCCEEDED(hr));

	// BackBuffer
	ID3D11Texture2D* BackBuffer;

	// __uuidof(ID3D11Texture2D) : ������Ʈ�� ���� �ĺ��ڸ� �����´�?
	// Getbuffer : ����ü���� 0��° ���۸� ��´�.
	hr = SwapChain->GetBuffer
	(
		0,
		__uuidof(ID3D11Texture2D),
		(void**)&BackBuffer
	);
	assert(SUCCEEDED(hr));

	// Create RTV 
	// BackBuffer�� ������, ������ �� ���� ���� ����ü ( NULL : �⺻ ��)
	// , ������ ���� �����͸� ��ȯ
	hr = Device->CreateRenderTargetView(BackBuffer, NULL, &RTV);
	assert(SUCCEEDED(hr));
	BackBuffer->Release();

	// OM : ����� ���� �ΰ����� ���(���ٽ�, ����)���� ��������
	// �ĸ� ���ۿ� ���, ���� ��
	DeviceContext->OMSetRenderTargets(1, &RTV, NULL);


	// Create Shader
	{
		/// Vertex Blob
		HRESULT hr;

		// D3DX11CompileFromFile : ���̴��� �������Ҷ� ����ϴ� �Լ�
		hr = D3DX11CompileFromFile
		(
			L"effect.fx",
			0, // Ư���� ��� ����0
			0, // Shader���� include ������ �ٷ궧 ���?
			"VS", // Shader File�� �Լ� ������
			"vs_4_0_level_9_1", // ����� Shader Version
			0, // Shader Code�� ������ ���, 0 : ����ȭ ����
			0, // Ư���� ��� ����, 0
			0, // Shader�� �񵿱������� ������ �ϱ����� �ɼ�
			&VsBlob, // �����ϵ� Shader�� ��� ����ü ����Ű�� ������
			0, // Error Message�� ���� ����ü ����Ű�� ������
			0  // �񵿱� �����Ͻ� ���� �ڵ带 ��ȸ, 0 : �񵿱� ��� x
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

		// �����ϵ� ���̴��� ������ ���̴� ���� �����.
		hr = Device->CreateVertexShader
		(
			VsBlob->GetBufferPointer(), // �����ϵ� ���̴� ������
			VsBlob->GetBufferSize(), // �����ϵ� ���ؽ� ���̴� ũ��
			NULL,
			&VertexShader // ���ؽ� ���̴��� �ּ�
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
		// �÷��̾��� �þ�, �ȼ�ȭ ���Ѽ� ������?
		// ���� Ÿ�ٿ� �������� �ϱ� ���ؼ��� ����Ʈ�� ������ �ʿ�
		// (ViewPort == ����Ÿ���� ������ ������ ���� ����)
		// ���� Ÿ���� 8���� ����Ʈ ���� 8�� ����

		/*
		���ؽ� ���̴�)'���� ��µǴ� ������ ��ġ��ǥ(3D)��
		�ֻ纯ȯ�� �Ϸ�� �ֻ纯ȯ ��ǥ���� ��. (-1<=x<=1, -1<=y<=1, 0<=z<=1)

		�����Ͷ�����(RS)�� ����Ʈ ��ȯ�� ���ؼ�,
		�� 3D ��ǥ ��ġ�κ��� 2D ��ũ�� ��ǥ(0<=x<=1, 0<=y<=1)�� ���

		��ũ�� ��ǥ : ���� ��(0,0), ������ �Ʒ�(1,1)
		*/

		D3D11_VIEWPORT viewPort;
		ZeroMemory(&viewPort, sizeof(D3D11_VIEWPORT));
		viewPort.TopLeftX = 0; // ����Ʈ ������ �»� X ��ǥ
		viewPort.TopLeftY = 0; // ����Ʈ ������ �»� Y ��ǥ
		viewPort.Width = Width; // ����Ʈ ������ ���� (������ �ϴ� X)
		viewPort.Height = Height; // ����Ʈ ������ ���� (������ �ϴ� Y)
		viewPort.MaxDepth = 0.0f; // ����Ʈ ������ ���̰� �ִ밪
		viewPort.MinDepth = 0.0f; // ����Ʈ ������ ���̰� �ּҰ�

		// ����Ʈ �����ϴ� �Լ�
		// (����Ÿ�� ����, ����Ʈ�� ������ ����Ʈ ����ü �迭)
		DeviceContext->RSSetViewports(1, &viewPort);
	}
}

void Destroy() // �������� �����Ѱ��� ���� Release() ���ش�.
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