#pragma once

#include "afxdialogex.h"
#include "Include.h"
#include "ToolView.h"

#define RUBY        0x01
#define DIAMOND     0x02
#define SAPPHIRE    0x04

struct IMAGE_ITEM
{
    CString strCategory; // 예: "Player", "Monster", "NPC"
    CString strName;     // 실제 파일 이름(확장자 제외)
    CImage* pImage;
};

class CUnitTool;

extern CUnitTool* g_pUnitTool;

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
    map<CString, CImage*,greater<CString>>  m_mapPngImages;
    map<CString, CString>  m_mapFilePaths;
    map<CString, map<CString, vector<CString>>> m_mapCategory;
public:
    CString m_strName;  // 유닛 이름
    int m_iAttack;      // 공격력
    int m_iHp;          // 체력
    int m_iLevel;       // 레벨 
    float m_fAttackSpeed; // 공격 속도 
    int m_iLuck;        // 운 
    float m_fMoveSpeed; // 이동 속도 
    float m_fEvasion;   // 회피력 


    CListBox  m_ListBox;     // 리스트박스 ( 이미지 출력하는곳 )
    CListBox m_ListBox2;     // 리스트박스2 ( 객체들 출력하는곳 )
    CListBox m_ListBox3;     // 리스트박스3 ( 무슨 객체를 출력할지 선택하는곳)
    CButton   m_Radio[3];    // 플레이어/몬스터/NPC 라디오 버튼
    CButton   m_Check[3];   
 

    CStatic   m_AnimationCtrl; // 애니메이션용 Static
    CStatic   m_Picture;       // Picture Control

    // 유닛 데이터 관리
    map<CString, UNITDATA*> m_mapUnitData;

    // 현재 선택된 라디오 버튼 인덱스 (0: Player, 1: Monster, 2: NPC)
    int m_iCategorySelect;

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

    //이미지 불러오깅
    void SaveFileData(const CString& strFilePath);
    void LoadFileData(const CString& strFilePath);

    // 애니메이션
    void DisplayImage(const CString& strName);
    void StartAnimation();


    
    afx_msg void OnLbnDblclkList2(); // 생성된 유닛들
    afx_msg void OnLbnDblclkList3(); // 유닛들 타입 모아놓는곳

  private:

        vector<CString> m_ImagePaths;    
        size_t m_CurrentFrameIndex;         
        UINT_PTR m_AnimationTimer;            
public:
    afx_msg void OnBnClickedDeleteImage2();
    afx_msg void OnBnClickedPause();

    CString ConvertToRelativePath(const CString& fullPath);

    CString ConvertToAbsolutePath(const CString& relativePath);

   void OnLbnSelchangeList2();

   CString GetUnitImagePath(const CString& strKey);

    CString m_strSelectedImagePath;  // 선택된 유닛의 이미지 저장용!!
 
    public:
        CString m_strSelectedUnit; // 현재 선택된 유닛 이름
        CString m_strSelectedCategory; // 현재 선택된 카테고리 (Player, Monster, NPC)
        void SetToolView(CView* pView) { m_pToolView = pView; }
public:
        CString GetSelectedUnit() const { return m_strSelectedUnit; }
        CString GetSelectedCategory() const { return m_strSelectedCategory;  }
       private:
            CView* m_pToolView;
  public:
            CString CUnitTool::GetUnitImagePath(const CString& strCategory, const CString& strUnitName)
            {
                auto itCategory = m_mapCategory.find(strCategory);
                if (itCategory != m_mapCategory.end())
                {
                    auto itUnit = itCategory->second.find(strUnitName);
                    if (itUnit != itCategory->second.end() && !itUnit->second.empty())
                    {
                        return itUnit->second[0]; // 첫 번째 이미지 경로 반환
                    }
                }
                return _T("");
            }

            
           

};
