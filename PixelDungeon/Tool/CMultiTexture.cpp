#include "pch.h"
#include "CMultiTexture.h"
#include "CDevice.h"

CMultiTexture::CMultiTexture()  
{
}

CMultiTexture::~CMultiTexture()
{
    Release();
}

const TEXINFO* CMultiTexture::Get_Texture(const TCHAR* pStateKey, 
                                            const int& iCnt)
{
    auto        iter = find_if(m_MapMultiTex.begin(), m_MapMultiTex.end(), [&](auto& MyPair)->bool
        {
            if (pStateKey == MyPair.first)
                return true;

            return false;
        });

    if (iter == m_MapMultiTex.end())
        return nullptr;
  

    return iter->second[iCnt];
}

HRESULT CMultiTexture::Insert_Texture(const TCHAR* pFilePath, const TCHAR* pStateKey, const int& iCnt)
{

    // ��θ� �ǹ̴� ���̴� ���� 520byte�� �Ѿ�� �ȵȴ�.
    TCHAR       szFullPath[MAX_PATH] = L"";


    for (int i = 0; i < iCnt; ++i)
    {
        swprintf_s(szFullPath, pFilePath, i);

        TEXINFO* pTexInfo = new TEXINFO;
        ZeroMemory(pTexInfo, sizeof(TEXINFO));

        // D3DXGetImageInfoFromFile : ������ �̹��� ���Ͽ� ���� ������ ���� ����ü�� ����ϴ� �Լ�
        if (FAILED(D3DXGetImageInfoFromFile(szFullPath, &(pTexInfo->tImgInfo))))
        {
            Safe_Delete(pTexInfo);
            AfxMessageBox(szFullPath);
            return E_FAIL;
        }

        if (FAILED(D3DXCreateTextureFromFileEx(CDevice::Get_Instance()->Get_Device(),
            szFullPath,
            pTexInfo->tImgInfo.Width,
            pTexInfo->tImgInfo.Height,
            pTexInfo->tImgInfo.MipLevels,
            0,  // USAGE : �ؽ�ó�� ��� ���(0 �Ϲ����� �ؽ�ó, D3DUSAGE_RENDERTARGET : ȭ�� �׸��� �ؽ�ó)
            pTexInfo->tImgInfo.Format,
            D3DPOOL_MANAGED,
            D3DX_DEFAULT,
            D3DX_DEFAULT,
            0,              // 0�� �־��ָ� �÷�Ű ��ȿȭ
            nullptr,        // �̹��� ���� ���
            nullptr,        // �ȷ�Ʈ �̹����� ���� ó��
            &(pTexInfo->pTexture))))
        {
            Safe_Delete(pTexInfo);
            AfxMessageBox(L"D3DXCreateTextureFromFileEx Failed");
            return E_FAIL;
        }


        m_MapMultiTex[pStateKey].push_back(pTexInfo);
    }
    
    return S_OK;
}

void CMultiTexture::Release()
{
    for_each(m_MapMultiTex.begin(), m_MapMultiTex.end(), [](auto& MyPair)
        {
            for_each(MyPair.second.begin(), MyPair.second.end(), 
                Safe_Delete<TEXINFO*>);
            MyPair.second.clear();
        });

        m_MapMultiTex.clear();
}


