#include "pch.h"
#include "CSingleTexture.h"
#include "CDevice.h"

CSingleTexture::CSingleTexture()
    : m_pTexInfo(nullptr)
{
}

CSingleTexture::~CSingleTexture()
{
    Release();
}

HRESULT CSingleTexture::Insert_Texture(const TCHAR* pFilePath, const TCHAR* pStateKey, const int& iCnt)
{
    m_pTexInfo = new TEXINFO;
    ZeroMemory(m_pTexInfo, sizeof(TEXINFO));

    // D3DXGetImageInfoFromFile : ������ �̹��� ���Ͽ� ���� ������ ���� ����ü�� ����ϴ� �Լ�
    if (FAILED(D3DXGetImageInfoFromFile(pFilePath, &(m_pTexInfo->tImgInfo))))
    {
        Safe_Delete(m_pTexInfo);
        AfxMessageBox(L"D3DXGetImageInfoFromFile Failed");
        return E_FAIL;
    }

    if (FAILED(D3DXCreateTextureFromFileEx(CDevice::Get_Instance()->Get_Device(),
        pFilePath,
        m_pTexInfo->tImgInfo.Width,
        m_pTexInfo->tImgInfo.Height,
        m_pTexInfo->tImgInfo.MipLevels,
        0,  // USAGE : �ؽ�ó�� ��� ���(0 �Ϲ����� �ؽ�ó, D3DUSAGE_RENDERTARGET : ȭ�� �׸��� �ؽ�ó)
        m_pTexInfo->tImgInfo.Format,
        D3DPOOL_MANAGED,
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        0,              // 0�� �־��ָ� �÷�Ű ��ȿȭ
        nullptr,        // �̹��� ���� ���
        nullptr,        // �ȷ�Ʈ �̹����� ���� ó��
        &(m_pTexInfo->pTexture))))
    {
        Safe_Delete(m_pTexInfo);
        AfxMessageBox(L"D3DXCreateTextureFromFileEx Failed");
        return E_FAIL;
    }

    return S_OK;
}

void CSingleTexture::Release()
{
    Safe_Release(m_pTexInfo->pTexture);        
    Safe_Delete(m_pTexInfo);
}


