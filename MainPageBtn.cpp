//==========================================================//
//文件名:   MainPageBtn.cpp
//文件描述:	首页展开按钮									 
//时间:     2011/05/25		
//作者:		周康						 
//==========================================================//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "StdAfx.h"
#include "MainPageBtn.h"
#include "swc7cidlist.h"
#include "LogicalStruct/MainPageLogicalStruct.h"
#include "GlobalEnvironment/GlobalEnv.h"
#include "ResourceFactory/IBitmapFactory.h"
#include "ElementMsgId/listcmd.h"
#include "msglist.h"
#include "FrameBase\IElementEventProcessor.h"
#include "libmarkup/markup.h"
#include "util.h"
#include "businesselementcommon\elementeventinfodef.h"
#include "BaseMisc\StringParamReader.h"
#include "BusinessElementCommon\Utilcom.h"
#include "BaseMisc\MemDC.h"
#include "BaseMisc\SWPalette.h"
#include "commandlist.h"

#define BITMAP_FOLDER_NAME		_T("style_zoom_bar")	//bitmap文件夹

#define SCREEN_CX_1024			1024				//1024的屏幕x方向分辨率

#define PROCESS_MAINBTN_PARAM \
			_T("<params>")\
			_T("<") STR_EXPAND_RIGHT_PARAM_NAME _T(">") _T("%d") _T("</") STR_EXPAND_RIGHT_PARAM_NAME _T(">")\
			_T("</params>")

#define PAGE_BTN_TYPE1	_T("MAINPAGE")
#define PAGE_BTN_TYPE2	_T("OTHER_TYPE")
#define PAGE_BTN_TYPE3	_T("COMMON")

CMainPageBtn::CMainPageBtn(void)
{
	m_bExpandRight = FALSE;
	m_bExpandBottom = FALSE;
	m_ePageBtnType = PAGE_BTN_TYPE_NOTHING;
}

CMainPageBtn::~CMainPageBtn(void)
{
}

BOOL CMainPageBtn::OnPaint( CPaintDC *pDC )
{
	CRect rRect;
	GetDrawRect(rRect);

	// 获取重画区域
	CRect rClip;
	pDC->GetClipBox(rClip);

	// 检查该区域是否需要重画
	if ((rClip & rRect) == CRect(0,0,0,0))
		return TRUE;

	CString strCopy;

	//画箭头
	if (rRect.Height() > rRect.Width())
	{ 
		if (m_bExpandRight)
		{
			if (m_bMouseOn)
			{
				strCopy = _T("style_zoom_bar.right_arrow_right_on"); 
			}
			else
			{
				strCopy = _T("style_zoom_bar.right_arrow_right_off");
			}
		}
		else
		{
			if (m_bMouseOn)
			{
				strCopy = _T("style_zoom_bar.right_arrow_left_on");
			}
			else
			{
				strCopy = _T("style_zoom_bar.right_arrow_left_off");
			}
		}
	}
	else
	{
		if (m_bExpandBottom)
		{
			if (m_bMouseOn)
			{
				strCopy = _T("style_zoom_bar.bottom_arrow_down_on");
			}
			else
			{
				strCopy = _T("style_zoom_bar.bottom_arrow_down_off");
			}
		}
		else
		{
			if (m_bMouseOn)
			{
				strCopy = _T("style_zoom_bar.bottom_arrow_up_on");
			}
			else
			{
				strCopy = _T("style_zoom_bar.bottom_arrow_up_off");
			}
		}
	}

	CMemDC dcMem(pDC);
	dcMem.FillSolidRect(rRect, m_colorBack);

	CDC	MemDC;
	MemDC.CreateCompatibleDC(&dcMem);

	CBitmap	*pBitMap = NULL,*pOldBitMap = NULL;
	BITMAP	BitMap;

	DECLARE_SW_PALETTE(spBitMapMgr);
	ASSERT_RESULT(spBitMapMgr != NULL);

	if ( PAGE_BTN_TYPE_COMMON == m_ePageBtnType )
	{
		strCopy.Insert( 0, _T("trade.") );
	}
	pBitMap = spBitMapMgr->GetBitmap (strCopy);
	ASSERT_RESULT(pBitMap != NULL);
	pOldBitMap = MemDC.SelectObject(pBitMap);
	pBitMap->GetBitmap(&BitMap);

	int nX = max(0,(rRect.Width() - BitMap.bmWidth)/2);
	int nY = max(0,(rRect.Height() - BitMap.bmHeight)/2);

	dcMem.BitBlt(nX,nY,BitMap.bmWidth,BitMap.bmHeight,&MemDC,0,0,SRCCOPY);
	MemDC.SelectObject(pOldBitMap);
	MemDC.DeleteDC();

	return TRUE;
}

BOOL CMainPageBtn::InitObject( LPCWSTR lpszParam, LPCONTAINER_INFO lpContainerInfo )
{
	__super::InitObject(lpszParam,lpContainerInfo);

	RegisterMessage( WM_PAINT );
	RegisterMessage(WM_LBUTTONDOWN);
	RegisterMessage( WM_MOUSEMOVE );
	RegisterMessage( WM_MOUSELEAVE );
	RegisterMessage (WM_KEYDOWN);
	RegisterMessage (WM_BROADCASTCMD);

	m_nCxScreen = ::GetSystemMetrics(SM_CXSCREEN);
	GetInitParam (lpszParam);
	m_bMouseOn = false;

	return TRUE;
}

void CMainPageBtn::GetInitParam (LPCWSTR lpszParam)
{
	PAGE_BTN_TYPE PageBtnType = PAGE_BTN_TYPE_NOTHING;
	CString strParam = lpszParam;
	bool bFind = false;

	const TCHAR * szSeps = L";";

	CStringParamReader StringParamReader;
	StringParamReader.SetParamString(strParam, szSeps);

	std::wstring strValue;
	CString str;

	bFind = StringParamReader.GetParam(_T("STYLE_TYPE"),strValue);
	if (bFind)
	{
		str = strValue.c_str();
		if (str.Compare (PAGE_BTN_TYPE1) == 0)
		{
			m_ePageBtnType = PAGE_BTN_TYPE_MAINPAGE;
		}
		else if (str.Compare (PAGE_BTN_TYPE2) == 0)
		{
			m_ePageBtnType = PAGE_BTN_TYPE_OTHER;
		}
		else if ( 0 == str.Compare( PAGE_BTN_TYPE3 ) )
		{
			m_ePageBtnType = PAGE_BTN_TYPE_COMMON;
		}
	}
	bFind = StringParamReader.GetParam(_T("EXPANDRIGHT_TYPE"),strValue);
	if (bFind)
	{
		str = strValue.c_str();
		m_bExpandRight = static_cast<BOOL> (_wtoi (str));
	}
	bFind = StringParamReader.GetParam(_T("EXPANDBOTTOM_TYPE"),strValue);
	if (bFind)
	{
		str = strValue.c_str();
		m_bExpandBottom = static_cast<BOOL> (_wtoi (str));
	}
	if ( PAGE_BTN_TYPE_COMMON == m_ePageBtnType )
	{
		m_colorBack = GetPaletteColor(_T("CombinedTrade.Separator"));
	} 
	else
	{
		m_colorBack = GetPaletteColor(_T("MainPage.Black"));
	}
}

LPCWSTR CMainPageBtn::GetUUID()
{
	return UUIDSTR_ELEMENT_MAINPAGEBTN;
}

BOOL CMainPageBtn::OnSize (UINT uType, INT cx, INT cy)
{
	if( m_ePageBtnType == PAGE_BTN_TYPE_MAINPAGE)
	{
		int nCxScreen = ::GetSystemMetrics(SM_CXSCREEN);
		m_bCommonScreen = (SCREEN_CX_1024 == nCxScreen)?TRUE:FALSE;
		if (m_bExpandRight && m_nCxScreen != nCxScreen)
		{
			m_bExpandRight = !m_bExpandRight;
			ActionEventProcessor (_T("click_showhide_leftlevel"), NULL);
		}
		m_nCxScreen = nCxScreen;
	}

	return TRUE;
}

BOOL CMainPageBtn::OnLButtonDown( POINT ptMouse )
{
	CRect ClientRc;
	GetContainerRect(ClientRc);
	CString strParam;
	if (ClientRc.Height() > ClientRc.Width())
	{ 
		m_bExpandRight = !m_bExpandRight;
		switch (m_ePageBtnType)
		{
		case PAGE_BTN_TYPE_MAINPAGE:
			{
				strParam.Format (_T("<params><%s>%d</%s></params>"),
					STR_SUBSTYLE_LAYOUT_CHANGE_PARAM_NAME, 1, STR_SUBSTYLE_LAYOUT_CHANGE_PARAM_NAME);
				ActionEventProcessor (_T("click_showhide_leftlevel"), strParam);
				SetMainpageStyle(!m_bExpandRight);
			}
			break;
		case PAGE_BTN_TYPE_OTHER:
			{
				strParam.Format (_T("<params><%s>%d</%s><%s>%d</%s></params>"),
					STR_NEWSWND_SHOWHIDE,m_bExpandRight,STR_NEWSWND_SHOWHIDE,
					STR_SUBSTYLE_LAYOUT_CHANGE_PARAM_NAME, 1, STR_SUBSTYLE_LAYOUT_CHANGE_PARAM_NAME);
				ActionEventProcessor (_T("click_showhide_rtk"), strParam);
			}
			break;
		case PAGE_BTN_TYPE_COMMON:
			{
				strParam.Format( _T("<params><%s>%d</%s></params>"),
					STR_SUBSTYLE_LAYOUT_CHANGE_PARAM_NAME, 1, STR_SUBSTYLE_LAYOUT_CHANGE_PARAM_NAME );
				ActionEventProcessor( _T("click_fold_button"), strParam );
			}
			break;
		default:
			break;
		}
	}
	else
	{
		m_bExpandBottom = !m_bExpandBottom;
		switch ( m_ePageBtnType )
		{
		case PAGE_BTN_TYPE_OTHER:
			{
				strParam.Format (_T("<params><%s>%d</%s><%s>%d</%s></params>"),
					STR_NEWSWND_SHOWHIDE,m_bExpandBottom,STR_NEWSWND_SHOWHIDE,
					STR_SUBSTYLE_LAYOUT_CHANGE_PARAM_NAME, 1, STR_SUBSTYLE_LAYOUT_CHANGE_PARAM_NAME);
				ActionEventProcessor (_T("click_showhide_newswnd"), strParam);
			}
			break;
		case PAGE_BTN_TYPE_COMMON:
			{
				strParam.Format( _T("<params><%s>%d</%s></params>"),
					STR_SUBSTYLE_LAYOUT_CHANGE_PARAM_NAME, 1, STR_SUBSTYLE_LAYOUT_CHANGE_PARAM_NAME );
				ActionEventProcessor( _T("click_fold_button"), strParam );
			}
			break;
		default:
			break;
		}
	}

	SkipMsg ();
	return __super::OnLButtonDown(ptMouse);
}

BOOL CMainPageBtn::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if (!IsFocus())
		return TRUE;

	switch (nChar)
	{
	case VK_UP:
	case VK_DOWN:
	case VK_PRIOR:
	case VK_NEXT:
	case VK_LEFT:
	case VK_RIGHT:
		break;
	default:
		return CBusinessBaseContainer::OnKeyDown (nChar, nRepCnt, nFlags);
	}

	return TRUE;
}

void CMainPageBtn::ActionEventProcessor (LPCWSTR lpszOperation, LPCWSTR lpszParam)
{
	DECLARE_IELEMENT_EVENT_PROCESSOR (spEvent);
	ASSERT (NULL != spEvent);

	spEvent->EventProcessor (GetContainerName(), lpszOperation, lpszParam);
}

BOOL CMainPageBtn::OnMouseMove( INT x, INT y, DWORD dwState )
{
	if (!m_bTracking) 
	{ 
		TRACKMOUSEEVENT tme; 
		tme.cbSize = sizeof(tme); 
		tme.hwndTrack = GetVMWindow(); 
		tme.dwFlags = TME_LEAVE; 
		tme.dwHoverTime = 500; 
		m_bTracking = _TrackMouseEvent(&tme); 
	}

	CRect rClient;
	GetDrawRect(rClient);
	CPoint point(x,y);
	if (rClient.PtInRect(point))
	{
		m_bMouseOn = true;
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
	}
	else
	{
		m_bMouseOn = false;
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}
	
	InvalidContainerRect();

	return TRUE;
}
BOOL CMainPageBtn::OnMouseLeave()
{
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	m_bMouseOn = false;
	m_bTracking = FALSE;
	InvalidContainerRect();
	return TRUE;
}

BOOL CMainPageBtn::OnUserMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	CRect ClientRc;
	GetContainerRect(ClientRc);

	switch (uMsg)
	{
	case WM_ELEMENT_EVENT:
		{
			DECLARE_GLOBALENV( spGlobalEnv );

			IUnknownPtr pParam = spGlobalEnv->GlobalGet (static_cast<DWORD> (lParam));
			DECLARE_SMART_OBJECT_NOSET (_ELEMENT_EVENT_INFO, spElementEventInfo);

			spElementEventInfo = CUnknown::dynamic_runtime_cast<CPackager<_ELEMENT_EVENT_INFO>> (pParam);
			ASSERT( spElementEventInfo );

			if (wcscmp (spElementEventInfo->Get ().lpszFuncName, _T("SetParam")) == 0 && 
				wcscmp (spElementEventInfo->Get ().lpszFuncParam, _T("Type=Both")) == 0)
			{
				std::wstring szParam = this->GetString (_T("EXPANDRIGHT_TYPE"));
				if (TRUE == static_cast<BOOL> (_wtoi (szParam.c_str ())))	//展开
				{
					m_bExpandRight = TRUE;
				}
				else
				{
					m_bExpandRight = FALSE;
				}

				szParam = this->GetString (_T("EXPANDBOTTOM_TYPE"));
				if (TRUE == static_cast<BOOL> (_wtoi (szParam.c_str ())))	//展开
				{
					m_bExpandBottom = TRUE;
				}
				else
				{
					m_bExpandBottom = FALSE;
				}
			}
			else if (wcscmp (spElementEventInfo->Get ().lpszFuncName, _T("NewsParam")) == 0 && 
				wcscmp (spElementEventInfo->Get ().lpszFuncParam, _T("Type=ShowHide")) == 0)
			{
				std::wstring szParam = this->GetString (_T("EXPANDRIGHT_TYPE"));
				if (TRUE == static_cast<BOOL> (_wtoi (szParam.c_str ())))	//展开
				{
					m_bExpandRight = TRUE;
				}
				else
				{
					m_bExpandRight = FALSE;
				}
				szParam = this->GetString (_T("EXPANDBOTTOM_TYPE"));
				if (TRUE == static_cast<BOOL> (_wtoi (szParam.c_str ())))	//展开
				{
					m_bExpandBottom = TRUE;
				}
				else
				{
					m_bExpandBottom = FALSE;
				}
			}
			else if (wcscmp (spElementEventInfo->Get ().lpszFuncName, _T("SetParam")) == 0 && 
				wcscmp (spElementEventInfo->Get ().lpszFuncParam, _T("Type=leftlevel")) == 0)
			{
				std::wstring szParam = this->GetString (_T("EXPANDRIGHT_TYPE"));
				if (TRUE == static_cast<BOOL> (_wtoi (szParam.c_str ())))	//展开
				{
					m_bExpandRight = TRUE;
				}
				else
				{
					m_bExpandRight = FALSE;
				}
				szParam = this->GetString (_T("EXPANDBOTTOM_TYPE"));
				if (TRUE == static_cast<BOOL> (_wtoi (szParam.c_str ())))	//展开
				{
					m_bExpandBottom = TRUE;
				}
				else
				{
					m_bExpandBottom = FALSE;
				}
			}
			InvalidContainerRect();
		}
		break;
		default:
			break;
	}
	return TRUE;
}

BOOL CMainPageBtn::OnBroadcastCommand (WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case SCMD_CLIENT_SKIN_COLOR_CHANGED: //切换白皮肤处理消息
		{
			if ( PAGE_BTN_TYPE_COMMON == m_ePageBtnType )
			{
				m_colorBack = GetPaletteColor(_T("CombinedTrade.Separator"));
			} 
			else
			{
				m_colorBack = GetPaletteColor(_T("MainPage.Black"));
			}
			InvalidContainerRect();
			break;
		}
	default:
		break;
	}

	return TRUE;
}