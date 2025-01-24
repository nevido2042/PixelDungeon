// CUnitTool.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "afxdialogex.h"
#include "CUnitTool.h"


// CUnitTool 대화 상자

IMPLEMENT_DYNAMIC(CUnitTool, CDialog)

CUnitTool::CUnitTool(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CUnitTool, pParent)
	, m_strTemp(_T(""))
	, m_strCopy(_T(""))
{

}

CUnitTool::~CUnitTool()
{
}

void CUnitTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strTemp);
	DDX_Text(pDX, IDC_EDIT2, m_strCopy);
}


BEGIN_MESSAGE_MAP(CUnitTool, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CUnitTool::OnBnClickedButton1)
END_MESSAGE_MAP()



void CUnitTool::OnBnClickedButton1()
{
	UpdateData(TRUE);		// 다이얼로그 박스로부터 입력된 값들을 얻어옴

	m_strCopy = m_strTemp;

	UpdateData(FALSE);		// 변수에 저장된 값으로 다이얼로그 박스에 반영
}
