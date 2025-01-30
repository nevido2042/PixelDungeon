#include "pch.h"
#include "CPictureControl.h"

void CPictureControl::OnDropFiles(HDROP hDropInfo)
{
	UINT fileCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	TCHAR szFileName[MAX_PATH];

	for (UINT i = 0; i < fileCount; ++i)
	{
		DragQueryFile(hDropInfo, i, szFileName, MAX_PATH);

		// ����Ʈ �ڽ��� �߰�
		//m_ListBox.AddString(szFileName);

		// ���� ��Ʈ�ѿ� ����
		if (GetSafeHwnd())
		{
			// ��: �̹��� �ε� �Լ� ȣ��

			CImage image;
			HRESULT hr = image.Load(szFileName);
			if (FAILED(hr))
			{
				AfxMessageBox(_T("�̹��� �ε� ����"));
				return;
			}

			CRect rect;
			GetClientRect(&rect);

			CClientDC dc(this);
			image.Draw(dc.m_hDC, rect);

		}
		else
		{
			AfxMessageBox(_T("���� ��Ʈ�� �ڵ��� ��ȿ���� �ʽ��ϴ�."));
		}
	}

	DragFinish(hDropInfo);
}

BEGIN_MESSAGE_MAP(CPictureControl, CStatic)
    ON_WM_DROPFILES()
END_MESSAGE_MAP()