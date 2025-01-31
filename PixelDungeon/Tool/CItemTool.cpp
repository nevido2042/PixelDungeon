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
	ITEM_INFO tItemInfo;
	tItemInfo.strName = m_strItemName;
	tItemInfo.strDescription = m_strItemDescription;
	tItemInfo.tImgInfo.pImage = m_pImage;
	tItemInfo.tImgInfo.strRelative;

	m_mapItemInfo.insert({ m_strItemName, tItemInfo });

	m_ListBox.AddString(m_strItemName);
}
