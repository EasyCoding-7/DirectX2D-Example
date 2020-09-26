#include "stdafx.h"
#include "Device.h"

ID3D11Buffer* vertexBuffer;

/*
inputlayout 오브젝트 : 어떤 VertexBuffer의 어느 데이터가 포함되어 있고,
Shader에 어떤 식으로 넘길 것인가를 IA에게 알려주기 위해 필요함
*/

ID3D11InputLayout* inputLayout; // 정점들을 .fx로 전달하는 역할, IA
D3D11_INPUT_ELEMENT_DESC layoutDesc[] = // inputLayout의 DESC
{
	{
		"POSITION", // .fx의 POSITION (시멘틱 이름)
		0, // .fx의 POSITION 뒤에붙는 인덱스번호 0번 (시멘틱 인덱스)
		DXGI_FORMAT_R32G32B32_FLOAT, // 포맷 형식
		0, // 입력슬롯의 인덱스 값
		0, // VertexData의 처음에서 요소까지의 오프셋
		D3D11_INPUT_PER_VERTEX_DATA, // 데이터 종류?
		0 // 인스턴스 번호
	},
	{
		// 컬러추가
		"COLOR",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		12, // 0부터 11까지는 위의 Vertex 정보라서 12부터
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	}
};

struct Vertex
{
	D3DXVECTOR3 Position;
	D3DXCOLOR Color;
};


void InitScene()
{
	// Direct 좌표는 화면 가운데가 (0, 0)
	// 왼쪽 최대 -1 오른쪽 최대 1 위로 최대 1 아래로 최대 -1

	// 정점의 위치 세팅
	Vertex vertices[4];

	vertices[0] = { D3DXVECTOR3(0.0f, -0.5f, 0.0f), D3DXCOLOR(1,0,0,1) };
	vertices[1] = { D3DXVECTOR3(-0.5f, -0.5f, 0.0f), D3DXCOLOR(1,0,0,1) };
	vertices[2] = { D3DXVECTOR3(-0.5f, 0.5f, 0.0f), D3DXCOLOR(1,0,0,1) };
	vertices[3] = { D3DXVECTOR3(0.5f, -0.5f, 0.0f), D3DXCOLOR(1,0,0,1) };


	// GPU는 float 밖에 모른다. 꼭 버퍼를 거쳐서 보내야함
	// Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;//버텍스를 담기위한 버퍼로
		desc.ByteWidth = sizeof(Vertex) * 4; // 정점 개수만큼
		desc.CPUAccessFlags = 0; // CPU 엑세스 X
		desc.Usage = D3D11_USAGE_DEFAULT; // 기본값

		D3D11_SUBRESOURCE_DATA data; // 버퍼에 넣을 데이터
		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = vertices;

		HRESULT hr = Device->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	// Create InputLayout
	{
		HRESULT hr = Device->CreateInputLayout
		(
			layoutDesc,
			ARRAYSIZE(layoutDesc),
			VsBlob->GetBufferPointer(), // 바이크 코드의 포인터
			VsBlob->GetBufferSize(), // 바이트 코드의 사이즈
			&inputLayout // 넘겨 받을 레이아웃 포인터
		);
		assert(SUCCEEDED(hr));
	}
}

void DestroyScene()
{
	inputLayout->Release();
	vertexBuffer->Release();
}

void Update()
{

}

void Render()
{
	// D3DXCOLOR
	D3DXCOLOR bgColor = D3DXCOLOR(1, 1, 1, 1);
	DeviceContext->ClearRenderTargetView(RTV, bgColor);

	{
		// 그려줄 코드 작성
		UINT srtide = sizeof(Vertex);
		UINT offset = 0;

		// 왼손 좌표계이므로 삼각형은 무조건 시계방향으로 그려야한다. 아니면 안그려짐
		DeviceContext->IASetVertexBuffers// 버텍스 버퍼 설정
		(
			0, 1,
			&vertexBuffer, &srtide, &offset
		);

		DeviceContext->IASetInputLayout(inputLayout); // 레이아웃 설정

		// 어떤 형태로 렌더링 할것인가.
		DeviceContext->IASetPrimitiveTopology
		(
			D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
		);

		DeviceContext->Draw(6, 0); // 정점개수, 배열의 시작 인덱스 값
	}

	SwapChain->Present(0, 0);
}