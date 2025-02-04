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

	m_Picture.DragAcceptFiles();

	// 드래그 앤 드랍 활성화
	//DragAcceptFiles(TRUE);

	return TRUE;
}


BEGIN_MESSAGE_MAP(CItemTool, CDialog)
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
//	ON_STN_CLICKED(IDC_NAME2, &CItemTool::OnStnClickedName2)
ON_EN_CHANGE(IDC_ITEM_NAME, &CItemTool::OnEnChangeItemName)
ON_EN_CHANGE(IDC_ITEM_DESCRIPTION, &CItemTool::OnEnChangeItemDescription)
ON_BN_CLICKED(IDC_ADD_ITEM, &CItemTool::OnBnClickedAddItem)
ON_LBN_SELCHANGE(IDC_ITEM_LIST, &CItemTool::OnLbnSelchangeItemList)
ON_BN_CLICKED(IDC_ITEM_SAVE, &CItemTool::OnBnClickedItemSave)
END_MESSAGE_MAP()
// CItemTool 메시지 처리기

void CItemTool::OnDestroy()
{
	CDialog::OnDestroy();

	for_each(m_mapPngImage.begin(), m_mapPngImage.end(), [](auto& MyPair)
		{
			(MyPair.second).pImage->Destroy();
			Safe_Delete((MyPair.second).pImage);
		});

	m_mapPngImage.clear();

	//하나의 이미지로 여러아이템을 쓰면 터짐, 어케하지 (같은 이미지라도 다시 할당 받는 방법을 써야하나)
	//언오드셋으로 해결
	std::unordered_set<void*> deletedPointers;

	for_each(m_mapItemInfo.begin(), m_mapItemInfo.end(), [&](auto& MyPair)
		{
			IMAGE_INFO& tImgInfo = MyPair.second.tImgInfo;
			if (tImgInfo.pImage)
			{
				// 이미 해제된 포인터인지 확인
				if (deletedPointers.find(tImgInfo.pImage) == deletedPointers.end())
				{
					deletedPointers.insert(tImgInfo.pImage);
					tImgInfo.pImage->Destroy();
					Safe_Delete(tImgInfo.pImage); // 해제 후 nullptr
				}
				else
				{
					tImgInfo.pImage = nullptr; // 중복 참조된 포인터는 nullptr 설정
				}
			}
		});

	m_mapItemInfo.clear();
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
	GetDlgItemText(IDC_ITEM_NAME, m_strItemDescription);

	// 디버그용으로 출력
	TRACE(_T("Item Name Changed: %s\n"), m_strItemDescription);
}


void CItemTool::OnBnClickedAddItem()
{
	if (m_strItemName.IsEmpty())
	{
		return;
	}

	ITEM_INFO tItemInfo;
	tItemInfo.strName = m_strItemName;
	tItemInfo.strDescription = m_strItemDescription;
	tItemInfo.tImgInfo.pImage = m_pImage;
	tItemInfo.tImgInfo.strRelative;

	m_mapItemInfo.insert({ m_strItemName, tItemInfo });

	m_ListBox.AddString(m_strItemName);
}


void CItemTool::OnLbnSelchangeItemList()
{
	int iIndex = m_ListBox.GetCurSel();
	if (iIndex == LB_ERR)
		return;

	CString strFindName;
	m_ListBox.GetText(iIndex, strFindName);

	auto iter = m_mapItemInfo.find(strFindName);
	if (iter == m_mapItemInfo.end())
		return;

	CImage* pImage = iter->second.tImgInfo.pImage;

	if (!pImage)
		return;

	// CImage에서 HBITMAP 가져오기
	HBITMAP hBitmap = (HBITMAP)pImage->Detach();

	// CStatic에 이미지 설정
	m_Picture.SetBitmap(hBitmap);

	// 다시 CImage에 HBITMAP 연결
	pImage->Attach(hBitmap);

	// 이름과 설명 출력
	m_ItemName.SetWindowTextW(iter->first.GetString());
	m_ItemDescription.SetWindowTextW(iter->second.strDescription.GetString());
}


void CItemTool::OnBnClickedItemSave()
{
	CFileDialog		Dlg(FALSE,		// TRUE(불러오기), FALSE(다른 이름으로 저장) 모드 지정
		L"dat",		// default 확장자명
		L"*.dat",	// 대화 상자에 표시될 최초 파일명
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

		for (auto& ItemInfo : m_mapItemInfo)
		{
			WriteFile(hFile, &ItemInfo, sizeof(ITEM_INFO), &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
}
