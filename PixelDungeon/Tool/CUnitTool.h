#pragma once

#include "afxdialogex.h"
#include "Include.h"

// 아이템 비트 플래그 예시
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

public:
    // MFC UI 연결 멤버
    CString m_strName;       // 유닛 이름
    int     m_iAttack;       // 공격력
    int     m_iHp;           // 체력
    int     m_iMaxHp;        // 최대체력 (사용하신다면)
    float   m_iEvasion;      // 회피율 (사용하신다면)
    CString m_strFindName;   // 검색 키워드

    CListBox  m_ListBox;     // 리스트박스
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
    afx_msg void OnSave();
    afx_msg void OnLoad();
    afx_msg void OnSearch();
    afx_msg void OnListBox();
    afx_msg void OnDestroy();
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
};
