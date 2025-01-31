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
    DDX_Radio(pDX, IDC_RADIO_PLAYER, m_iCategorySelect);

    DDX_Control(pDX, IDC_PICTURE, m_Picture);



    DDX_Text(pDX, IDC_EDIT5, m_strName);
    DDX_Text(pDX, IDC_EDIT3, m_iAttack);
    DDX_Text(pDX, IDC_EDIT4, m_iHp);

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
    ON_STN_CLICKED(IDC_PICTURE, &CUnitTool::OnStnClickedPicture)
    ON_BN_CLICKED(IDC_BUTTON1, &CUnitTool::OnBnClickedPause) // 일시 정지 버튼
    ON_BN_CLICKED(IDC_DELETE_IMAGE2, &CUnitTool::OnBnClickedDeleteImage2) // 이미지 삭제 버튼


    ON_BN_CLICKED(IDOK, &CUnitTool::OnBnClickedOk)
  

    ON_STN_CLICKED(IDC_evasion, &CUnitTool::OnStnClickedevasion)
END_MESSAGE_MAP()


// 초기화
BOOL CUnitTool::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_ListBox3.AddString(_T("Player"));
    m_ListBox3.AddString(_T("Monster"));
    m_ListBox3.AddString(_T("NPC"));
    // 이미지 리스트 불러오기
    LoadFileData(L"../Save/UnitToolFiles.txt");

    // 유닛 데이터 불러오기
    LoadUnitData(L"../Save/UnitData.txt");

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

    // 이제 strKey == "Monster:슬라임:Slime01.png" 로 들어옴
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

    // 3) 드롭된 파일들 경로 취득 << 이거 절대경로로 
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
    // (1) 파일 열기
    CStdioFile file;
    if (!file.Open(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
    {
        AfxMessageBox(_T("유닛 데이터 파일을 열 수 없습니다!"));
        return;
    }

    // (2) m_mapUnitData 순회하며 한 줄씩 저장
    //     key 형태가 "Category:UnitName" 이라고 가정
    for (auto& kv : m_mapUnitData)
    {
        const CString& strFullKey = kv.first;   // "Player:Hero" 등
        UNITDATA* pData = kv.second;
        if (pData == nullptr)
            continue;

        // 먼저 Category, UnitName 분리
        int posColon = strFullKey.Find(_T(":"));
        if (posColon == -1)
            continue; // 잘못된 키라면 패스

        CString strCategory = strFullKey.Left(posColon);
        CString strUnitName = strFullKey.Mid(posColon + 1);

        // 필요하다면 pData->iMaxHp, m_iEvasion 등도 같이 저장 가능
        // 여기서는 예시로 iAttack, iHp 두 가지만 저장
        CString strLine;
        strLine.Format(_T("%s|%s|%d|%d\n"),
            strCategory,
            strUnitName,
            pData->iAttack,
            pData->iHp);

        file.WriteString(strLine);
    }

    file.Close();
}
//////////
void CUnitTool::LoadUnitData(const CString& strFilePath)
{
    // (1) 파일 열기
    CStdioFile file;
    if (!file.Open(strFilePath, CFile::modeRead | CFile::typeText))
    {
        // 없으면 없는 대로 패스(또는 메시지 박스)
        // AfxMessageBox(_T("유닛 데이터 파일을 열 수 없습니다!"));
        return;
    }

    // (2) 기존 데이터 날리거나(또는 append) 필요에 따라 정리
    //     여기서는 중복 방지를 위해 싹 날린다고 가정
    for (auto& kv : m_mapUnitData)
    {
        delete kv.second;
    }
    m_mapUnitData.clear();

    // (3) 파일에서 한 줄씩 읽어서 파싱
    CString strLine;
    while (file.ReadString(strLine))
    {
        // 예) "Monster|슬라임|25|100"
        //     [카테고리]|[유닛이름]|[공격력]|[HP]
        if (strLine.IsEmpty())
            continue;

        // 구분자(|) 찾아 분리
        int pos1 = strLine.Find(_T("|"));
        if (pos1 == -1) continue;
        int pos2 = strLine.Find(_T("|"), pos1 + 1);
        if (pos2 == -1) continue;
        int pos3 = strLine.Find(_T("|"), pos2 + 1);
        if (pos3 == -1) continue;

        CString strCategory = strLine.Left(pos1);
        CString strUnitName = strLine.Mid(pos1 + 1, pos2 - (pos1 + 1));
        CString strAttack = strLine.Mid(pos2 + 1, pos3 - (pos2 + 1));
        CString strHp = strLine.Mid(pos3 + 1);

        int iAttack = _ttoi(strAttack);
        int iHp = _ttoi(strHp);

        // (4) UNITDATA 동적 할당, 맵에 삽입
        UNITDATA* pData = new UNITDATA;
        pData->strName = strUnitName;
        pData->iAttack = iAttack;
        pData->iHp = iHp;

        // key: "Category:UnitName"
        CString strKey;
        strKey.Format(_T("%s:%s"), strCategory, strUnitName);

        m_mapUnitData[strKey] = pData;
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

    // "카테고리|유닛이름|전체경로" 형태로 저장
    for (auto& pair : m_mapFilePaths)
    {
        // pair.first : "Monster:슬라임:Slime01.png"
        // pair.second: "D:\\Images\\Slime01.png" (절대 경로)
        CString strKey = pair.first;
        CString strPath = pair.second;

        // 콜론(:)을 기준으로 파싱
        int firstColon = strKey.Find(_T(":"));
        if (firstColon == -1)
            continue;
        int secondColon = strKey.Find(_T(":"), firstColon + 1);
        if (secondColon == -1)
            continue;

        CString strCategory = strKey.Left(firstColon);
        CString strUnitName = strKey.Mid(firstColon + 1, secondColon - (firstColon + 1));

        // [절대 경로] 그대로 저장 (기존 Convert_RelativePath 제거)
        // CString strRelativePath = Convert_RelativePath(strPath);
        // strLine.Format(_T("%s|%s|%s\n"), strCategory, strUnitName, strRelativePath);

        CString strLine;
        strLine.Format(_T("%s|%s|%s\n"), strCategory, strUnitName, strPath);
        file.WriteString(strLine);
    }

    file.Close();
}

// ----------------------------------------------------------------------
// LoadFileData는 그대로 둡니다. 파일에서 읽어온 Path를 그대로 CImage 로드.
//
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
    m_ListBox.ResetContent(); // 초기화만 유지

    CString strLine;
    while (file.ReadString(strLine))
    {
        int pos1 = strLine.Find(_T("|"));
        if (pos1 == -1) continue;

        int pos2 = strLine.Find(_T("|"), pos1 + 1);
        if (pos2 == -1) continue;

        CString strCategory = strLine.Left(pos1);
        CString strName = strLine.Mid(pos1 + 1, pos2 - (pos1 + 1));
        CString strPath = strLine.Mid(pos2 + 1);

        CString strFileName = PathFindFileName(strPath);
        CString strKey;
        strKey.Format(_T("%s:%s:%s"), strCategory, strName, strFileName);

        // 이미지 로드
        CImage* pImg = new CImage();
        if (FAILED(pImg->Load(strPath)))
        {
            AfxMessageBox(_T("이미지 로드 실패: ") + strPath);
            delete pImg;
            continue;
        }
        m_mapPngImages[strKey] = pImg;
        m_mapFilePaths[strKey] = strPath;

        // 카테고리와 유닛 매핑
        m_mapCategory[strCategory][strName].push_back(strPath);

        // [REMOVED] 리스트 박스에 추가하지 않음
        // m_ListBox.AddString(strKey);
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


    auto& mapUnitNameToImages = m_mapCategory[strCategory];
    if (mapUnitNameToImages.find(m_strName) == mapUnitNameToImages.end())
    {
        mapUnitNameToImages[m_strName] = vector<CString>();
    }

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
    {
        // ...
        return;
    }

    CString strSelectedUnit;
    m_ListBox2.GetText(iIndex, strSelectedUnit);

    int iTypeIndex = m_ListBox3.GetCurSel();
    if (iTypeIndex == LB_ERR)
        return;

    CString strSelectedType;
    m_ListBox3.GetText(iTypeIndex, strSelectedType);

    m_ImagePaths.clear();
    m_ListBox.ResetContent();
    m_CurrentFrameIndex = 0;

    auto itType = m_mapCategory.find(strSelectedType);
    if (itType != m_mapCategory.end())
    {
        auto itUnit = itType->second.find(strSelectedUnit);
        if (itUnit != itType->second.end())
        {
            for (auto& imagePath : itUnit->second)
            {
                // (1) fileName 만이 아니라 풀 키를 만들어서 저장하자
                //     ex) "Monster:슬라임:Slime01.png"
                CString fileName = PathFindFileName(imagePath);

                CString fullKey;
                fullKey.Format(_T("%s:%s:%s"), strSelectedType, strSelectedUnit, fileName);

                // 리스트박스에는 풀키를 AddString
                m_ListBox.AddString(fullKey);

                // m_ImagePaths 는 필요하다면 유지, 
                // 아니면 fullKey 대신 그냥 경로를 계속 저장
                m_ImagePaths.push_back(imagePath);
            }
        }
    }

    if (!m_ImagePaths.empty())
    {
        m_AnimationTimer = SetTimer(1, 100, NULL);
    }

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

void CUnitTool::OnStnClickedPicture()
{
    
}

CString CUnitTool::Convert_RelativePath(const CString& fullPath)
{
    TCHAR currentDir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentDir);

    CString relativePath = fullPath;
    PathRelativePathTo(relativePath.GetBuffer(MAX_PATH), currentDir, FILE_ATTRIBUTE_DIRECTORY, fullPath, FILE_ATTRIBUTE_NORMAL);
    relativePath.ReleaseBuffer();

    return relativePath;
}




void CUnitTool::OnBnClickedDeleteImage2()
{
    int iSel = m_ListBox.GetCurSel();
    if (iSel == LB_ERR)
    {
        AfxMessageBox(_T("삭제할 이미지를 선택하세요."));
        return;
    }

    CString strKey;
    m_ListBox.GetText(iSel, strKey); // "Monster:슬라임:Slime01.png" 식의 풀키라고 가정

    // (1) m_mapPngImages 에서 제거
    auto itImage = m_mapPngImages.find(strKey);
    if (itImage != m_mapPngImages.end())
    {
        itImage->second->Destroy();
        delete itImage->second;
        m_mapPngImages.erase(itImage);
    }

    // (2) m_mapFilePaths 에서 제거
    auto itPath = m_mapFilePaths.find(strKey);
    if (itPath != m_mapFilePaths.end())
        m_mapFilePaths.erase(itPath);

    // (3) m_mapCategory
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

    // (4) 리스트박스에서 제거
    m_ListBox.DeleteString(iSel);

    // (5) m_ImagePaths에서도 제거 (이미 '이미지 재생' 목록을 갱신한다면)
    auto itVec2 = std::find_if(m_ImagePaths.begin(), m_ImagePaths.end(), [&](const CString& path) {
        return PathFindFileName(path) == strFilename;
        });
    if (itVec2 != m_ImagePaths.end())
        m_ImagePaths.erase(itVec2);

    // (6) 애니메이션 관련 처리 (필요시)
    if (m_ImagePaths.empty())
    {
        KillTimer(m_AnimationTimer);
        m_AnimationTimer = 0;
        CClientDC dc(&m_Picture);
        CRect rect;
        m_Picture.GetClientRect(&rect);
        dc.FillSolidRect(rect, RGB(255, 255, 255));
    }

    AfxMessageBox(_T("이미지가 삭제되었습니다."));
}



void CUnitTool::OnBnClickedPause()
{
    if (m_AnimationTimer != 0)
    {
        KillTimer(m_AnimationTimer); // 타이머 중지
        m_AnimationTimer = 0; // 타이머 ID 초기화
        AfxMessageBox(_T("애니메이션이 일시 정지되었습니다."));
    }
    else
    {
        // 현재 프레임을 기반으로 이미지를 다시 렌더링
        if (!m_ImagePaths.empty())
        {
            CString currentImagePath = m_ImagePaths[m_CurrentFrameIndex];

            CImage image;
            if (FAILED(image.Load(currentImagePath)))
            {
                AfxMessageBox(_T("이미지를 로드할 수 없습니다."));
                return;
            }

            CClientDC dc(&m_Picture);
            CRect rect;
            m_Picture.GetClientRect(&rect);

            dc.FillSolidRect(rect, RGB(255, 255, 255));
            image.Draw(dc, rect);
        }
     

        m_AnimationTimer = SetTimer(1, 100, NULL); // 다시 타이머 시작
        AfxMessageBox(_T("애니메이션이 재개되었습니다."));
    }
}





void CUnitTool::OnBnClickedOk()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CDialog::OnOK();
}






void CUnitTool::OnStnClickedevasion()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
