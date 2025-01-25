﻿#include "pch.h"
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
    DDX_Radio(pDX, IDC_RADIO_PLAYER, m_iCategorySelect);

    DDX_Control(pDX, IDC_PICTURE, m_Picture);

    DDX_Text(pDX, IDC_EDIT5, m_strName);
    DDX_Text(pDX, IDC_EDIT3, m_iAttack);
    DDX_Text(pDX, IDC_EDIT4, m_iHp);

    DDX_Control(pDX, IDC_LIST1, m_ListBox);
    DDX_Text(pDX, IDC_EDIT6, m_strFindName);

    DDX_Control(pDX, IDC_LIST3, m_ListBox3);
    DDX_Control(pDX, IDC_LIST2, m_ListBox2);
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
    ON_LBN_SELCHANGE(IDC_LIST2, &CUnitTool::OnLbnDblclkList2)
    ON_LBN_SELCHANGE(IDC_LIST3, &CUnitTool::OnLbnDblclkList3)
END_MESSAGE_MAP()


// 초기화
BOOL CUnitTool::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_ListBox3.AddString(_T("Player"));
    m_ListBox3.AddString(_T("Monster"));
    m_ListBox3.AddString(_T("NPC"));

    DragAcceptFiles(TRUE);

    // 시작 시 파일 로드 (이미지 리스트 불러오기)
    LoadFileData(L"../Save/UnitToolFiles.txt");

    return TRUE;
}

// 리스트박스(IDC_LIST1)에서 선택된 항목을 클릭했을 때
void CUnitTool::OnListBox()
{
    UpdateData(TRUE);

    int iIndex = m_ListBox.GetCurSel();
    if (iIndex == LB_ERR)
        return;

    CString strKey;
    m_ListBox.GetText(iIndex, strKey);

    // [FIX] "카테고리 : 유닛" → Replace(...) 제거.
    //      이제는 리스트박스 자체가 "카테고리:유닛이름:파일명" 형태 키를 가지고 있음.
    auto it = m_mapPngImages.find(strKey);
    if (it == m_mapPngImages.end())
    {
        AfxMessageBox(_T("이미지를 찾을 수 없습니다: ") + strKey);
        return;
    }
    DisplayImage(strKey);

    UpdateData(FALSE);
}

// 드롭된 파일 처리
void CUnitTool::OnDropFiles(HDROP hDropInfo)
{
    UpdateData(TRUE);

    // 1) 현재 선택된 카테고리
    CString strCategory;
    switch (m_iCategorySelect)
    {
    case 0: strCategory = _T("Player");  break;
    case 1: strCategory = _T("Monster"); break;
    case 2: strCategory = _T("NPC");     break;
    }

    // 2) 리스트2에서 현재 선택된 유닛 이름
    int iSel = m_ListBox2.GetCurSel();
    if (iSel == LB_ERR)
    {
        AfxMessageBox(_T("이미지를 추가할 유닛(리스트2)을 먼저 선택하세요."));
        DragFinish(hDropInfo);
        return;
    }

    CString strUnitName;
    m_ListBox2.GetText(iSel, strUnitName);

    // 3) 드롭된 파일들 경로 취득
    TCHAR szFilePath[MAX_PATH] = L"";
    int iFileCnt = DragQueryFile(hDropInfo, 0xFFFFFFFF, nullptr, 0);

    for (int i = 0; i < iFileCnt; ++i)
    {
        DragQueryFile(hDropInfo, i, szFilePath, MAX_PATH);
        CString strFullPath = szFilePath;

        // [FIX] 키는 "Category:UnitName:FileName" 으로 통일
        CString strFileName = PathFindFileName(strFullPath);

        CString strKey;
        strKey.Format(_T("%s:%s:%s"), strCategory, strUnitName, strFileName);

        // 중복 확인
        if (m_mapPngImages.find(strKey) != m_mapPngImages.end())
        {
            AfxMessageBox(_T("[중복] 이미 존재하는 항목입니다: ") + strKey);
            continue;
        }

        // 로딩
        CImage* pImg = new CImage();
        if (FAILED(pImg->Load(strFullPath)))
        {
            AfxMessageBox(_T("이미지를 로드할 수 없습니다: ") + strFullPath);
            delete pImg;
            continue;
        }
        m_mapPngImages[strKey] = pImg;
        m_mapFilePaths[strKey] = strFullPath;

        // m_mapCategory 에도 추가
        m_mapCategory[strCategory][strUnitName].push_back(strFullPath);

        // [FIX] 리스트박스(IDC_LIST1)에 바로 추가해 보고 싶다면:
        m_ListBox.AddString(strKey);
    }

    DragFinish(hDropInfo);

    // 새로운 이미지 목록을 갱신
    OnLbnDblclkList2();

    UpdateData(FALSE);
}

// 파일 저장
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

    int nCount = (int)m_mapUnitData.size();
    ar << nCount;

    for (auto& pair : m_mapUnitData)
    {
        UNITDATA* pData = pair.second;
        ar << pData->strName;
        ar << pData->iAttack;
        ar << pData->iHp;

        // BYTE -> int 변환
        ar << (int)pData->byJobIndex;
        ar << (int)pData->byItem;
    }

    ar.Close();
    file.Close();

    AfxMessageBox(_T("유닛 정보가 저장되었습니다."));
}

// 파일 불러오기
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

    for (auto& pair : m_mapPngImages)
    {
        pair.second->Destroy();
        delete pair.second;
    }
    m_mapPngImages.clear();
    m_mapFilePaths.clear();
}

void CUnitTool::SaveFileData(const CString& strFilePath)
{
    CStdioFile file;
    if (!file.Open(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
    {
        AfxMessageBox(_T("파일을 저장할 수 없습니다!"));
        return;
    }

    // "카테고리|유닛이름|전체경로" 형태로 저장
    for (auto& pair : m_mapFilePaths)
    {
        // pair.first 예) "Monster:슬라임:Slime01.png"
        // pair.second 예) "D:\Images\Slime01.png"
        CString strKey = pair.first;
        CString strPath = pair.second;

        // 콜론(:)을 기준으로 파싱
        int firstColon = strKey.Find(_T(":"));
        if (firstColon == -1) continue;
        int secondColon = strKey.Find(_T(":"), firstColon + 1);
        if (secondColon == -1) continue;

        CString strCategory = strKey.Left(firstColon);
        CString strUnitName = strKey.Mid(firstColon + 1, secondColon - (firstColon + 1));
        // 마지막 부분은 파일명
        // 하지만 여기서는 파일명은 별로 안 중요, 어차피 전체경로는 strPath에 있음

        CString strLine;
        strLine.Format(_T("%s|%s|%s\n"), strCategory, strUnitName, strPath);
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
        // 예: "Monster|슬라임|D:\Images\슬라임1.png"
        int pos1 = strLine.Find(_T("|"));
        if (pos1 == -1) continue;

        int pos2 = strLine.Find(_T("|"), pos1 + 1);
        if (pos2 == -1) continue;

        CString strCategory = strLine.Left(pos1);
        CString strName = strLine.Mid(pos1 + 1, pos2 - (pos1 + 1));
        CString strPath = strLine.Mid(pos2 + 1);

        // [FIX] 맵에 저장할 때 "Category:UnitName:FileName" 사용
        CString strFileName = PathFindFileName(strPath);

        CString strKey;
        strKey.Format(_T("%s:%s:%s"), strCategory, strName, strFileName);

        CImage* pImg = new CImage();
        if (FAILED(pImg->Load(strPath)))
        {
            AfxMessageBox(_T("이미지 로드 실패: ") + strPath);
            delete pImg;
            continue;
        }
        m_mapPngImages[strKey] = pImg;
        m_mapFilePaths[strKey] = strPath;

        // m_mapCategory 에도 넣음
        m_mapCategory[strCategory][strName].push_back(strPath);

        // [FIX] 리스트박스에도 같은 키로 추가
        m_ListBox.AddString(strKey);
    }

    file.Close();
}

// PNG 표시
void CUnitTool::DisplayImage(const CString& strKey)
{
    auto it = m_mapPngImages.find(strKey);
    if (it == m_mapPngImages.end())
    {
        AfxMessageBox(_T("이미지를 찾을 수가 없어요: ") + strKey);
        return;
    }

    CImage* pImage = it->second;
    CClientDC dc(&m_Picture);
    CRect rect;
    m_Picture.GetClientRect(&rect);

    dc.FillSolidRect(rect, RGB(255, 255, 255));
    pImage->Draw(dc, rect);
}

void CUnitTool::StartAnimation()
{
    if (!m_vecBitmaps.empty())
    {
        SetTimer(1, 100, NULL);
        m_iCurrentFrame = 0;
        m_AnimationCtrl.SetBitmap(m_vecBitmaps[m_iCurrentFrame]);
    }
}

void CUnitTool::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1 && !m_vecBitmaps.empty())
    {
        m_iCurrentFrame = (m_iCurrentFrame + 1) % m_vecBitmaps.size();
        m_AnimationCtrl.SetBitmap(m_vecBitmaps[m_iCurrentFrame]);
    }
    CDialog::OnTimer(nIDEvent);
}

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

void CUnitTool::OnBnClickedButton10()
{
    // TODO
}

void CUnitTool::OnBnClickedRadioPlayer()
{
    // TODO
}

void CUnitTool::OnBnClickedRadioMonster()
{
    // TODO
}

void CUnitTool::OnBnClickedRadioNpc()
{
    // TODO
}

// [적용] 버튼
void CUnitTool::OnBnClickedButtonApply()
{
    UpdateData(TRUE);

    CString strCategory;
    switch (m_iCategorySelect)
    {
    case 0: strCategory = _T("Player");  break;
    case 1: strCategory = _T("Monster"); break;
    case 2: strCategory = _T("NPC");     break;
    }

    if (m_strName.IsEmpty())
    {
        AfxMessageBox(_T("유닛 이름을 입력하세요."));
        return;
    }

    // 기존 로직대로 m_mapUnitData에 "Category:UnitName" 형태로 관리할 수도 있고,
    // 혹은 "Category:UnitName:FileName"를 쓰셔도 되는데, 여기서는 간단히 유지
    CString strKey;
    strKey.Format(_T("%s:%s"), strCategory, m_strName);

    UNITDATA* pData = nullptr;
    auto itData = m_mapUnitData.find(strKey);
    if (itData == m_mapUnitData.end())
    {
        pData = new UNITDATA;
        pData->strName = m_strName;
    }
    else
    {
        pData = itData->second;
    }

    pData->iAttack = m_iAttack;
    pData->iHp = m_iHp;

    m_mapUnitData[strKey] = pData;

    // m_mapCategory
    auto& mapUnitNameToImages = m_mapCategory[strCategory];
    if (mapUnitNameToImages.find(m_strName) == mapUnitNameToImages.end())
    {
        mapUnitNameToImages[m_strName] = vector<CString>();
    }

    // 리스트3에서 선택된 타입이 현재 strCategory와 같다면, 리스트2 갱신
    CString strSelectedType;
    int iTypeIndex = m_ListBox3.GetCurSel();
    if (iTypeIndex != LB_ERR)
    {
        m_ListBox3.GetText(iTypeIndex, strSelectedType);
        if (strSelectedType == strCategory)
        {
            m_ListBox2.ResetContent();
            auto itType = m_mapCategory.find(strSelectedType);
            if (itType != m_mapCategory.end())
            {
                for (auto& unitPair : itType->second)
                {
                    m_ListBox2.AddString(unitPair.first);
                }
            }
        }
    }

    AfxMessageBox(_T("유닛이 생성(또는 갱신)되었습니다."));
    UpdateData(FALSE);
}

// 삭제
void CUnitTool::OnBnClickedButtonDelete()
{
    UpdateData(TRUE);

    int iIndex = m_ListBox.GetCurSel();
    if (iIndex == LB_ERR)
    {
        AfxMessageBox(_T("삭제할 항목을 선택하세요."));
        return;
    }

    CString strKey;
    m_ListBox.GetText(iIndex, strKey);

    // [FIX] 이미 리스트박스가 "카테고리:유닛이름:파일명" 형태의 key 이므로
    // 그대로 find & 삭제
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

    m_ListBox.DeleteString(iIndex);

    AfxMessageBox(_T("선택된 항목이 삭제되었습니다."));
    UpdateData(FALSE);
}

// Delete 키로 삭제
BOOL CUnitTool::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE)
    {
        CWnd* pFocus = GetFocus();
        if (pFocus && pFocus->GetSafeHwnd() == m_ListBox.GetSafeHwnd())
        {
            OnBnClickedButtonDelete();
            return TRUE;
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}

// 리스트2(유닛 목록) 더블클릭 시 → 그 유닛의 모든 이미지 키를 리스트1에 표시
void CUnitTool::OnLbnDblclkList2()
{
    UpdateData(TRUE);

    int iIndex = m_ListBox2.GetCurSel();
    if (iIndex == LB_ERR)
        return;

    CString strSelectedUnit;
    m_ListBox2.GetText(iIndex, strSelectedUnit);

    int iTypeIndex = m_ListBox3.GetCurSel();
    if (iTypeIndex == LB_ERR)
        return;

    CString strSelectedType;
    m_ListBox3.GetText(iTypeIndex, strSelectedType);

    m_ListBox.ResetContent();

    auto itType = m_mapCategory.find(strSelectedType);
    if (itType != m_mapCategory.end())
    {
        auto itUnit = itType->second.find(strSelectedUnit);
        if (itUnit != itType->second.end())
        {
            // 해당 유닛의 모든 이미지 경로 vector
            for (auto& imagePath : itUnit->second)
            {
                // [FIX] 키를 만들어 리스트에 추가
                CString strFileName = PathFindFileName(imagePath);
                CString strKey;
                strKey.Format(_T("%s:%s:%s"), strSelectedType, strSelectedUnit, strFileName);

                m_ListBox.AddString(strKey);
            }
        }
    }

    UpdateData(FALSE);
}

// 리스트3(카테고리) 더블클릭 시 → 해당 카테고리에 속한 유닛 이름들을 리스트2에
void CUnitTool::OnLbnDblclkList3()
{
    UpdateData(TRUE);

    int iIndex = m_ListBox3.GetCurSel();
    if (iIndex == LB_ERR)
        return;

    CString strSelectedType;
    m_ListBox3.GetText(iIndex, strSelectedType);

    m_ListBox2.ResetContent();

    auto itType = m_mapCategory.find(strSelectedType);
    if (itType != m_mapCategory.end())
    {
        for (auto& unitPair : itType->second)
        {
            m_ListBox2.AddString(unitPair.first);
        }
    }

    UpdateData(FALSE);
}
