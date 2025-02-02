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
public:
    CString m_strName;  // 유닛 이름
    int m_iAttack;      // 공격력
    int m_iHp;          // 체력
    int m_iLevel;       // 레벨 ⭐ 추가
    float m_fAttackSpeed; // 공격 속도 ⭐ 추가
    int m_iLuck;        // 운 ⭐ 추가
    float m_fMoveSpeed; // 이동 속도 ⭐ 추가
    float m_fEvasion;   // 회피력 ⭐ 추가


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
;
    afx_msg void OnListBox();
    afx_msg void OnDestroy();


    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedButton9();
    afx_msg void OnBnClickedButton10();
    afx_msg void OnBnClickedRadioNpc();
    afx_msg void OnBnClickedRadioMonster();
    afx_msg void OnBnClickedRadioPlayer();
    afx_msg void OnBnClickedButtonDelete();


    afx_msg BOOL PreTranslateMessage(MSG* pMsg);

    afx_msg void OnBnClickedButtonApply(); // 유닛 생성 버튼

    //캐릭터 정보
    void SaveUnitData(const CString& strFilePath);
    void LoadUnitData(const CString& strFilePath);
    // 이미지 입출력
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
    CAnimateCtrl m_Animate_1;


    private:

        vector<CString> m_ImagePaths;    
        size_t m_CurrentFrameIndex;         
        UINT_PTR m_AnimationTimer;            
public:
    afx_msg void OnBnClickedDeleteImage2();
    afx_msg void OnBnClickedPause();
 


};
