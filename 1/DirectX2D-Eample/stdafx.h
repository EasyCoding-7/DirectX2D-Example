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

// DirectX ���̺귯��
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

// DXGI : DirectX Graphic Infrastructure, �׷��Ƚ��� ������ �������� ����� ����, create, release �Լ��� ���, ��缭�� �ۼ�
extern IDXGISwapChain* SwapChain;	// �������

// pc �ڿ� ����, CPU�� RAM�� ����, ��� ���� ���˰� �ڿ� �Ҵ�
extern ID3D11Device* Device;

// ��ȭ��, �׷��� ī�� ����, �׷��ֱ� ���� �Ѱ� DC,
// ���� ��� ����, GPU�� ������ ������ ��� ����
extern ID3D11DeviceContext* DeviceContext;

// ���� ��ȭ��, WindowAPI�� HDC ����
extern ID3D11RenderTargetView* RTV;

extern ID3D11VertexShader* VertexShader;
extern ID3D11PixelShader* PixelShader;

// Blob : Binary large object
// ��Ƽ�̵�� �����͸� �ٷ궧 ���(����Ʈ �ڵ带 �������?)
extern ID3D10Blob* VsBlob;
extern ID3D10Blob* PsBlob;