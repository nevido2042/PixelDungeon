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
	afx_msg void OnListBox();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnSaveMap();

public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	void	Horizontal_Scroll();
	CListBox					m_ListBox;
	map<CString, IMAGE_INFO>	m_mapPngImage;
	CStatic						m_Picture;

private:
	void		Load_FileData(const CString& strFilePath);
	CToolView*	Get_ToolView();
	void		Save_Tile();
	void		Setting_ListBox();
	// 특정 경로에서 파일 이름을 리스트 박스에 추가하는 함수
	void		Add_FilesToListBox(const CString& relativeFolderPath, const CString& fileFilter);
};
