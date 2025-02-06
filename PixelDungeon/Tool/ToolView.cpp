// ToolView.cpp: CToolView 클래스의 구현
#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Tool.h"
#endif

#include "ToolDoc.h"
#include "ToolView.h"
#include "CDevice.h"
#include "CTextureMgr.h"
#include "MainFrm.h"
#include "CTerrain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HWND	g_hWnd;




// CToolView
IMPLEMENT_DYNCREATE(CToolView, CScrollView)

BEGIN_MESSAGE_MAP(CToolView, CScrollView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_WM_DESTROY()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
//	ON_WM_MOUSEHWHEEL()
ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CToolView 생성/소멸

CToolView::CToolView() noexcept
	: m_pDevice(CDevice::Get_Instance()),
	m_pTerrain(nullptr),
	m_fZoom(0.f),
	m_byDrawID(0),
	m_byOption(0)
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CToolView::~CToolView()
{
}

void CToolView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	m_fZoom = 1.f;

	m_byDrawID = 1;

	// SetScrollSizes : 스크롤 바의 사이즈를 지정
	// MM_TEXT : 픽셀 단위로 조정하겠다는 옵션

	SetScrollSizes(MM_TEXT, CSize(TILECX * TILEX, TILECY * TILEY / 2));


	// AfxGetMainWnd : 현재 메인 윈도우의 값을 반환하는 전역함수

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrm)
	{
		CUnitTool* pUnitTool = (CUnitTool*)pMainFrm->GetDlgItem(IDD_CUnitTool);
		if (pUnitTool)
		{
			pUnitTool->SetToolView(this);
		}
	}
	RECT rcWnd{};

	// GetWindowRect : 현재 윈도우(창)의 rect 정보를 얻어오는 함수
	pMainFrm->GetWindowRect(&rcWnd);

	// SetRect : 지정한 매개 변수 대로 Rect 정보를 기록하는 함수

	SetRect(&rcWnd, 0, 0, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top);

	// rcWnd.right : 창의 가로 사이즈
	// rcWnd.bottom : 창의 세로 사이즈

	RECT rcMainView{};

	// GetClientRect : 현재 View창의 정보를 Rect 형식으로 얻어오는 함수
	GetClientRect(&rcMainView);

	float	fRowFrm = float(rcWnd.right - rcMainView.right);
	float	fColFrm = float(rcWnd.bottom - rcMainView.bottom);

	// SetWindowPos : 매개 변수대로 윈도우 위치와 크기를 조정하는 함수
	// SetWindowPos(배치할 윈도우의 Z순서에 대한 포인터, X좌표, Y좌표, 가로 크기, 세로 크기, 배치할 윈도우의 크기 및 위치지정 옵션)
	
	pMainFrm->SetWindowPos(nullptr,		// 순서 변경하지 않음
							0, 
							0,
							int(WINCX + fRowFrm), 
							int(WINCY + fColFrm), 
							SWP_NOZORDER); // 현재 순서를 유지함


	g_hWnd = m_hWnd;

	if (FAILED(CDevice::Get_Instance()->Init_Device()))
	{
		AfxMessageBox(L"Init_Device Create Failed");
		return;
	}
	
	m_pTerrain = new CTerrain;
	if (FAILED(m_pTerrain->Initialize()))
	{
		AfxMessageBox(L"m_pTerrain Create Failed");
		return;
	}

	m_pTerrain->Set_MainView(this);

	if (FAILED(CTextureMgr::Get_Instance()->Read_ImgPath(L"../Data/ImgPath.txt")))
	{
		ERR_MSG(L"ImgPath Read Failed");
	}

}

void CToolView::DisplayImage(const CString& strKey)
{
	CUnitTool* pUnitTool = (CUnitTool*)AfxGetMainWnd()->GetDlgItem(IDD_CUnitTool);
	if (!pUnitTool)
	{
		AfxMessageBox(_T("CUnitTool을 찾을 수 없습니다!"));
		return;
	}

	CString strImagePath = pUnitTool->GetUnitImagePath(strKey);
	if (strImagePath.IsEmpty())
	{
		AfxMessageBox(_T("이미지 경로를 찾을 수 없습니다: ") + strKey);
		return;
	}

	CImage image;
	if (FAILED(image.Load(strImagePath)))
	{
		AfxMessageBox(_T("이미지 로드 실패: ") + strImagePath);
		return;
	}

	CClientDC dc(this);
	CRect rect(10, 10, 10 + image.GetWidth(), 10 + image.GetHeight());
	dc.FillSolidRect(rect, RGB(255, 255, 255)); // 배경 초기화

	image.Draw(dc, rect);
}


void CToolView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (g_pUnitTool)
	{
		CString strSelectedUnit = g_pUnitTool->GetSelectedUnit();
		CString strSelectedCategory = g_pUnitTool->GetSelectedCategory();

		TRACE(_T("[디버깅] 선택된 유닛: %s, 선택된 카테고리: %s\n"), strSelectedUnit, strSelectedCategory);

		if (!strSelectedUnit.IsEmpty() && !strSelectedCategory.IsEmpty())
		{
			TRACE(_T("[디버깅] CreateUnit() 호출됨!\n"));
			CreateUnit(strSelectedCategory, strSelectedUnit, point);
		}
		else
		{
			AfxMessageBox(_T("선택된 유닛이나 카테고리가 없습니다!"));
		}
	}
	else
	{
		AfxMessageBox(_T("CUnitTool이 존재하지 않습니다!"));
	}

	CView::OnRButtonDown(nFlags, point);
}

void CToolView::CreateUnit(const CString& strCategory, const CString& strUnitName, CPoint point)
{
	// ► '유닛 이름' 대신 '카테고리'를 검사
	if (strCategory.CompareNoCase(_T("Player")) == 0)
	{
		// 이미 Player 카테고리인 유닛이 존재하는지 검사
		for (const auto& unit : m_vecUnits)
		{
			if (unit.strCategory.CompareNoCase(_T("Player")) == 0)
			{
				AfxMessageBox(_T("플레이어는 하나만 생성할 수 있습니다!!!!!!"));
				return;
			}
		}
	}

	// --- 이하 동일 ---
	float fZoom = Get_Zoom();
	CPoint adjustedPoint;
	adjustedPoint.x = int((point.x + GetScrollPos(0)) / fZoom);
	adjustedPoint.y = int((point.y + GetScrollPos(1)) / fZoom);

	CString strImagePath = g_pUnitTool->GetUnitImagePath(strCategory, strUnitName);
	if (!strImagePath.IsEmpty())
	{
		m_vecUnits.push_back({ strCategory, strUnitName, adjustedPoint });
		Invalidate(FALSE); // 화면 갱신
	}
}



void CToolView::OnMouseMove(UINT nFlags, CPoint point)
{
	CScrollView::OnMouseMove(nFlags, point);

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		Change_Tile(point);
	}


	Invalidate(FALSE);
}



void CToolView::OnDraw(CDC* /*pDC*/)
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;


	m_pDevice->Render_Begin();
	m_pTerrain->Render();
	
	CClientDC dc(this);

	float fZoom = Get_Zoom(); // 현재 줌 값 가져오기
	int iScrollX = GetScrollPos(0); // X축 스크롤 위치
	int iScrollY = GetScrollPos(1); // Y축 스크롤 위치

	for (const auto& unit : m_vecUnits)
	{
		CString strImagePath = g_pUnitTool->GetUnitImagePath(unit.strCategory, unit.strUnitName);

		if (!strImagePath.IsEmpty())
		{
			CImage image;
			if (SUCCEEDED(image.Load(strImagePath)))
			{
	
				int iX = int((unit.position.x - iScrollX) * fZoom);
				int iY = int((unit.position.y - iScrollY) * fZoom);
				int iWidth = int(image.GetWidth() * fZoom);
				int iHeight = int(image.GetHeight() * fZoom);

				
				//CDevice::Get_Instance()->Get_Device();
				CRect rect(iX, iY, iX + iWidth, iY + iHeight);
				image.StretchBlt(dc, rect, SRCCOPY);
			}
		}
	}


	m_pDevice->Render_End();


}





void CToolView::OnDestroy()
{
	CScrollView::OnDestroy();

	Safe_Delete(m_pTerrain);

	CTextureMgr::Destroy_Instance();
	m_pDevice->Destroy_Instance();

}


#pragma region 생략
BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CScrollView::PreCreateWindow(cs);
}

// CToolView 인쇄

BOOL CToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CToolView 진단

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CToolDoc* CToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CToolView 메시지 처리기

#pragma endregion


void CToolView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ADD) // VK_ADD는 키패드의 '+' 키
	{
		m_fZoom += 0.5f;
	}

	if (nChar == VK_SUBTRACT) // VK_ADD는 키패드의 '+' 키
	{
		m_fZoom -= 0.5f;
	}

	Invalidate(FALSE);

	CView::OnKeyDown(nChar, nRepCnt, nFlags); // 기본 처리 호출
}

void CToolView::Change_Tile(CPoint _point)
{
	m_pTerrain->Tile_Change(D3DXVECTOR3(float((_point.x) + GetScrollPos(0)) / Get_Zoom(),
		float((_point.y) + GetScrollPos(1)) / Get_Zoom(),
		0.f), m_byDrawID, m_byOption);

	// Invalidate : 호출 시 윈도우에 WM_PAINT와 WM_ERASEBKGND 메세지를 발생시킴
	// WM_PAINT 메세지 발생 시, OnDraw함수가 다시 호출

	// false : WM_PAINT 메세지만 발생
	// true : WM_PAINT, WM_ERASEBKGND 둘 다 메세지 발생

	Invalidate(FALSE);

	// AfxGetMainWnd : 현재 쓰레드로부터 Wnd 반환
	// 현재 쓰레드가 메인 쓰레드일 경우에 정상적인 동작
	//CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();

	// AfxGetApp : 메인 쓰레드가 갖고 있는 현재 메인 app을 반환
	// CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd())

	// GetParentFrame : 현재 View창을 둘러싸고 있는 상위 FrameWnd
	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(GetParentFrame());
	CMiniView* pMiniView = dynamic_cast<CMiniView*>(pMainFrm->m_SecondSplitter.GetPane(0, 0));

	pMiniView->Invalidate(FALSE);
}

BOOL CToolView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta > 0)
	{
	
		//휠 업 (위로 스크롤)
		m_fZoom += 0.1f;
	}
	else if (zDelta < 0)
	{
		//휠 다운 (아래로 스크롤)
		m_fZoom -= 0.1f;
	}

	Invalidate(FALSE);

	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}
