#include "pch.h"
#include "CPictureControl.h"

void CPictureControl::OnDropFiles(HDROP hDropInfo)
{
	UINT fileCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	TCHAR szFileName[MAX_PATH];

	for (UINT i = 0; i < fileCount; ++i)
	{
		DragQueryFile(hDropInfo, i, szFileName, MAX_PATH);

		// 리스트 박스에 추가
		//m_ListBox.AddString(szFileName);

		// 픽쳐 컨트롤에 적용
		if (GetSafeHwnd())
		{
			// 예: 이미지 로드 함수 호출

			CImage image;
			HRESULT hr = image.Load(szFileName);
			if (FAILED(hr))
			{
				AfxMessageBox(_T("이미지 로드 실패"));
				return;
			}

			CRect rect;
			GetClientRect(&rect);

			CClientDC dc(this);
			image.Draw(dc.m_hDC, rect);

		}
		else
		{
			AfxMessageBox(_T("픽쳐 컨트롤 핸들이 유효하지 않습니다."));
		}
	}

	DragFinish(hDropInfo);
}

BEGIN_MESSAGE_MAP(CPictureControl, CStatic)
    ON_WM_DROPFILES()
END_MESSAGE_MAP()