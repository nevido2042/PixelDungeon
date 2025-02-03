#pragma once

#include "CSingleTexture.h"
#include "CMultiTexture.h"

class CTextureMgr
{
	DECLARE_SINGLETON(CTextureMgr)

private:
	CTextureMgr();
	~CTextureMgr();

public:
	const TEXINFO* Get_Texture(const TCHAR* pObjKey,
							   const TCHAR* pStateKey = L"",
							   const int& iCnt = 0);

public:
	HRESULT		Insert_Texture(const TCHAR* pFilePath,
								TEXTYPE eTexture,
								const TCHAR* pObjKey,
								const TCHAR* pStateKey = L"",
								const int& iCnt = 0);

	int Get_TextureCount(const TCHAR* pObjKey, const TCHAR* pStateKey);
	
	
	 void		Release();


private:
	map<wstring, CTexture*>		m_mapTex;

};

