#pragma once

// Window
#include <Windows.h>
#include <assert.h>

// STL
#include <string>
#include <vector>
#include <map>
#include <functional>

using namespace std;

// DirectX 라이브러리
#include <d3d11.h>
#include <D3DX11.h>
#include <D3DX10.h>
#include <D3DX10math.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

// FrameWork

// Extern Global
extern UINT Width;
extern UINT Height;

extern HWND Hwnd;
extern wstring Title;

// DXGI : DirectX Graphic Infrastructure, 그래픽스에 관련한 기초적인 기능을 제공, create, release 함수를 사용, 사양서를 작성
extern IDXGISwapChain* SwapChain;	// 더블버퍼

// pc 자원 생성, CPU와 RAM의 영역, 기능 지원 점검과 자원 할당
extern ID3D11Device* Device;

// 도화지, 그래픽 카드 영역, 그려주기 위한 총괄 DC,
// 렌더 대상 설정, GPU가 수행할 랜더링 명령 지시
extern ID3D11DeviceContext* DeviceContext;

// 실제 도화지, WindowAPI의 HDC 역할
extern ID3D11RenderTargetView* RTV;

extern ID3D11VertexShader* VertexShader;
extern ID3D11PixelShader* PixelShader;

// Blob : Binary large object
// 멀티미디어 데이터를 다룰때 사용(바이트 코드를 담기위함?)
extern ID3D10Blob* VsBlob;
extern ID3D10Blob* PsBlob;