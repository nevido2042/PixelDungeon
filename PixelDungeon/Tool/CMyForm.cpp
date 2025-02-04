// CMyForm.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "CMyForm.h"


// CMyForm

IMPLEMENT_DYNCREATE(CMyForm, CFormView)

CMyForm::CMyForm()
	: CFormView(IDD_CMyForm)
{

}

CMyForm::~CMyForm()
{
}

void CMyForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMyForm, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, &CMyForm::OnUnitTool)
	ON_BN_CLICKED(IDC_BUTTON7, &CMyForm::OnMapTool)
//	ON_BN_CLICKED(IDC_BUTTON13, &CMyForm::OnBnClickedButton13)
ON_BN_CLICKED(IDC_BUTTON13, &CMyForm::OnItemTool)
ON_BN_CLICKED(IDC_BUTTON2, &CMyForm::OnPathFinder)
END_MESSAGE_MAP()


// CMyForm 진단

#ifdef _DEBUG
void CMyForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMyForm 메시지 처리기


void CMyForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	m_Font.CreatePointFont(180, L"나눔체");

	GetDlgItem(IDC_BUTTON1)->SetFont(&m_Font);
	GetDlgItem(IDC_BUTTON7)->SetFont(&m_Font);
	GetDlgItem(IDC_BUTTON13)->SetFont(&m_Font);
}


void CMyForm::OnUnitTool()
{
	//GetSafeHwnd : 현재 다이얼로그 윈도우의 핸들을 반환
	if(nullptr == m_UnitTool.GetSafeHwnd())
		m_UnitTool.Create(IDD_CUnitTool);	// 해당 id에 맞는 다이얼로그 생성

	m_UnitTool.ShowWindow(SW_SHOW);
}


void CMyForm::OnMapTool()
{
	if (nullptr == m_MapTool.GetSafeHwnd())
		m_MapTool.Create(IDD_CMapTool);	// 해당 id에 맞는 다이얼로그 생성

	m_MapTool.ShowWindow(SW_SHOW);

}


void CMyForm::OnItemTool()
{
	if (nullptr == m_ItemTool.GetSafeHwnd())
		m_ItemTool.Create(IDD_CItemTool);	// 해당 id에 맞는 다이얼로그 생성

	m_ItemTool.ShowWindow(SW_SHOW);
}


void CMyForm::OnPathFinder()
{
	if (nullptr == m_PathFind.GetSafeHwnd())
		m_PathFind.Create(IDD_CPathFind);	// 해당 id에 맞는 다이얼로그 생성

	m_PathFind.ShowWindow(SW_SHOW);
}
