#pragma once
#include "afxdialogex.h"
#include "Include.h"
#include "ToolView.h"

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
	afx_msg void OnBnClickedRadioPass();
	afx_msg void OnBnClickedRadioBlock();
	afx_msg void OnListBox();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnSaveMap();

public:
	virtual BOOL OnInitDialog();
public:
	void	Horizontal_Scroll();
	CListBox					m_ListBox;
	CStatic						m_Picture;

private:
	CToolView*	Get_ToolView();
};
