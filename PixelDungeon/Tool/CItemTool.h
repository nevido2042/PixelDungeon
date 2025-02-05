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
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
public:
	virtual BOOL OnInitDialog();
private:
	CPictureControl				m_Picture;
	map<CString, CImage*>		m_mapPngImage;
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
	// 아이템 리스트 박스
	CListBox m_ListBox;
	// //아이템 이름
	CEdit m_ItemName;
	// // 아이템 설명란
	CEdit m_ItemDescription;

public:
	afx_msg void OnEnChangeItemName();
	afx_msg void OnEnChangeItemDescription();
	afx_msg void OnBnClickedAddItem();
	afx_msg void OnLbnSelchangeItemList();
	afx_msg void OnBnClickedItemSave();
//	afx_msg void OnStnClickedItemImg();
};
