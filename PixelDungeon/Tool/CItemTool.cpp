// CItemTool.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "afxdialogex.h"
#include "CItemTool.h"
#include "Include.h"
#include "CFileInfo.h"


// CItemTool 대화 상자

IMPLEMENT_DYNAMIC(CItemTool, CDialog)

CItemTool::CItemTool(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CItemTool, pParent)
	, m_strItemName(_T(""))
	, m_strItemDescription(_T(""))
	, m_pImage(nullptr)
{

}

CItemTool::~CItemTool()
{
}

void CItemTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ITEM_IMG, m_Picture);
	m_Picture.DragAcceptFiles();
	DDX_Text(pDX, IDC_ITEM_NAME, m_strItemName);
	DDX_Text(pDX, IDC_ITEM_DESCRIPTION, m_strItemDescription);
	DDX_Control(pDX, IDC_ITEM_LIST, m_ListBox);
	DDX_Control(pDX, IDC_ITEM_NAME, m_ItemName);
	DDX_Control(pDX, IDC_ITEM_DESCRIPTION, m_ItemDescription);
	DDX_Control(pDX, IDC_LIST1, m_ImgListBox);
}

BOOL CItemTool::PreTranslateMessage(MSG* pMsg)
{
	//if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE) {  // Delete 키 확인
	//	if (GetFocus() == &m_ListBox) {  // 리스트 박스가 포커스를 가지고 있는지 확인
	//		int iIndex = m_ListBox.GetCurSel();  // 선택된 항목 가져오기
	//		if (iIndex != LB_ERR) {  // 유효한 항목인지 확인

	//			CString strFindName;
	//			m_ListBox.GetText(iIndex, strFindName);

	//			auto iter = m_mapPngImage.find(strFindName);
	//			if (iter != m_mapPngImage.end())
	//			{
	//				Safe_Delete((*iter).second.pImage);
	//				m_mapPngImage.erase(iter);
	//				Save_Tile();
	//			}


	//			m_ListBox.DeleteString(iIndex);  // 선택된 항목 삭제

	//		}
	//		return TRUE;  // 메시지 처리 완료
	//	}
	//}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CItemTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	//m_Picture.DragAcceptFiles();

	Load_Png_And_ListBox();
	Load_Item();
	// 드래그 앤 드랍 활성화
	//DragAcceptFiles(TRUE);

	return TRUE;
}

void CItemTool::Load_Png_And_ListBox()
{
	//Texture->Terrain->File로 가서 파일 모두의 이름을 리스트에 등록

	// CFileFind : mfc 에서 제공하는 파일 및 경로 제어 관련 클래스
	CFileFind       Find;

	TCHAR szTilePath[MAX_PATH]; // 현재 디렉토리를 저장할 버퍼

	// GetCurrentDirectory 함수 호출
	GetCurrentDirectory(MAX_PATH, szTilePath);
	//L"D:\\Fork_Git\\PixelDungeon\\PixelDungeon\\Tool"

	PathRemoveFileSpec(szTilePath);
	//L"D:\\Fork_Git\\PixelDungeon\\PixelDungeon

	lstrcat(szTilePath, L"\\Texture\\Item\\Item\\*.*");
	//L"D:\\Fork_Git\\PixelDungeon\\PixelDungeon\\Texture\\Terrain\\Tile\\*.*"
	// FindFile : 매개 변수로 전달된 경로에 파일의 유무를 확인하는 함수

	BOOL    bContinue = Find.FindFile(szTilePath);

	while (bContinue)
	{
		// FindNextFile : 동일 경로 안에서 다음 파일을 찾는 함수, 찾을 게 없을 경우 또는 마지막 대상을 찾았을 경우 0을 반환

		bContinue = Find.FindNextFile();

		if (Find.IsDots())
			continue;

		if (Find.IsSystem())
			continue;

		// 1. 파일의 이름만 얻어오자
		// GetFileTitle : 파일 이름만 얻어오는 함수
		wstring     wstrTextureName = Find.GetFileTitle().GetString();

		m_ImgListBox.AddString(wstrTextureName.c_str());
	}

	// png 저장
	PathRemoveFileSpec(szTilePath);

	// 검색할 디렉터리와 파일 패턴 지정
	CString strDirectory = szTilePath;
	CString strSearchPath = strDirectory + L"\\*.png";

	// CFileFind 객체를 사용하여 파일 탐색
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strSearchPath);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// 디렉터리인지 확인 (파일만 처리)
		if (finder.IsDots() || finder.IsDirectory())
			continue;

		// 전체 파일 경로 가져오기
		CString strFullPath = finder.GetFilePath();

		// 파일 이름만 추출
		CString strFileName = PathFindFileName(strFullPath);

		// 확장자 제거
		TCHAR szFileName[MAX_PATH] = { 0 };
		lstrcpy(szFileName, strFileName.GetString());
		PathRemoveExtension(szFileName);

		// 확장자가 제거된 파일 이름으로 map에 추가
		auto iter = m_mapPngImage.find(szFileName);
		if (iter == m_mapPngImage.end())
		{
			// 이미지 로드
			CImage* pPngImage = new CImage;
			if (SUCCEEDED(pPngImage->Load(strFullPath)))
			{
				// 맵에 삽입
				m_mapPngImage.insert({ szFileName, pPngImage });

				// ListBox에 추가
				//m_ListBox.AddString(szFileName);
			}
			else
			{
				// 이미지 로드 실패 시 메모리 해제
				delete pPngImage;
			}
		}
	}

}

void CItemTool::Load_Item()
{
	TCHAR szItamDataPath[MAX_PATH]; // 현재 디렉토리를 저장할 버퍼

	// GetCurrentDirectory 함수 호출
	GetCurrentDirectory(MAX_PATH, szItamDataPath);
	//L"D:\\Fork_Git\\PixelDungeon\\PixelDungeon\\Tool"

	PathRemoveFileSpec(szItamDataPath);
	//L"D:\\Fork_Git\\PixelDungeon\\PixelDungeon

	lstrcat(szItamDataPath, L"\\Save\\Item.dat");
	//L"D:\\Fork_Git\\PixelDungeon\\PixelDungeon\\Texture\\Terrain\\Tile\\*.*"
	// FindFile : 매개 변수로 전달된 경로에 파일의 유무를 확인하는 함수

	CStdioFile File;
	if (!File.Open(szItamDataPath, CFile::modeRead | CFile::typeBinary))
	{
		AfxMessageBox(_T("저장된 타일이 없다, 드래그앤 드랍으로 넣어라, 파일 이름 형식 맞춰서"));
		return;
	}

	while (true)
	{
		ITEM_INFO* pItemInfo = new ITEM_INFO;

		UINT iResult(0);
		iResult = ReadStringUntilNull(File, pItemInfo->strImageName);
		iResult = ReadStringUntilNull(File, pItemInfo->strName);
		iResult = ReadStringUntilNull(File, pItemInfo->strDescription);
		//iResult = File.ReadString(pItemInfo->strImageName);
		//iResult = File.ReadString(pItemInfo->strName);
		//iResult = File.ReadString(pItemInfo->strDescription);
		if (iResult == 0)
		{
			Safe_Delete(pItemInfo);
			File.Close();
			return;
		}

		m_mapItemInfo.emplace(pItemInfo->strName, pItemInfo);
		m_ListBox.AddString(pItemInfo->strName);
	}
	File.Close();
}

int CItemTool::ReadStringUntilNull(CFile& File, CString& strOut)
{
	WCHAR ch;
	CString tempStr;

	int iResult(0);

	while (File.Read(&ch, sizeof(WCHAR)) == sizeof(WCHAR)) { // WCHAR 단위로 읽기
		if (ch == L'\0') { // 널 문자를 만나면 종료
			break;
		}
		tempStr += ch;
		++iResult;
	}

	strOut = tempStr; // 읽은 문자열 저장
	return iResult;
}


BEGIN_MESSAGE_MAP(CItemTool, CDialog)
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
//	ON_STN_CLICKED(IDC_NAME2, &CItemTool::OnStnClickedName2)
ON_EN_CHANGE(IDC_ITEM_NAME, &CItemTool::OnEnChangeItemName)
ON_EN_CHANGE(IDC_ITEM_DESCRIPTION, &CItemTool::OnEnChangeItemDescription)
ON_BN_CLICKED(IDC_ADD_ITEM, &CItemTool::OnBnClickedAddItem)
ON_LBN_SELCHANGE(IDC_ITEM_LIST, &CItemTool::OnLbnSelchangeItemList)
ON_BN_CLICKED(IDC_ITEM_SAVE, &CItemTool::OnSave)
//ON_STN_CLICKED(IDC_ITEM_IMG, &CItemTool::OnStnClickedItemImg)
ON_LBN_SELCHANGE(IDC_LIST1, &CItemTool::OnImgListBox)
END_MESSAGE_MAP()
// CItemTool 메시지 처리기

void CItemTool::OnDestroy()
{
	CDialog::OnDestroy();

	for_each(m_mapPngImage.begin(), m_mapPngImage.end(), [](auto& MyPair)
		{
			MyPair.second->Destroy();
			Safe_Delete(MyPair.second);
		});

	m_mapPngImage.clear();

	for_each(m_mapItemInfo.begin(), m_mapItemInfo.end(), [](auto& MyPair)
		{
			Safe_Delete(MyPair.second);
		});

	m_mapPngImage.clear();

	//하나의 이미지로 여러아이템을 쓰면 터짐, 어케하지 (같은 이미지라도 다시 할당 받는 방법을 써야하나)
	//언오드셋으로 해결
	//std::unordered_set<void*> deletedPointers;

	//for_each(m_mapItemInfo.begin(), m_mapItemInfo.end(), [&](auto& MyPair)
	//	{
	//		IMAGE_INFO& tImgInfo = MyPair.second.tImgInfo;
	//		if (tImgInfo.pImage)
	//		{
	//			// 이미 해제된 포인터인지 확인
	//			if (deletedPointers.find(tImgInfo.pImage) == deletedPointers.end())
	//			{
	//				deletedPointers.insert(tImgInfo.pImage);
	//				tImgInfo.pImage->Destroy();
	//				Safe_Delete(tImgInfo.pImage); // 해제 후 nullptr
	//			}
	//			else
	//			{
	//				tImgInfo.pImage = nullptr; // 중복 참조된 포인터는 nullptr 설정
	//			}
	//		}
	//	});

	//m_mapItemInfo.clear();
}


void CItemTool::OnEnChangeItemName()
{
	// Edit Control에서 텍스트를 가져와 m_strItemName에 저장
	GetDlgItemText(IDC_ITEM_NAME, m_strItemName);

	// 디버그용으로 출력
	TRACE(_T("Item Name Changed: %s\n"), m_strItemName);
}


void CItemTool::OnEnChangeItemDescription()
{
	// Edit Control에서 텍스트를 가져와 m_strItemDescription에 저장
	GetDlgItemText(IDC_ITEM_DESCRIPTION, m_strItemDescription);

	// 디버그용으로 출력
	TRACE(_T("Item 설명 Changed: %s\n"), m_strItemDescription);
}


void CItemTool::OnBnClickedAddItem()
{
	if (m_strItemName.IsEmpty())
	{
		return;
	}

	ITEM_INFO* pItemInfo = new ITEM_INFO;
	pItemInfo->strImageName = m_strImageName;
	pItemInfo->strName = m_strItemName;
	pItemInfo->strDescription = m_strItemDescription;

	m_mapItemInfo.insert({ m_strItemName, pItemInfo });

	m_ListBox.AddString(m_strItemName);
}


void CItemTool::OnLbnSelchangeItemList()
{
	int iIndex = m_ListBox.GetCurSel();
	if (iIndex == LB_ERR)
		return;

	CString strFindName;
	m_ListBox.GetText(iIndex, strFindName);

	m_mapItemInfo.find(strFindName);

	auto iter = m_mapItemInfo.find(strFindName);
	if (iter == m_mapItemInfo.end())
		return;

	CString strImgName = iter->second->strImageName;
	auto iterImg = m_mapPngImage.find(strImgName);

	CImage* pImage = iterImg->second;

	if (!pImage)
		return;

	//픽쳐 디시를 가져온다.
	CClientDC dc(&m_Picture);
	//렉트
	CRect rect;
	//픽쳐의 렉트를 가져온다.
	m_Picture.GetClientRect(&rect);
	//픽쳐를 렉트만큼 배경(파랑)으로 채운다.
	dc.FillSolidRect(rect, RGB(255, 255, 255));
	//이미지를 픽쳐에 렉트만큼 그린다.
	pImage->Draw(dc, rect);

	// 이름과 설명 출력
	m_ItemName.SetWindowTextW(iter->first.GetString());
	m_ItemDescription.SetWindowTextW(iter->second->strDescription.GetString());
}


void CItemTool::OnSave()
{
	CFileDialog		Dlg(FALSE,		// TRUE(불러오기), FALSE(다른 이름으로 저장) 모드 지정
		L"dat",		// default 확장자명
		L"Item.dat",	// 대화 상자에 표시될 최초 파일명
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,	// 읽기 전용 체크 박스 숨김 | 중복된 이름으로 파일 저장 시 경고 메세지 띄움
		L"Data Files(*.dat) | *.dat ||", // 대화 상자에 표시될 파일 형식
		this);	// 부모 윈도우 주소

	TCHAR	szPath[MAX_PATH] = L"";

	GetCurrentDirectory(MAX_PATH, szPath);

	PathRemoveFileSpec(szPath);

	lstrcat(szPath, L"\\Save");

	Dlg.m_ofn.lpstrInitialDir = szPath;

	if (IDOK == Dlg.DoModal())
	{
		CString	str = Dlg.GetPathName().GetString();
		const TCHAR* pGetPath = str.GetString();

		HANDLE hFile = CreateFile(pGetPath,
			GENERIC_WRITE,
			0, 0,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			0);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		if (m_mapItemInfo.empty())
			return;

		DWORD dwByte(0);

		for (auto& Iter : m_mapItemInfo)
		{
			int iLength(0);

			/*TCHAR szBuf[MAX_STR];
			lstrcpyW(szBuf, Iter.second->strImageName.GetString());
			iLength = lstrlen(szBuf);
			WriteFile(hFile, szBuf, sizeof(iLength), &dwByte, nullptr);*/

			iLength = (Iter.second->strImageName.GetLength() + 1) * sizeof(wchar_t);
			WriteFile(hFile, Iter.second->strImageName.GetString(), iLength, &dwByte, nullptr);

			iLength = (Iter.second->strName.GetLength() + 1) * sizeof(wchar_t);
			WriteFile(hFile, Iter.second->strName.GetString(), iLength, &dwByte, nullptr);

			iLength = (Iter.second->strDescription.GetLength() + 1) * sizeof(wchar_t);
			WriteFile(hFile, Iter.second->strDescription.GetString(), iLength, &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
}


//void CItemTool::OnStnClickedItemImg()
//{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//}


void CItemTool::OnImgListBox()
{
	int iIndex = m_ImgListBox.GetCurSel();
	if (iIndex == LB_ERR)
		return;

	CString strFindName;
	m_ImgListBox.GetText(iIndex, strFindName);

	m_strImageName = strFindName;

	auto iter = m_mapPngImage.find(strFindName);
	if (iter == m_mapPngImage.end())
		return;

	CImage* pImage = iter->second;

	if (!pImage)
		return;
	//이미지 저장
	m_pImage = pImage;

	//픽쳐 디시를 가져온다.
	CClientDC dc(&m_Picture);
	//렉트
	CRect rect;
	//픽쳐의 렉트를 가져온다.
	m_Picture.GetClientRect(&rect);
	//픽쳐를 렉트만큼 배경(파랑)으로 채운다.
	dc.FillSolidRect(rect, RGB(255, 255, 255));
	//이미지를 픽쳐에 렉트만큼 그린다.
	pImage->Draw(dc, rect);


}
