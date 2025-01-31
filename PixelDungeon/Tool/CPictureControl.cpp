#include "pch.h"
#include "CPictureControl.h"
#include "CItemTool.h"

void CPictureControl::OnDropFiles(HDROP hDropInfo)
{
	UINT fileCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	TCHAR szFullPath[MAX_PATH];
	CString strRelativePath;

	for (UINT i = 0; i < fileCount; ++i)
	{
		DragQueryFile(hDropInfo, i, szFullPath, MAX_PATH);

		// 현재 작업 디렉토리 가져오기
		TCHAR szCurrentDir[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, szCurrentDir);

		TCHAR szRelativePath[MAX_PATH];
		// 절대 경로 -> 상대 경로 변환
		if (PathRelativePathTo(szRelativePath, szCurrentDir, FILE_ATTRIBUTE_DIRECTORY, szFullPath, FILE_ATTRIBUTE_NORMAL)) 
		{
			_tprintf(_T("Relative Path: %s\n"), szRelativePath);
			strRelativePath = szRelativePath;
		}
		else 
		{
			_tprintf(_T("Failed to calculate relative path.\n"));
		}

		// 픽쳐 컨트롤에 적용
		if (GetSafeHwnd())
		{
			// 예: 이미지 로드 함수 호출

			CImage* pImage = new CImage;
			HRESULT hr = pImage->Load(szFullPath);
			if (FAILED(hr))
			{
				AfxMessageBox(_T("이미지 로드 실패"));
				Safe_Delete(pImage);
				return;
			}

			PassImageToParent(pImage, strRelativePath);

			CRect rect;
			GetClientRect(&rect);

			CClientDC dc(this);
			pImage->Draw(dc.m_hDC, rect);

		}
		else
		{
			AfxMessageBox(_T("픽쳐 컨트롤 핸들이 유효하지 않습니다."));
		}
	}

	DragFinish(hDropInfo);
}

void CPictureControl::PassImageToParent(CImage* pImage, CString strRelativePath)
{
	CItemTool* pItemTool = NULL;
	pItemTool = dynamic_cast<CItemTool*>(GetParent());

	pItemTool->Set_Image(pImage);
	pItemTool->Set_Relative(strRelativePath);
}

BEGIN_MESSAGE_MAP(CPictureControl, CStatic)
    ON_WM_DROPFILES()
END_MESSAGE_MAP()