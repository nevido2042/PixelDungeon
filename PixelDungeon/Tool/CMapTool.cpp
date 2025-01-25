// CMapTool.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "afxdialogex.h"
#include "CMapTool.h"
#include "CFileInfo.h"
#include "MainFrm.h"
#include "CTerrain.h"

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
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_BTN_SAVE_MAP, &CMapTool::OnBnClickedBtnSaveMap)
END_MESSAGE_MAP()


// CMapTool 메시지 처리기


void CMapTool::OnListBox()
{
	UpdateData(TRUE);

	CString	strFindName;

	// GetCurSel : 커서가 선택한 셀의 인덱스 값을 반환
	int	iIndex = m_ListBox.GetCurSel();
	if (iIndex == -1)
	{
		return; //아무것도 클릭 안했을 시 리턴
	}

	/*
		1. 박스 리스트 클릭
		2. 선택 한 셀의 이름 툴뷰로 전달
		3. 전달된 이름에 따라 DrawID 변경

		1. 박스 리스트 클릭
		2. 선택 한 셀의 이름으로 m_mapPngImage에 저장된 정보(?)를 툴뷰로 전달
		3. 전달된 정보(?)에 따라 DrawID 변경

		1. 박스 리스트 클릭
		2. 선택 한 셀의 파일 이름 숫자로 툴뷰로 전달
		3. 전달된 숫자에 따라 DrawID 변경
	*/

	//임시방편으로 박스리스트 인덱스 값으로 선택 타일 변경
	//Get_ToolView()->Set_DrawID(iIndex);

	m_ListBox.GetText(iIndex, strFindName);

	auto	iter = m_mapPngImage.find(strFindName);

	if (iter == m_mapPngImage.end())
		return;

	// 첫 번째, 두 번째, 세 번째 문자 추출
	TCHAR firstChar = strFindName.GetAt(0);
	TCHAR secondChar = strFindName.GetAt(1);
	TCHAR thirdChar = strFindName.GetAt(2);
	int combinedIndex = -1;

	// 세 자리 수일 경우
	if (_istdigit(firstChar) && _istdigit(secondChar) && _istdigit(thirdChar))
	{
		combinedIndex = (firstChar - _T('0')) * 100 + (secondChar - _T('0')) * 10 + (thirdChar - _T('0'));
	}
	// 두 자리 수일 경우: 첫 번째와 두 번째 문자를 사용
	else if (_istdigit(firstChar) && _istdigit(secondChar))
	{
		combinedIndex = (firstChar - _T('0')) * 10 + (secondChar - _T('0'));
	}
	// 한 자리 수일 경우: 첫 번째 문자만 사용
	else if (_istdigit(firstChar))
	{
		combinedIndex = firstChar - _T('0');
	}
	else
	{
		AfxMessageBox(_T("fileN (N = 정수 세 자리 까지 가능) 형식을 지켜라"));
	}

	// 툴뷰의 드로우 아이디 변경
	if (combinedIndex != -1)
	{
		Get_ToolView()->Set_DrawID(combinedIndex);
	}


	m_Picture.SetBitmap(*(iter->second.pImage));

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

	for (int i = 0; i < iFileCnt; ++i)
	{
		DragQueryFile(hDropInfo, i, szFilePath, MAX_PATH);

		// 상대 경로
		CString strRelative = CFileInfo::Convert_RelativePath(szFilePath);

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

			IMAGE_INFO tImgInfo{ pPngImage , strRelative };

			m_mapPngImage.insert({ strFileName, tImgInfo });
			m_ListBox.AddString(szFileName);
		}
	}

	//추가한 파일경로를 저장한다.
	Save_Tile();

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
			(MyPair.second).pImage->Destroy();
			Safe_Delete((MyPair.second).pImage);
		});

	m_mapPngImage.clear();
}


BOOL CMapTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	//Setting_ListBox();

	Load_FileData(L"../Save/Tiles/Tiles.txt");

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	Horizontal_Scroll();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMapTool::Load_FileData(const CString& strFilePath)
{
	CStdioFile File;
	if (!File.Open(strFilePath, CFile::modeRead | CFile::typeText))
	{
		AfxMessageBox(_T("저장된 타일이 없다, 드래그앤 드랍으로 넣어라, 파일 이름 형식 맞춰서"));
		return;
	}

	TCHAR	szFileName[MAX_STR] = L"";

	CString strRelativePath;
	while (File.ReadString(strRelativePath)) // 파일에서 한 줄씩 읽기
	{
		// 읽어온 데이터를 리스트 박스에 추가
		if (strRelativePath.IsEmpty()) // 빈 줄 방지
		{
			break;
		}

		// PathFindFileName : 경로 중 파일 이름만 추출
		CString strFileName = PathFindFileName(strRelativePath);

		lstrcpy(szFileName, strFileName.GetString());

		// 확장자 명을 제거하는 함수
		PathRemoveExtension(szFileName);

		strFileName = szFileName;

		auto	iter = m_mapPngImage.find(strFileName);

		if (iter == m_mapPngImage.end())
		{
			CImage* pPngImage = new CImage;
			pPngImage->Load(strRelativePath);

			IMAGE_INFO tImgInfo{ pPngImage, strRelativePath };

			m_mapPngImage.insert({ strFileName, tImgInfo });
			m_ListBox.AddString(strFileName);
		}
	}

	File.Close();
}

CToolView* CMapTool::Get_ToolView()
{
	CToolView* pToolView = NULL;
	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(GetParentFrame());
	pToolView = dynamic_cast<CToolView*>(pMainFrm->m_MainSplitter.GetPane(0, 1));

	return pToolView;
}

void CMapTool::Save_Tile()
{
	// 파일 열기 (쓰기 모드, 새 파일 생성)
	CStdioFile File;
	if (!File.Open(L"../Save/Tiles/Tiles.txt", CFile::modeCreate | CFile::modeWrite | CFile::typeText))
	{
		AfxMessageBox(_T("저장 파일 생성 불가."));
	}

	// 파일 저장
	for (auto iter = m_mapPngImage.begin(); iter != m_mapPngImage.end(); ++iter)
	{
		CString strRelative = (*iter).second.strRelative;
		File.WriteString(strRelative + _T("\n"));
	}

	// 파일 닫기
	File.Close();
}

void CMapTool::Setting_ListBox()
{
	//m_ListBox.AddString()
	// 상대 경로와 필터 설정
	CString relativeFolderPath = _T("../Resources/Tile/Use");  // 상대 경로
	CString fileFilter = _T("*.png");  // .png 파일만 필터링

	// AddFilesToListBox 호출
	Add_FilesToListBox(relativeFolderPath, fileFilter);
}

void CMapTool::Add_FilesToListBox(const CString& relativeFolderPath, const CString& fileFilter)
{
	WIN32_FIND_DATA findData;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	// 현재 작업 디렉토리 가져오기
	TCHAR currentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currentDir);

	// 현재 디렉토리와 상대 경로를 결합하여 절대 경로를 만듬
	CString fullPath = CString(currentDir) + _T("\\") + relativeFolderPath;

	// 경로와 필터 결합 (예: "C:\\MyFolder\\*.png")
	CString searchPath = fullPath + _T("\\") + fileFilter;

	hFind = FindFirstFile(searchPath, &findData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("파일을 찾을 수 없습니다."), MB_OK | MB_ICONERROR);
		return;
	}

	do
	{
		// 디렉토리는 제외하고 파일만 추가
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			// 파일 이름 추가
			m_ListBox.AddString(findData.cFileName);

			// 상대경로 생성
			CString strRelative = relativeFolderPath + _T("\\") + findData.cFileName;

			// 이미지 로드
			CImage* pPngImage = new CImage;
			if (pPngImage->Load(strRelative) != S_OK)  // 이미지 로드 오류 체크
			{
				delete pPngImage;  // 실패 시 메모리 해제
				continue;
			}

			// 이미지 정보 구조체
			IMAGE_INFO tImgInfo{ pPngImage, strRelative };

			// m_mapPngImage에 상대경로와 이미지 객체 추가
			m_mapPngImage.insert({ findData.cFileName, tImgInfo });
		}
	} while (FindNextFile(hFind, &findData) != 0);

	FindClose(hFind); // 검색 종료
}


BOOL CMapTool::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE) {  // Delete 키 확인
		if (GetFocus() == &m_ListBox) {  // 리스트 박스가 포커스를 가지고 있는지 확인
			int iIndex = m_ListBox.GetCurSel();  // 선택된 항목 가져오기
			if (iIndex != LB_ERR) {  // 유효한 항목인지 확인

				CString strFindName;
				m_ListBox.GetText(iIndex, strFindName);

				auto iter = m_mapPngImage.find(strFindName);
				if (iter != m_mapPngImage.end())
				{
					Safe_Delete((*iter).second.pImage);
					m_mapPngImage.erase(iter);
					Save_Tile();
				}


				m_ListBox.DeleteString(iIndex);  // 선택된 항목 삭제

			}
			return TRUE;  // 메시지 처리 완료
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CMapTool::OnBnClickedBtnSaveMap()
{
	CTerrain* pTerrain = Get_ToolView()->m_pTerrain;

	pTerrain->Save_Tile();
}
