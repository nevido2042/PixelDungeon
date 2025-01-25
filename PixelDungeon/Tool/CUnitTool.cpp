#include "pch.h"
#include "Tool.h"
#include "CUnitTool.h"

IMPLEMENT_DYNAMIC(CUnitTool, CDialog)

CUnitTool::CUnitTool(CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_CUnitTool, pParent)
    , m_strName(_T(""))
    , m_iAttack(0)
    , m_iHp(0)
    , m_iMaxHp(0)
    , m_iEvasion(0.0f)
    , m_strFindName(_T(""))
    , m_iCurrentFrame(0)
    , m_iCategorySelect(0)
{
}

CUnitTool::~CUnitTool()
{
}

void CUnitTool::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    // 라디오 버튼 초기값
    DDX_Radio(pDX, IDC_RADIO_PLAYER, m_iCategorySelect);

    // Picture Control
    DDX_Control(pDX, IDC_PICTURE, m_Picture);

    // 유닛 정보
    DDX_Text(pDX, IDC_EDIT5, m_strName);
    DDX_Text(pDX, IDC_EDIT3, m_iAttack);
    DDX_Text(pDX, IDC_EDIT4, m_iHp);

    // 리스트박스
    DDX_Control(pDX, IDC_LIST1, m_ListBox);

    // 검색
    DDX_Text(pDX, IDC_EDIT6, m_strFindName);
}

BEGIN_MESSAGE_MAP(CUnitTool, CDialog)
    ON_BN_CLICKED(IDC_BUTTON4, &CUnitTool::OnSave)
    ON_BN_CLICKED(IDC_BUTTON5, &CUnitTool::OnLoad)
    ON_BN_CLICKED(IDC_BUTTON7, &CUnitTool::OnSearch)
    ON_LBN_SELCHANGE(IDC_LIST1, &CUnitTool::OnListBox)
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_WM_DROPFILES()
    ON_BN_CLICKED(IDC_BUTTON9, &CUnitTool::OnBnClickedButton9)
    ON_BN_CLICKED(IDC_RADIO_PLAYER, &CUnitTool::OnBnClickedRadioPlayer)
    ON_BN_CLICKED(IDC_RADIO_MONSTER, &CUnitTool::OnBnClickedRadioMonster)
    ON_BN_CLICKED(IDC_RADIO_NPC, &CUnitTool::OnBnClickedRadioNpc)
    ON_BN_CLICKED(IDC_BUTTON_APPLY, &CUnitTool::OnBnClickedButtonApply)
    ON_BN_CLICKED(IDC_BUTTON_DELETE, &CUnitTool::OnBnClickedButtonDelete)
END_MESSAGE_MAP()

// 초기화
BOOL CUnitTool::OnInitDialog()
{
    CDialog::OnInitDialog();

    // 드래그 앤 드롭 허용
    DragAcceptFiles(TRUE);

    // 시작 시 파일 로드 (이미지 리스트 불러오기)
    LoadFileData(L"../Save/UnitToolFiles.txt");

    return TRUE;
}

// 리스트박스에서 항목 선택 시 처리
void CUnitTool::OnListBox()
{
    UpdateData(TRUE);

    int iIndex = m_ListBox.GetCurSel();
    if (iIndex == LB_ERR)
        return;

    CString strSelected;
    m_ListBox.GetText(iIndex, strSelected);

    // 예: "Player : 슬라임" -> 실제 key는 "Player:슬라임"
    CString strKey = strSelected;
    strKey.Replace(_T(" : "), _T(":"));

    auto it = m_mapPngImages.find(strKey);
    if (it == m_mapPngImages.end())
    {
        AfxMessageBox(_T("이미지를 찾을 수 없습니다: ") + strKey);
        return;
    }

    // 이미지 표시
    DisplayImage(strKey);

    UpdateData(FALSE);
}

// 드롭된 파일 처리
void CUnitTool::OnDropFiles(HDROP hDropInfo)
{
    UpdateData(TRUE);

    // 라디오 버튼으로 카테고리 결정
    CString strCategory;
    switch (m_iCategorySelect)
    {
    case 0: strCategory = _T("Player");  break;
    case 1: strCategory = _T("Monster"); break;
    case 2: strCategory = _T("NPC");     break;
    }

    TCHAR szFilePath[MAX_PATH] = L"";
    int iFileCnt = DragQueryFile(hDropInfo, 0xFFFFFFFF, nullptr, 0);

    for (int i = 0; i < iFileCnt; ++i)
    {
        DragQueryFile(hDropInfo, i, szFilePath, MAX_PATH);
        CString strFullPath = szFilePath;
        CString strFileName = PathFindFileName(strFullPath);

        // 확장자 제거
        PathRemoveExtension(strFileName.GetBuffer());
        strFileName.ReleaseBuffer();

        // key: "카테고리:파일명"
        CString strKey;
        strKey.Format(_T("%s:%s"), strCategory, strFileName);

        // 중복 확인
        if (m_mapPngImages.find(strKey) != m_mapPngImages.end())
        {
            AfxMessageBox(_T("이미 존재하는 항목입니다: ") + strKey);
            continue;
        }

        // 실제 이미지 로드
        CImage* pImg = new CImage();
        if (FAILED(pImg->Load(strFullPath)))
        {
            AfxMessageBox(_T("이미지를 로드할 수 없습니다: ") + strFullPath);
            delete pImg;
            continue;
        }

        // 맵에 추가
        m_mapPngImages[strKey] = pImg;
        m_mapFilePaths[strKey] = strFullPath;

        // 리스트박스에 표시
        CString strListDisplay;
        strListDisplay.Format(_T("%s : %s"), strCategory, strFileName);
        m_ListBox.AddString(strListDisplay);
    }

    DragFinish(hDropInfo);
    UpdateData(FALSE);
}

// 파일 저장 버튼
void CUnitTool::OnSave()
{
    CFileDialog dlg(FALSE, _T("dat"), _T("UnitData.dat"),
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        _T("Data Files (*.dat)|*.dat|All Files (*.*)|*.*||"), this);

    if (dlg.DoModal() != IDOK) return;
    CString strPath = dlg.GetPathName();

    CFile file;
    if (!file.Open(strPath, CFile::modeCreate | CFile::modeWrite))
    {
        AfxMessageBox(_T("파일 열기 실패(저장)."));
        return;
    }

    CArchive ar(&file, CArchive::store);

    // 맵에 있는 유닛 수
    int nCount = (int)m_mapUnitData.size();
    ar << nCount;

    // 각 유닛 데이터 직렬화
    for (auto& pair : m_mapUnitData)
    {
        UNITDATA* pData = pair.second;
        ar << pData->strName;
        ar << pData->iAttack;
        ar << pData->iHp;

        // BYTE → int 변환
        ar << (int)pData->byJobIndex;
        ar << (int)pData->byItem;
    }

    ar.Close();
    file.Close();

    AfxMessageBox(_T("유닛 정보가 저장되었습니다."));
}

// 파일 불러오기 버튼
void CUnitTool::OnLoad()
{
    CFileDialog dlg(TRUE, _T("dat"), _T("UnitData.dat"),
        OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
        _T("Data Files (*.dat)|*.dat|All Files (*.*)|*.*||"), this);

    if (dlg.DoModal() != IDOK) return;
    CString strPath = dlg.GetPathName();

    CFile file;
    if (!file.Open(strPath, CFile::modeRead))
    {
        AfxMessageBox(_T("파일 열기 실패(불러오기)."));
        return;
    }

    CArchive ar(&file, CArchive::load);

    // 기존 자료 초기화
    for (auto& pair : m_mapUnitData)
        delete pair.second;
    m_mapUnitData.clear();
    m_ListBox.ResetContent();

    int nCount = 0;
    ar >> nCount;

    for (int i = 0; i < nCount; ++i)
    {
        UNITDATA* pData = new UNITDATA;
        int nJobIndex, nItem;

        ar >> pData->strName;
        ar >> pData->iAttack;
        ar >> pData->iHp;
        ar >> nJobIndex;
        ar >> nItem;

        pData->byJobIndex = (BYTE)nJobIndex;
        pData->byItem = (BYTE)nItem;

        m_mapUnitData.insert({ pData->strName, pData });
        m_ListBox.AddString(pData->strName);
    }

    ar.Close();
    file.Close();

    AfxMessageBox(_T("유닛 정보를 불러왔습니다."));
    UpdateData(FALSE);
}

// 검색
void CUnitTool::OnSearch()
{
    UpdateData(TRUE);

    if (m_strFindName.IsEmpty())
    {
        AfxMessageBox(_T("검색할 유닛 이름을 입력하세요."));
        return;
    }

    auto it = m_mapUnitData.find(m_strFindName);
    if (it == m_mapUnitData.end())
    {
        AfxMessageBox(_T("찾는 유닛이 없습니다."));
        return;
    }

    // 리스트박스에서 해당 항목 찾아 선택
    for (int i = 0; i < m_ListBox.GetCount(); ++i)
    {
        CString strItem;
        m_ListBox.GetText(i, strItem);
        if (strItem == it->first)
        {
            m_ListBox.SetCurSel(i);
            OnListBox();
            break;
        }
    }
}

// 대화 상자 파괴 시점
void CUnitTool::OnDestroy()
{
    CDialog::OnDestroy();

    // 현재 이미지 목록 파일로 저장
    SaveFileData(L"../Save/UnitToolFiles.txt");

    // 동적 할당된 이미지 해제
    for (auto& pair : m_mapPngImages)
    {
        pair.second->Destroy();
        delete pair.second;
    }
    m_mapPngImages.clear();
    m_mapFilePaths.clear();
}

// 이미지 목록 저장
void CUnitTool::SaveFileData(const CString& strFilePath)
{
    CStdioFile file;
    if (!file.Open(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
    {
        AfxMessageBox(_T("파일을 저장할 수 없습니다!"));
        return;
    }

    // 예: "Player:슬라임" -> "D:\Images\슬라임.png"
    // "Player|슬라임|D:\Images\슬라임.png" 형태로 저장
    for (auto& pair : m_mapFilePaths)
    {
        CString strKey = pair.first;
        CString strPath = pair.second;

        int pos = strKey.Find(_T(":"));
        if (pos == -1) continue;

        CString strCategory = strKey.Left(pos);
        CString strName = strKey.Mid(pos + 1);

        CString strLine;
        strLine.Format(_T("%s|%s|%s\n"), strCategory, strName, strPath);
        file.WriteString(strLine);
    }

    file.Close();
}

// 이미지 목록 불러오기
void CUnitTool::LoadFileData(const CString& strFilePath)
{
    CStdioFile file;
    if (!file.Open(strFilePath, CFile::modeRead | CFile::typeText))
    {
        AfxMessageBox(_T("파일을 불러올 수 없습니다!"));
        return;
    }

    m_mapPngImages.clear();
    m_mapFilePaths.clear();
    m_ListBox.ResetContent();

    CString strLine;
    while (file.ReadString(strLine))
    {
        // 예: "Player|슬라임|D:\Images\슬라임.png"
        int pos1 = strLine.Find(_T("|"));
        if (pos1 == -1) continue;

        int pos2 = strLine.Find(_T("|"), pos1 + 1);
        if (pos2 == -1) continue;

        CString strCategory = strLine.Left(pos1);
        CString strName = strLine.Mid(pos1 + 1, pos2 - (pos1 + 1));
        CString strPath = strLine.Mid(pos2 + 1);

        CString strKey;
        strKey.Format(_T("%s:%s"), strCategory, strName);

        CImage* pImg = new CImage();
        if (FAILED(pImg->Load(strPath)))
        {
            AfxMessageBox(_T("이미지 로드 실패: ") + strPath);
            delete pImg;
            continue;
        }

        m_mapPngImages[strKey] = pImg;
        m_mapFilePaths[strKey] = strPath;

        // 리스트박스에는 "Player : 슬라임"
        CString strList;
        strList.Format(_T("%s : %s"), strCategory, strName);
        m_ListBox.AddString(strList);
    }

    file.Close();
}

// PNG 이미지를 Picture Control에 표시
void CUnitTool::DisplayImage(const CString& strKey)
{
    auto it = m_mapPngImages.find(strKey);
    if (it == m_mapPngImages.end())
    {
        AfxMessageBox(_T("이미지를 찾을 수 없습니다: ") + strKey);
        return;
    }

    CImage* pImage = it->second;
    CClientDC dc(&m_Picture);
    CRect rect;
    m_Picture.GetClientRect(&rect);

    // 배경 흰색으로 클리어
    dc.FillSolidRect(rect, RGB(255, 255, 255));
    pImage->Draw(dc, rect);
}

// 애니메이션 시작
void CUnitTool::StartAnimation()
{
    if (!m_vecBitmaps.empty())
    {
        // 예시: 0.1초 간격으로 타이머 설정
        SetTimer(1, 100, NULL);
        m_iCurrentFrame = 0;
        m_AnimationCtrl.SetBitmap(m_vecBitmaps[m_iCurrentFrame]);
    }
}

// 타이머 콜백
void CUnitTool::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1 && !m_vecBitmaps.empty())
    {
        m_iCurrentFrame = (m_iCurrentFrame + 1) % m_vecBitmaps.size();
        m_AnimationCtrl.SetBitmap(m_vecBitmaps[m_iCurrentFrame]);
    }
    CDialog::OnTimer(nIDEvent);
}

// [리스트 초기화] 버튼
void CUnitTool::OnBnClickedButton9()
{
    m_ListBox.ResetContent();

    for (auto& pair : m_mapPngImages)
    {
        pair.second->Destroy();
        delete pair.second;
    }
    m_mapPngImages.clear();
    m_mapFilePaths.clear();

    AfxMessageBox(_T("이미지 목록이 초기화되었습니다."));
}

// 버튼10 (미구현)
void CUnitTool::OnBnClickedButton10()
{
    // TODO
}

// 라디오 버튼(플레이어)
void CUnitTool::OnBnClickedRadioPlayer()
{
    // TODO: 필요 시 구현
}

// 라디오 버튼(몬스터)
void CUnitTool::OnBnClickedRadioMonster()
{
    // TODO: 필요 시 구현
}

// 라디오 버튼(NPC)
void CUnitTool::OnBnClickedRadioNpc()
{
    // TODO: 필요 시 구현
}

// [적용] 버튼
void CUnitTool::OnBnClickedButtonApply()
{
    AfxMessageBox(_T("적용 버튼이 클릭되었습니다."));
}


// 리스트박스에서 선택된 항목 삭제
void CUnitTool::OnBnClickedButtonDelete()
{
    UpdateData(TRUE);

    // 선택된 항목 가져오기
    int iIndex = m_ListBox.GetCurSel();
    if (iIndex == LB_ERR)
    {
        AfxMessageBox(_T("삭제할 항목을 선택하세요."));
        return;
    }

    // 선택된 항목의 텍스트 가져오기
    CString strSelected;
    m_ListBox.GetText(iIndex, strSelected);

    // 예: "Player : 슬라임" -> "Player:슬라임"
    CString strKey = strSelected;
    strKey.Replace(_T(" : "), _T(":"));

    // 이미지와 데이터 맵에서 해당 항목 삭제
    auto itImage = m_mapPngImages.find(strKey);
    if (itImage != m_mapPngImages.end())
    {
        itImage->second->Destroy();
        delete itImage->second;
        m_mapPngImages.erase(itImage);
    }

    auto itData = m_mapUnitData.find(strKey);
    if (itData != m_mapUnitData.end())
    {
        delete itData->second;
        m_mapUnitData.erase(itData);
    }

    // 리스트박스에서 항목 제거
    m_ListBox.DeleteString(iIndex);

    AfxMessageBox(_T("선택된 항목이 삭제되었습니다."));
    UpdateData(FALSE);
}


BOOL CUnitTool::PreTranslateMessage(MSG* pMsg)
{ 
     ////////////////
    /////////////////////////
    //////////////////////// 
    /////////////////////
    ////////////////////
    ////////////////
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE) // 메세지 확인하고 // 이게 딜리트 버튼일때
    {
     
        CWnd* pFocus = GetFocus(); // 현재 포커스를 가진 컨트롤이 리스트박스일때
        if (pFocus && pFocus->GetSafeHwnd() == m_ListBox.GetSafeHwnd())
        {
      
            OnBnClickedButtonDelete();
            return TRUE; 
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}