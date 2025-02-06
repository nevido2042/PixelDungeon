#include "pch.h"
#include "Tool.h"
#include "CUnitTool.h"

IMPLEMENT_DYNAMIC(CUnitTool, CDialog)


CUnitTool* g_pUnitTool = nullptr;

CUnitTool::CUnitTool(CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_CUnitTool, pParent)
    , m_strName(_T(""))
    , m_iAttack(0)
    , m_iHp(0)
    , m_fEvasion(0.0f)
    , m_iLuck(0)
    ,m_fAttackSpeed(0.f)
    , m_iCurrentFrame(0)
    , m_iCategorySelect(0) 
{
   g_pUnitTool = this;
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
    DDX_Text(pDX, IDC_EDIT_LEVEL, m_iLevel);
    DDX_Text(pDX, IDC_EDIT_ATTACKSPEED, m_fAttackSpeed);
    DDX_Text(pDX, IDC_EDIT_LUCK, m_iLuck);
    DDX_Text(pDX, IDC_EDIT_MOVESPEED, m_fMoveSpeed);
    DDX_Text(pDX, IDC_EDIT_EVASION, m_fEvasion);


    DDX_Control(pDX, IDC_LIST1, m_ListBox);

    DDX_Control(pDX, IDC_LIST3, m_ListBox3);
    DDX_Control(pDX, IDC_LIST2, m_ListBox2);

  
}
BEGIN_MESSAGE_MAP(CUnitTool, CDialog)
    ON_LBN_SELCHANGE(IDC_LIST1, &CUnitTool::OnListBox)
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_WM_DROPFILES()
    ON_BN_CLICKED(IDC_BUTTON9, &CUnitTool::OnBnClickedButton9)
    ON_BN_CLICKED(IDC_RADIO_PLAYER, &CUnitTool::OnBnClickedRadioPlayer) // 플레이어 꼬리표
    ON_BN_CLICKED(IDC_RADIO_MONSTER, &CUnitTool::OnBnClickedRadioMonster) // 몬스터 꼬리표
    ON_BN_CLICKED(IDC_RADIO_NPC, &CUnitTool::OnBnClickedRadioNpc) // NPC 꼬리표
    ON_BN_CLICKED(IDC_BUTTON_APPLY, &CUnitTool::OnBnClickedButtonApply) // 객체생성 버튼
    ON_BN_CLICKED(IDC_BUTTON_DELETE, &CUnitTool::OnBnClickedButtonDelete) //객체삭제 버튼
    ON_LBN_SELCHANGE(IDC_LIST2, &CUnitTool::OnLbnDblclkList2)
    ON_LBN_SELCHANGE(IDC_LIST3, &CUnitTool::OnLbnDblclkList3)
 
    ON_BN_CLICKED(IDC_BUTTON1, &CUnitTool::OnBnClickedPause) // 일시 정지 버튼
    ON_BN_CLICKED(IDC_DELETE_IMAGE2, &CUnitTool::OnBnClickedDeleteImage2) // 이미지 삭제 버튼



END_MESSAGE_MAP()


// 초기화
BOOL CUnitTool::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_ListBox3.AddString(_T("Player"));
    m_ListBox3.AddString(_T("Monster"));
    m_ListBox3.AddString(_T("NPC"));

 
    LoadFileData(L"../Save/UnitToolFiles.txt"); // 이미지 먼저
    LoadUnitData(L"../Save/UnitData.txt");      // 그 다음 유닛

    DragAcceptFiles(TRUE);

  


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

    // 이제 strKey == "Monster:신승훈:신승훈01.png" 으로 들어옴
    auto it = m_mapPngImages.find(strKey);
    if (it == m_mapPngImages.end())
    {
        AfxMessageBox(_T("이미지를 찾을 수 없습니다: ") + strKey);
        return;
    }
    DisplayImage(strKey);

    UpdateData(FALSE);
}


// 드롭된 파일 처리 함수
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

    // 3) 드롭된 파일들 경로 취득 << 이거 절대경로로 
    TCHAR szFilePath[MAX_PATH] = L"";
    int iFileCnt = DragQueryFile(hDropInfo, 0xFFFFFFFF, nullptr, 0);

    for (int i = 0; i < iFileCnt; ++i)
    {
        DragQueryFile(hDropInfo, i, szFilePath, MAX_PATH);
        CString strFullPath = szFilePath;


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






// 대화 상자 파괴 시점
void CUnitTool::OnDestroy()
{
    // 이미지 목록 저장
    SaveFileData(L"../Save/UnitToolFiles.txt");

    // 유닛 데이터 저장
    SaveUnitData(L"../Save/UnitData.txt");

    // 동적 할당된 이미지/데이터 해제
    for (auto& pair : m_mapPngImages)
    {
        pair.second->Destroy();
        delete pair.second;
    }
    m_mapPngImages.clear();
    m_mapFilePaths.clear();

    for (auto& kv : m_mapUnitData)
    {
        delete kv.second;
    }
    m_mapUnitData.clear();

    KillTimer(m_AnimationTimer);

    CDialog::OnDestroy();

}
void CUnitTool::SaveUnitData(const CString& strFilePath)
{
    CString strAbsolutePath = ConvertToAbsolutePath(strFilePath); // 상대경로 -> 절대경로 변환

    CStdioFile file;
    if (!file.Open(strAbsolutePath, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
    {
        CString errorMsg;
        errorMsg.Format(_T("⚠ 유닛 데이터 파일을 저장할 수 없습니다! 경로를 확인하세요.\n경로: %s"), strAbsolutePath);
        AfxMessageBox(errorMsg);
        return;
    }

    for (auto& kv : m_mapUnitData)
    {
        const CString& strFullKey = kv.first;
        UNITDATA* pData = kv.second;
        if (pData == nullptr)
            continue;

        int posColon = strFullKey.Find(_T(":"));
        if (posColon == -1)
            continue;

        CString strCategory = strFullKey.Left(posColon);
        CString strUnitName = strFullKey.Mid(posColon + 1);


        // 파일에 저장 (상대경로 변환 후)
        CString strLine;
        strLine.Format(_T("%s|%s|%d|%d|%d|%.2f|%d|%.2f|%.2f\n"),
            strCategory, pData->strName, pData->iAttack, pData->iHp,
            pData->iLevel, pData->fAttackSpeed, pData->iLuck,
            pData->fMoveSpeed, pData->fEvasion);

        file.WriteString(strLine);
    }

    file.Close();
}


//////////
void CUnitTool::LoadUnitData(const CString& strFilePath)
{
    CString strAbsolutePath = ConvertToAbsolutePath(strFilePath);

    CStdioFile file;
    if (!file.Open(strAbsolutePath, CFile::modeRead | CFile::typeText))
    {
        CString errorMsg;
        errorMsg.Format(_T("⚠ 유닛 데이터 파일을 불러올 수 없습니다!\n경로: %s"), strAbsolutePath);
        AfxMessageBox(errorMsg);
        return;
    }

    // 기존 데이터 삭제
    for (auto& kv : m_mapUnitData)
    {
        delete kv.second;
    }
    m_mapUnitData.clear();
 

    CString strLine;
    while (file.ReadString(strLine))
    {
        if (strLine.IsEmpty()) continue;

        TRACE(_T("불러온 유닛 데이터: %s\n"), strLine);

        int pos[8];
        pos[0] = strLine.Find(_T("|"));
        for (int i = 1; i < 8; i++)
        {
            pos[i] = strLine.Find(_T("|"), pos[i - 1] + 1);
            if (pos[i] == -1) continue;
        }

        CString strCategory = strLine.Left(pos[0]); // Player, Monster, NPC
        CString strUnitName = strLine.Mid(pos[0] + 1, pos[1] - (pos[0] + 1));

        int iAttack = _ttoi(strLine.Mid(pos[1] + 1, pos[2] - (pos[1] + 1)));
        int iHp = _ttoi(strLine.Mid(pos[2] + 1, pos[3] - (pos[2] + 1)));
        int iLevel = _ttoi(strLine.Mid(pos[3] + 1, pos[4] - (pos[3] + 1)));
        float fAttackSpeed = (float)_ttof(strLine.Mid(pos[4] + 1, pos[5] - (pos[4] + 1)));
        int iLuck = _ttoi(strLine.Mid(pos[5] + 1, pos[6] - (pos[5] + 1)));
        float fMoveSpeed = (float)_ttof(strLine.Mid(pos[6] + 1, pos[7] - (pos[6] + 1)));
        float fEvasion = (float)_ttof(strLine.Mid(pos[7] + 1));

        UNITDATA* pData = new UNITDATA;
        pData->strName = strUnitName;
        pData->iAttack = iAttack;
        pData->iHp = iHp;
        pData->iLevel = iLevel;
        pData->fAttackSpeed = fAttackSpeed;
        pData->iLuck = iLuck;
        pData->fMoveSpeed = fMoveSpeed;
        pData->fEvasion = fEvasion;

        CString strKey;
        strKey.Format(_T("%s:%s"), strCategory, strUnitName);

        TRACE(_T("저장된 유닛: %s (공격력: %d, 체력: %d)\n"), strKey, iAttack, iHp);

        m_mapUnitData[strKey] = pData;

        if (m_mapCategory[strCategory].find(strUnitName) == m_mapCategory[strCategory].end())
        {
            // 이 유닛 이름으로 이미지 벡터가 아직 없다면, 일단 빈 벡터를 만들긴 할껀디..
            // (만약 LoadFileData()에서 이미 push_back 해줬다면, 아래 코드가 실행 안 됨)
            m_mapCategory[strCategory][strUnitName] = vector<CString>();
        }
    }

    file.Close();
}





void CUnitTool::SaveFileData(const CString& strFilePath)
{
    CStdioFile file;
    if (!file.Open(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
    {
        AfxMessageBox(_T("파일을 저장할 수 없습니다!"));
        return;
    }

    for (auto& pair : m_mapFilePaths)
    {
        CString strKey = pair.first;
        CString strPath = ConvertToRelativePath(pair.second); // 상대 경로 변환

  

        CString strLine;
        strLine.Format(_T("%s|%s\n"), strKey, strPath);
        file.WriteString(strLine);
    }

    file.Close();
}


void CUnitTool::LoadFileData(const CString& strFilePath)
{
    CStdioFile file;
    if (!file.Open(strFilePath, CFile::modeRead | CFile::typeText))
    {
        AfxMessageBox(_T("파일을 불러올 수 없습니다!"));
        return;
    }

    // 기존 데이터 초기화
    m_mapPngImages.clear();
    m_mapFilePaths.clear();
    m_mapCategory.clear();
    m_ListBox.ResetContent();

    CString strLine;
    while (file.ReadString(strLine))
    {
        int pos = strLine.Find(_T("|"));
        if (pos == -1) continue;

        CString strKey = strLine.Left(pos);
        CString strRelPath = strLine.Mid(pos + 1);
        CString strFullPath = ConvertToAbsolutePath(strRelPath);

        if (GetFileAttributes(strFullPath) == INVALID_FILE_ATTRIBUTES)
        {
            continue;
        }

       
        CImage* pImg = new CImage();
        if (FAILED(pImg->Load(strFullPath)))
        {
            delete pImg;
            continue;
        }

        // 맵에 저장
        m_mapPngImages[strKey] = pImg;
        m_mapFilePaths[strKey] = strFullPath;

        // [FIX] 🔥 카테고리-유닛-이미지 경로 저장
        int firstColon = strKey.Find(_T(':'));
        int secondColon = strKey.Find(_T(':'), firstColon + 1);
        if (firstColon == -1 || secondColon == -1) continue;

        CString strCategory = strKey.Left(firstColon);
        CString strUnit = strKey.Mid(firstColon + 1, secondColon - (firstColon + 1));

        // 카테고리, 유닛별 이미지 목록 저장
        m_mapCategory[strCategory][strUnit].push_back(strFullPath);
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


//CImage* pImage = iter->second.pImage;  // iter는   map<CString, CImage*>임.
//CClientDC dc(&m_Picture); // m_Picture는 픽쳐 컨트롤의 제어 변수임
//CRect rect;  // 백 이미지 넣을 렉트 선언해주는거
//
//m_Picture.GetClientRect(&rect); 
//dc.FillSolidRect(rect, RGB(255, 255, 255));
//pImage->Draw(dc, rect); // 이미지를 넣는 부분
//
//UpdateData(FALSE);


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
    if (nIDEvent == 1 && !m_ImagePaths.empty())
    {
        CString currentImagePath = m_ImagePaths[m_CurrentFrameIndex];

        CImage image;
        if (FAILED(image.Load(currentImagePath)))
        {
            AfxMessageBox(_T("이미지 로드 실패: ") + currentImagePath);
            KillTimer(m_AnimationTimer);
            return;
        }

        CClientDC dc(&m_Picture);
        CRect rect;
        m_Picture.GetClientRect(&rect);

        dc.FillSolidRect(rect, RGB(255, 255, 255));
        image.Draw(dc, rect);

        CString fileName = PathFindFileName(currentImagePath);
        for (int i = 0; i < m_ListBox.GetCount(); ++i)
        {
            CString listItem;
            m_ListBox.GetText(i, listItem);
            if (listItem == fileName)
            {
                m_ListBox.SetCurSel(i);
                break;
            }
        }

        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_ImagePaths.size();
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
        AfxMessageBox(_T("유닛 이름을 입력"));
        return;
    }

    // ✅ 플레이어 타입은 하나만 생성 가능하도록 체크
    if (strCategory == _T("Player"))
    {
        auto itPlayer = m_mapCategory.find(_T("Player"));
        if (itPlayer != m_mapCategory.end() && !itPlayer->second.empty())
        {
            AfxMessageBox(_T(" 플레이어는 하나만 생성할 수 있습니다!"));
            return;
        }
    }

    // 기존 로직: m_mapUnitData에 저장
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

    // 유닛 카테고리 맵에 추가
    auto& mapUnitNameToImages = m_mapCategory[strCategory];
    if (mapUnitNameToImages.find(m_strName) == mapUnitNameToImages.end())
    {
        mapUnitNameToImages[m_strName] = vector<CString>();
    }

    // 리스트 박스 갱신
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

    // 1) m_mapPngImages 에서 제거
    auto itImage = m_mapPngImages.find(strKey);
    if (itImage != m_mapPngImages.end())
    {
        itImage->second->Destroy();
        delete itImage->second;
        m_mapPngImages.erase(itImage);
    }

    // 2) m_mapFilePaths 에서 제거
    auto itPath = m_mapFilePaths.find(strKey);
    if (itPath != m_mapFilePaths.end())
    {
        m_mapFilePaths.erase(itPath);
    }

    CString strCategory, strUnit, strFilename;
    {
        int firstColon = strKey.Find(_T(':'));
        int secondColon = strKey.Find(_T(':'), firstColon + 1);
        strCategory = strKey.Left(firstColon);
        strUnit = strKey.Mid(firstColon + 1, secondColon - (firstColon + 1));
        strFilename = strKey.Mid(secondColon + 1);

    
        auto& vecPaths = m_mapCategory[strCategory][strUnit];
        for (auto it = vecPaths.begin(); it != vecPaths.end(); ++it)
        {
            if (PathFindFileName(*it) == strFilename)
            {
                vecPaths.erase(it);
                break;
            }
        }

       
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

void CUnitTool::OnLbnDblclkList2()
{
    UpdateData(TRUE);

    int iIndex = m_ListBox2.GetCurSel();
    if (iIndex == LB_ERR)
        return;

    // 선택한 유닛 이름을 저장!
    m_ListBox2.GetText(iIndex, m_strSelectedUnit);

    int iTypeIndex = m_ListBox3.GetCurSel();
    if (iTypeIndex == LB_ERR)
        return;

    // 선택한 카테고리를 저장!
    m_ListBox3.GetText(iTypeIndex, m_strSelectedCategory);

    TRACE(_T("[디버깅] 선택된 유닛: %s, 선택된 카테고리: %s\n"), m_strSelectedUnit, m_strSelectedCategory);

    // 리스트박스1 초기화
    m_ListBox.ResetContent();

    // 애니메이션용 벡터 초기화
    m_ImagePaths.clear();
    m_CurrentFrameIndex = 0;

    auto itType = m_mapCategory.find(m_strSelectedCategory);
    if (itType != m_mapCategory.end())
    {
        auto itUnit = itType->second.find(m_strSelectedUnit);
        if (itUnit != itType->second.end())
        {
            // 해당 유닛의 이미지 목록
            for (auto& imagePath : itUnit->second)
            {
                CString fileName = PathFindFileName(imagePath);
                CString fullKey;
                fullKey.Format(_T("%s:%s:%s"), m_strSelectedCategory, m_strSelectedUnit, fileName);

                // 리스트박스1에 추가
                m_ListBox.AddString(fullKey);

                // 애니메이션 재생을 위해 m_ImagePaths에도 추가
                m_ImagePaths.push_back(imagePath);
            }
        }
    }

    OnLbnSelchangeList2();
    UpdateData(FALSE);
}







void CUnitTool::OnLbnDblclkList3()
{
    UpdateData(TRUE);

    int iIndex = m_ListBox3.GetCurSel();
    if (iIndex == LB_ERR)
        return;

    CString strSelectedType;
    m_ListBox3.GetText(iIndex, strSelectedType);

    m_ListBox2.ResetContent();

    //  선택한 카테고리의 유닛만 추가
    auto itType = m_mapCategory.find(strSelectedType);
    if (itType != m_mapCategory.end())
    {
        for (auto& unitPair : itType->second)
        {
            TRACE(_T("리스트2에 추가: %s\n"), unitPair.first);
            m_ListBox2.AddString(unitPair.first);
        }
    }

    UpdateData(FALSE);
}






void CUnitTool::OnBnClickedDeleteImage2()
{
    int iSel = m_ListBox.GetCurSel();
    if (iSel == LB_ERR)
    {
        AfxMessageBox(_T("이미지 선택하고 삭제 버튼 눌러야돼"));
        return;
    }

    CString strKey;
    m_ListBox.GetText(iSel, strKey); // "Monster:신승훈:신승훈01.png" 식의 키라고 가정

    //  m_mapPngImages 에서 제거
    auto itImage = m_mapPngImages.find(strKey);
    if (itImage != m_mapPngImages.end())
    {
        itImage->second->Destroy();
        delete itImage->second;
        m_mapPngImages.erase(itImage);
    }

    //  m_mapFilePaths 에서 제거
    auto itPath = m_mapFilePaths.find(strKey);
    if (itPath != m_mapFilePaths.end())
        m_mapFilePaths.erase(itPath);

    //  m_mapCategory
    int firstColon = strKey.Find(_T(':'));
    int secondColon = strKey.Find(_T(':'), firstColon + 1);
    CString strCategory = strKey.Left(firstColon);
    CString strUnit = strKey.Mid(firstColon + 1, secondColon - (firstColon + 1));
    CString strFilename = strKey.Mid(secondColon + 1);

    auto& vecPaths = m_mapCategory[strCategory][strUnit];
    for (auto itVec = vecPaths.begin(); itVec != vecPaths.end(); ++itVec)
    {
        if (PathFindFileName(*itVec) == strFilename)
        {
            vecPaths.erase(itVec);
            break;
        }
    }

    // (리스트박스에서 제거
    m_ListBox.DeleteString(iSel);


    auto itVec2 = std::find_if(m_ImagePaths.begin(), m_ImagePaths.end(), [&](const CString& path) {
        return PathFindFileName(path) == strFilename;
        });
    if (itVec2 != m_ImagePaths.end())
        m_ImagePaths.erase(itVec2);

    //  애니메이션 관련 !! 
    if (m_ImagePaths.empty())
    {
        KillTimer(m_AnimationTimer);
        m_AnimationTimer = 0;
        CClientDC dc(&m_Picture);
        CRect rect;
        m_Picture.GetClientRect(&rect);
        dc.FillSolidRect(rect, RGB(255, 255, 255));
    }

    AfxMessageBox(_T("이미지 삭제 완료!!!!!!!!!!"));
}



void CUnitTool::OnBnClickedPause()
{
    if (m_AnimationTimer != 0)
    {
        // 이미 타이머가 켜져 있으면 -> 끄기 (일시 정지)
        KillTimer(m_AnimationTimer);
        m_AnimationTimer = 0;
        AfxMessageBox(_T("애니메이션 스탑!!!"));
    }
    else
    {
        // 타이머가 꺼져있으면 -> 켜기 

        //  만약 m_ImagePaths가 비었다면 재생할 수 없으니 체크
        if (m_ImagePaths.empty())
        {
            AfxMessageBox(_T("재생할 이미지가 없음!!! 리스트박스에 이미지 추가해주세용"));
            return;
        }

        // 첫 프레임부터 다시 그릴 수 있도록 
        m_CurrentFrameIndex = 0;

        // 타이머 시작
        m_AnimationTimer = SetTimer(1, 100, NULL);
        AfxMessageBox(_T("애니메이션 시작!!"));
    }
}

CString CUnitTool::ConvertToRelativePath(const CString& fullPath)
{
    TCHAR currentDir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentDir);

    TCHAR relativePath[MAX_PATH];
    if (PathRelativePathTo(relativePath, currentDir, FILE_ATTRIBUTE_DIRECTORY, fullPath, FILE_ATTRIBUTE_NORMAL))
    {
        return CString(relativePath);
    }

    // 변환 실패 시 원래 절대 경로 반환 (안전 장치)
    return fullPath;
}

CString CUnitTool::ConvertToAbsolutePath(const CString& relativePath)
{
    TCHAR absolutePath[MAX_PATH];
    if (GetFullPathName(relativePath, MAX_PATH, absolutePath, nullptr))
    {
        return CString(absolutePath);
    }

    // 변환 실패 시 원래 상대 경로 반환 (안전 장치)
    return relativePath;
}

// CUnitTool.cpp
void CUnitTool::OnLbnSelchangeList2()
{
    UpdateData(TRUE);

    int iIndex = m_ListBox2.GetCurSel();
    if (iIndex != LB_ERR)
    {
        CString strUnitName;
        m_ListBox2.GetText(iIndex, strUnitName);

        int iTypeIndex = m_ListBox3.GetCurSel();
        if (iTypeIndex != LB_ERR)
        {
            CString strCategory;
            m_ListBox3.GetText(iTypeIndex, strCategory);

            // 선택된 유닛의 첫 번째 이미지 경로 찾기
            auto& vecImages = m_mapCategory[strCategory][strUnitName];
            if (!vecImages.empty())
            {
                CString strFileName = PathFindFileName(vecImages[0]);
                CString strKey;
                strKey.Format(_T("%s:%s:%s"), strCategory, strUnitName, strFileName);

           
                if (m_pToolView)
                {
                    CToolView* pToolView = (CToolView*)m_pToolView;
                    pToolView->DisplayImage(strKey);
                }
            }
        }
    }

    UpdateData(FALSE);
}

CString CUnitTool::GetUnitImagePath(const CString& strKey)
{
    auto it = m_mapFilePaths.find(strKey);
    if (it == m_mapFilePaths.end())
    {
        return _T("");
    }
    return it->second;
}
