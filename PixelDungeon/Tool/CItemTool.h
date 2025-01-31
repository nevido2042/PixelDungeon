#pragma once
#include "afxdialogex.h"
#include "Include.h"
#include "CPictureControl.h"

// CItemTool 대화 상자

class CItemTool : public CDialog
{
	DECLARE_DYNAMIC(CItemTool)

public:
	CItemTool(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CItemTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CItemTool };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
public:
	virtual BOOL OnInitDialog();
private:
	CPictureControl				m_Picture;
	map<CString, IMAGE_INFO>	m_mapPngImage;
	map<CString, ITEM_INFO>		m_mapItemInfo;
public:
	void Set_Image(CImage* pImage) { m_pImage = pImage; }
	void Set_Relative(CString strRelative) { m_strRelative = strRelative; }
private:
	// //아이템 이름
	CString m_strItemName;
	// 아이템 설명
	CString m_strItemDescription;
	//	이미지 상대경로
	CString	m_strRelative;
	//	이미지
	CImage* m_pImage;
public:
	afx_msg void OnEnChangeItemName();
	afx_msg void OnEnChangeItemDescription();
	afx_msg void OnBnClickedAddItem();
	// 아이템 리스트 박스
	CListBox m_ListBox;
};
