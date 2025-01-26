#pragma once

#include "afxdialogex.h"
#include "Include.h"

#define RUBY        0x01
#define DIAMOND     0x02
#define SAPPHIRE    0x04

struct IMAGE_ITEM
{
    CString strCategory; // 예: "Player", "Monster", "NPC"
    CString strName;     // 실제 파일 이름(확장자 제외)
    CImage* pImage;
};
class CUnitTool : public CDialog
{
    DECLARE_DYNAMIC(CUnitTool)

public:
    CUnitTool(CWnd* pParent = nullptr);
    virtual ~CUnitTool();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_CUnitTool };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()

private:
    // 내부 사용: 애니메이션
    vector<HBITMAP> m_vecBitmaps;    // 애니메이션 프레임
    int             m_iCurrentFrame; // 현재 프레임 인덱스

    // PNG 이미지와 경로를 저장하는 맵
    map<CString, CImage*>  m_mapPngImages;
    map<CString, CString>  m_mapFilePaths;
    map<CString, map<CString, vector<CString>>> m_mapCategory;

//예: m_mapCategory["Monster"]["신승훈"] 신승훈이 가진 여러 이미지 경로를 vector<CString> 형태로 저장
//LoadFileData에서 타입 | 유닛이름 | 이미지경로를 읽어올 때마다 m_mapCategory[타입][유닛이름]에 이미지경로를 push_back 
//OnInitDialog(또는 적절한 초기화 시점)에서 IDC_LIST3 에 “Monster”, “NPC”, “Player” 같은 타입들을 추가한다.
//만약 파일로부터 동적으로 타입이 정해진다면, m_mapCategory에 있는 key값(타입)을 반복문으로 넣어주면 된다.
//IDC_LIST3(타입 목록)에서 SelChange가 일어날 때(ON_LBN_SELCHANGE 등) :
//
//    선택된 타입을 얻어온다.
//    IDC_LIST2를 ResetContent()하고, m_mapCategory[선택된타입]에 들어 있는 유닛 이름들을 넣어줄꺼임
//    IDC_LIST2(유닛 목록)에서 SelChange가 일어날 때 선택된 유닛 이름을 얻어온다.
//IDC_LIST1를 ResetContent()하고, 그 유닛이 가지는 모든 이미지 경로를 하나씩 리스트박스에 추가한다.
//IDC_LIST1(이미지 목록)에서 SelChange 또는 더블클릭(LBN_DBLCLK 등) :
//    선택된 이미지 경로를 얻어온다.
//    그 경로로부터 CImage 또는 Bitmap을 로드하여 Picture Control에 표시한다.
//    (이미 map< CString, CImage* > 형태로 관리하고 있다면, 경로 키를 이용해 이미지를 찾아서 그려줄 수도 있다.)


public:
    CString m_strName;       // 유닛 이름
    int     m_iAttack;       // 공격력
    int     m_iHp;           // 체력
    int     m_iMaxHp;        // 최대체력 (사용하신다면)
    float   m_iEvasion;      // 회피율 (사용하신다면)
    CString m_strFindName;   // 검색 키ㄹ워드

    CListBox  m_ListBox;     // 리스트박스 ( 이미지 출력하는곳 )
    CListBox m_ListBox2;     // 리스트박스2 ( 객체들 출력하는곳 )
    CListBox m_ListBox3;     // 리스트박스3 ( 무슨 객체를 출력할지 선택하는곳)
    CButton   m_Radio[3];    // 플레이어/몬스터/NPC 라디오 버튼
    CButton   m_Check[3];    // 체크박스 예시 (쓰신다면)
    CButton   m_Bitmap;      // 비트맵 버튼 (사용하신다면)

    CStatic   m_AnimationCtrl; // 애니메이션용 Static
    CStatic   m_Picture;       // Picture Control

    // 유닛 데이터 관리
    map<CString, UNITDATA*> m_mapUnitData;

    // 현재 선택된 라디오 버튼 인덱스 (0: Player, 1: Monster, 2: NPC)
    int m_iCategorySelect;

    // 이벤트 핸들러
    virtual BOOL OnInitDialog();
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnSearch();
    afx_msg void OnListBox();
    afx_msg void OnDestroy();
    void SaveUnitData(const CString& strFilePath);

    void LoadUnitData(const CString& strFilePath);

    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedButton9();
    afx_msg void OnBnClickedButton10();
    afx_msg void OnBnClickedRadioNpc();
    afx_msg void OnBnClickedRadioMonster();
    afx_msg void OnBnClickedRadioPlayer();
    afx_msg void OnBnClickedButtonApply();
    afx_msg void OnBnClickedButtonDelete();

    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

    afx_msg BOOL PreTranslateMessage(MSG* pMsg);

    // 파일 입출력
    void SaveFileData(const CString& strFilePath);
    void LoadFileData(const CString& strFilePath);

    //이미지 관련
    void DisplayImage(const CString& strName);


    // 애니메이션
    void StartAnimation();



    afx_msg void OnLbnDblclkList2();
    afx_msg void OnLbnDblclkList3();
    afx_msg void OnStnClickedPicture();
    afx_msg CString Convert_RelativePath(const CString& fullPath);
};
