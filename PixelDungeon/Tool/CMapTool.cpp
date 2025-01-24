﻿// CMapTool.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "afxdialogex.h"
#include "CMapTool.h"
#include "CFileInfo.h"

// CMapTool 대화 상자

IMPLEMENT_DYNAMIC(CMapTool, CDialog)

CMapTool::CMapTool(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CMapTool, pParent)
{

}

CMapTool::~CMapTool()
{
}

void CMapTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_PICTURE, m_Picture);
}


BEGIN_MESSAGE_MAP(CMapTool, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST1, &CMapTool::OnListBox)
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CMapTool 메시지 처리기


void CMapTool::OnListBox()
{
	UpdateData(TRUE);

	CString	strFindName;

	// GetCurSel : 커서가 선택한 셀의 인덱스 값을 반환
	int	iIndex = m_ListBox.GetCurSel();

	m_ListBox.GetText(iIndex, strFindName);

	auto	iter = m_mapPngImage.find(strFindName);

	if (iter == m_mapPngImage.end())
		return;

	m_Picture.SetBitmap(*(iter->second));

	UpdateData(FALSE);
}


void CMapTool::OnDropFiles(HDROP hDropInfo)
{
	UpdateData(TRUE);

	CDialog::OnDropFiles(hDropInfo);

	TCHAR	szFilePath[MAX_PATH] = L"";

	TCHAR	szFileName[MAX_STR] = L"";

	// DragQueryFile :  드롭된 파일으 정보를 얻어오는 함수
	// 
	// 두 번째 매개 변수 : 0xffffffff(-1)인 경우 드롭된 파일의 개수를 반환
	int	iFileCnt = DragQueryFile(hDropInfo, 0xffffffff, nullptr, 0);

	// 파일 열기 (쓰기 모드, 새 파일 생성)
	CStdioFile File;
	if (!File.Open(L"../Save/Tiles/Tiles.txt", CFile::modeCreate | CFile::modeWrite | CFile::typeText))
	{
		AfxMessageBox(_T("파일을 열 수 없습니다!"));
	}

	for (int i = 0; i < iFileCnt; ++i)
	{
		DragQueryFile(hDropInfo, i, szFilePath, MAX_PATH);

		// 상대 경로
		CString strRelative = CFileInfo::Convert_RelativePath(szFilePath);

		// 파일 저장
		// CString 데이터를 파일에 저장
		File.WriteString(strRelative + _T("\n"));

		// PathFindFileName : 경로 중 파일 이름만 추출
		CString strFileName = PathFindFileName(strRelative);

		lstrcpy(szFileName, strFileName.GetString());

		// 확장자 명을 제거하는 함수
		PathRemoveExtension(szFileName);

		strFileName = szFileName;

		auto	iter = m_mapPngImage.find(strFileName);

		if(iter == m_mapPngImage.end())
		{
			CImage* pPngImage = new CImage;
			pPngImage->Load(strRelative);

			m_mapPngImage.insert({ strFileName, pPngImage });
			m_ListBox.AddString(szFileName);

		}
	}

	// 파일 닫기
	File.Close();

	Horizontal_Scroll();

	UpdateData(FALSE);
}

void CMapTool::Horizontal_Scroll()
{
	CString		strName;

	CSize		Size;

	int		iWidth(0);

	CDC* pDC = m_ListBox.GetDC();

	for(int i = 0; i < m_ListBox.GetCount(); ++i)
	{
		m_ListBox.GetText(i, strName);

		// GetTextExtent : 매개 변수에 해당하는 문자열의 길이를 픽셀 단위로 변환
		Size = pDC->GetTextExtent(strName);

		if (Size.cx > iWidth)
			iWidth = Size.cx;
	}

	m_ListBox.ReleaseDC(pDC);

	// GetHorizontalExtent : 리스트 박스가 가로로 스크롤 할 수 있는 최대 범위를 얻어오는 함수
	if (iWidth > m_ListBox.GetHorizontalExtent())
		m_ListBox.SetHorizontalExtent(iWidth);

}


void CMapTool::OnDestroy()
{
	CDialog::OnDestroy();

	for_each(m_mapPngImage.begin(), m_mapPngImage.end(), [](auto& MyPair)
		{
			MyPair.second->Destroy();
			Safe_Delete(MyPair.second);
		});

	m_mapPngImage.clear();
}


BOOL CMapTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	Load_FileData(L"../Save/Tiles/Tiles.txt");
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	Horizontal_Scroll();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMapTool::Load_FileData(const CString& strFilePath)
{
	CStdioFile file;
	if (!file.Open(strFilePath, CFile::modeRead | CFile::typeText))
	{
		AfxMessageBox(_T("파일을 열 수 없습니다."));
		return;
	}

	TCHAR	szFileName[MAX_STR] = L"";

	CString strLine;
	while (file.ReadString(strLine)) // 파일에서 한 줄씩 읽기
	{
		// 읽어온 데이터를 리스트 박스에 추가
		if (!strLine.IsEmpty()) // 빈 줄 방지
		{
			m_ListBox.AddString(strLine);
		}

		// PathFindFileName : 경로 중 파일 이름만 추출
		CString strFileName = PathFindFileName(strLine);

		lstrcpy(szFileName, strFileName.GetString());

		// 확장자 명을 제거하는 함수
		PathRemoveExtension(szFileName);

		strFileName = szFileName;

		auto	iter = m_mapPngImage.find(strFileName);

		if (iter == m_mapPngImage.end())
		{
			CImage* pPngImage = new CImage;
			pPngImage->Load(strLine);

			m_mapPngImage.insert({ strFileName, pPngImage });
			//m_ListBox.AddString(strFileName);

		}
	}

	file.Close();
}
