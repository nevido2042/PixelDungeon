#include "pch.h"
#include "CDevice.h"

IMPLEMENT_SINGLETON(CDevice)

CDevice::CDevice() 
	: m_pDevice(nullptr), m_pSDK(nullptr), m_pSprite(nullptr), m_pFont(nullptr)
{
}

CDevice::~CDevice()
{
	Release();
}

HRESULT CDevice::Init_Device()
{
	// ��ġ �ʱ�ȭ ����
	// 1. ��ġ(�׷��� ī��)�� ������ ��ü ����
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);
	
	// 2. ������ ���� ��ü�� ��ġ�� ����(���� ����)
	// 
	// HAL : �ϵ���� �߻�ȭ ����

	D3DCAPS9	DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	// GetDeviceCaps : ��ġ ������ ������ �Լ�
	// D3DADAPTER_DEFAULT : ������ �������� �⺻ �׷��� ī��

	if (FAILED(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps)))
	{
		AfxMessageBox(L"GetDeviceCaps Failed");
		return E_FAIL;
	}

	DWORD		vp(0);		// ���ؽ� ���μ��� = ���� ��ȯ + ���� ����

	if (DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING;

	else
		vp |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DPRESENT_PARAMETERS		d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	Set_Parameters(d3dpp);

	// 3. ��ġ(�׷��� ī��)�� ������ ��ü ����

	// CreateDevice : ��ġ�� ������ ��ü ����
	// CreateDevice(� �׷��� ī�带 ������ ���ΰ�, � ������ ��ġ�� ������ ���ΰ�, ��ġ�� ����� ������ �ڵ�, ���� ���, ��� ȯ��, ������ ��ü�� ������ ������)
	if (FAILED(m_pSDK->CreateDevice(D3DADAPTER_DEFAULT,
							D3DDEVTYPE_HAL, g_hWnd, vp, &d3dpp, &m_pDevice)))
	{
		AfxMessageBox(L"CreateDevice Failed");
		return E_FAIL;
	}

	// sprite
	if (FAILED(D3DXCreateSprite(m_pDevice, &m_pSprite)))
	{
		AfxMessageBox(L"D3DXCreateSprite Failed");
		return E_FAIL;
	}

	//font

	D3DXFONT_DESCW		tFontInfo;
	ZeroMemory(&tFontInfo, sizeof(D3DXFONT_DESCW));

	tFontInfo.Height = 20;
	tFontInfo.Width = 10;
	tFontInfo.Weight = FW_HEAVY;
	tFontInfo.CharSet = HANGEUL_CHARSET;
	lstrcpy(tFontInfo.FaceName, L"�ü�");

	if (FAILED(D3DXCreateFontIndirect(m_pDevice, &tFontInfo, &m_pFont)))
	{
		AfxMessageBox(L"D3DXCreateFontIndirect Failed");
		return E_FAIL;
	}



	return S_OK;
}

// �ĸ� ������ ���� ����
// 1. �����
// 2. ���� ȭ�� �׸� ���� �غ��Ѵ�.
// 3. ���ǽ��� ��ȯ�Ѵ�.
void CDevice::Render_Begin()
{
	m_pDevice->Clear(0,			// ��Ʈ�� ����
					nullptr,	// ���� ��Ʈ �� ù ��° ��Ʈ�� �ּ�
					D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 
					D3DCOLOR_ARGB(255, 0, 0, 255), // �ĸ� ���۸� ���� ä�� ����
					1.f,	// z���� �ʱ�ȭ ��
					0);		// ���ٽ� ���� �ʱ�ȭ ��

	m_pDevice->BeginScene();

	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

}

void CDevice::Render_End(HWND hWnd)
{
	m_pSprite->End();

	m_pDevice->EndScene();

	m_pDevice->Present(NULL, NULL, hWnd, NULL);

	// 1, 2, 4 �� ���� ��� ���� : D3DSWAPEFFECY_COPY�� �ۼ����� ��� ����
	// 3���� : ��� ��� ������ �ڵ�, NULL�� ��쿡�� Set_Parameters���� ������ �ڵ� ������ ����
}

void CDevice::Release()
{
	Safe_Release(m_pFont);
	Safe_Release(m_pSprite);
	Safe_Release(m_pDevice);
	Safe_Release(m_pSDK);
}

void CDevice::Set_Parameters(D3DPRESENT_PARAMETERS& d3dpp)
{
	d3dpp.BackBufferWidth  = WINCX;
	d3dpp.BackBufferHeight = WINCY;

	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;

	// D3DSWAPEFFECT_DISCARD : ���� ü�� ���
	// D3DSWAPEFFECT_FLIP : ���� �ϳ��� �������鼭 ó���ϴ� ���
	// D3DSWAPEFFECT_COPY : ���� ���۸��� ������ ���� ���
	
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

	d3dpp.hDeviceWindow = g_hWnd;

	// ��ü ȭ�� ��� or â ���
	d3dpp.Windowed = TRUE;			// â ��� ����

	// �ڵ����� ����, ���ٽ� ���� ���� ����
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	
	// ��ü ȭ�� ����� ��� ����

	// ��ü ȭ�� �� ����� ������� ���� ����
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	// ������� ���� â �ÿ� ���� ����
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}
