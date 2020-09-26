#include "stdafx.h"
#include "Device.h"

ID3D11Buffer* vertexBuffer;

/*
inputlayout ������Ʈ : � VertexBuffer�� ��� �����Ͱ� ���ԵǾ� �ְ�,
Shader�� � ������ �ѱ� ���ΰ��� IA���� �˷��ֱ� ���� �ʿ���
*/

ID3D11InputLayout* inputLayout; // �������� .fx�� �����ϴ� ����, IA
D3D11_INPUT_ELEMENT_DESC layoutDesc[] = // inputLayout�� DESC
{
	{
		"POSITION", // .fx�� POSITION (�ø�ƽ �̸�)
		0, // .fx�� POSITION �ڿ��ٴ� �ε�����ȣ 0�� (�ø�ƽ �ε���)
		DXGI_FORMAT_R32G32B32_FLOAT, // ���� ����
		0, // �Է½����� �ε��� ��
		0, // VertexData�� ó������ ��ұ����� ������
		D3D11_INPUT_PER_VERTEX_DATA, // ������ ����?
		0 // �ν��Ͻ� ��ȣ
	},
	{
		// �÷��߰�
		"COLOR",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		12, // 0���� 11������ ���� Vertex ������ 12����
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
	// Direct ��ǥ�� ȭ�� ����� (0, 0)
	// ���� �ִ� -1 ������ �ִ� 1 ���� �ִ� 1 �Ʒ��� �ִ� -1

	// ������ ��ġ ����
	Vertex vertices[4];

	vertices[0] = { D3DXVECTOR3(0.0f, -0.5f, 0.0f), D3DXCOLOR(1,0,0,1) };
	vertices[1] = { D3DXVECTOR3(-0.5f, -0.5f, 0.0f), D3DXCOLOR(1,0,0,1) };
	vertices[2] = { D3DXVECTOR3(-0.5f, 0.5f, 0.0f), D3DXCOLOR(1,0,0,1) };
	vertices[3] = { D3DXVECTOR3(0.5f, -0.5f, 0.0f), D3DXCOLOR(1,0,0,1) };


	// GPU�� float �ۿ� �𸥴�. �� ���۸� ���ļ� ��������
	// Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;//���ؽ��� ������� ���۷�
		desc.ByteWidth = sizeof(Vertex) * 4; // ���� ������ŭ
		desc.CPUAccessFlags = 0; // CPU ������ X
		desc.Usage = D3D11_USAGE_DEFAULT; // �⺻��

		D3D11_SUBRESOURCE_DATA data; // ���ۿ� ���� ������
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
			VsBlob->GetBufferPointer(), // ����ũ �ڵ��� ������
			VsBlob->GetBufferSize(), // ����Ʈ �ڵ��� ������
			&inputLayout // �Ѱ� ���� ���̾ƿ� ������
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
		// �׷��� �ڵ� �ۼ�
		UINT srtide = sizeof(Vertex);
		UINT offset = 0;

		// �޼� ��ǥ���̹Ƿ� �ﰢ���� ������ �ð�������� �׷����Ѵ�. �ƴϸ� �ȱ׷���
		DeviceContext->IASetVertexBuffers// ���ؽ� ���� ����
		(
			0, 1,
			&vertexBuffer, &srtide, &offset
		);

		DeviceContext->IASetInputLayout(inputLayout); // ���̾ƿ� ����

		// � ���·� ������ �Ұ��ΰ�.
		DeviceContext->IASetPrimitiveTopology
		(
			D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
		);

		DeviceContext->Draw(6, 0); // ��������, �迭�� ���� �ε��� ��
	}

	SwapChain->Present(0, 0);
}