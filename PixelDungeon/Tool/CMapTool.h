#pragma once
#include "afxdialogex.h"
#include "Include.h"

// CMapTool 대화 상자

class CMapTool : public CDialog
{
	DECLARE_DYNAMIC(CMapTool)

public:
	CMapTool(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CMapTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CMapTool };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnListBox();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnDestroy();

public:
	virtual BOOL OnInitDialog();
public:
	void	Horizontal_Scroll();
	CListBox					m_ListBox;
	map<CString, CImage*>		m_mapPngImage;
	CStatic						m_Picture;

private:
	void CMapTool::Load_FileData(const CString& strFilePath);
	//CToolView* GetToolView()
	//{
	//	// 이 예제는 CToolView가 CMainFrame 내에 있는 경우에 해당
	//	CToolView* pToolView = NULL;
	//	CWnd* pWnd = GetDescendantWindow(ID_VIEW_TOOLVIEW); // ID_VIEW_TOOLVIEW는 CToolView의 ID입니다.

	//	if (pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CToolView)))
	//	{
	//		pToolView = (CToolView*)pWnd;
	//	}

	//	return pToolView;
	//}
};
