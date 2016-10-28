/*
 * WebView.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: lion
 */

#ifdef _WIN32

#include "object.h"
#include "ifs/gui.h"
#include "WebView.h"
#include "utf8.h"
#include <exlib/include/thread.h>

namespace fibjs
{

DECLARE_MODULE(gui);

static exlib::LockedList<AsyncEvent> s_uiPool;
static HANDLE s_sem;

void putGuiPool(AsyncEvent* ac)
{
	s_uiPool.putTail(ac);
	ReleaseSemaphore(s_sem, 1, NULL);
}

class gui_thread : public exlib::OSThread
{
public:
	virtual void Run()
	{
		OleInitialize(NULL);

		while (true)
		{
			if (MsgWaitForMultipleObjects(1, &s_sem, FALSE, INFINITE, QS_ALLEVENTS) == WAIT_OBJECT_0)
			{
				AsyncEvent *p = s_uiPool.getHead();
				p->invoke();
			} else
			{
				MSG msg;
				if (GetMessage(&msg, NULL, 0, 0))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
	}
};

void init_gui()
{
	s_sem = ::CreateSemaphore(NULL, 0, LONG_MAX, NULL);
	(new gui_thread())->start();
}

result_t gui_base::open(exlib::string url, exlib::string title,
                        obj_ptr<WebView_base>& retVal, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_GUICALL);

	retVal = new WebView(url, title);

	return 0;
}

const wchar_t* szWndClassMain = L"fibjs_window";

static void RegMainClass()
{
	static bool s_init = false;

	if (!s_init)
	{
		s_init = true;

		WNDCLASSW wc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = sizeof(void*);
		wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hIcon = NULL;
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpfnWndProc = WebView::WndProc;
		wc.lpszClassName = szWndClassMain;
		wc.lpszMenuName = NULL;
		wc.style = 0;

		RegisterClassW(&wc);
	}
}

WebView::WebView(exlib::string url, exlib::string title)
{
	oleObject = NULL;
	oleInPlaceObject = NULL;
	webBrowser2 = NULL;

	RegMainClass();

	hWndParent = CreateWindowExW(0, szWndClassMain,
	                             (LPCWSTR)UTF8_W(title),
	                             WS_OVERLAPPEDWINDOW,
	                             CW_USEDEFAULT, CW_USEDEFAULT,
	                             CW_USEDEFAULT, CW_USEDEFAULT,
	                             NULL, NULL, GetModuleHandle(NULL),
	                             NULL);
	ShowWindow(hWndParent, SW_SHOW);

	::SetRect(&rObject, -300, -300, 300, 300);

	SetWindowLongPtr(hWndParent, 0, (LONG_PTR)this);
	AddRef();

	::OleCreate(CLSID_WebBrowser,
	            IID_IOleObject, OLERENDER_DRAW, 0, this, this,
	            (void**)&oleObject);

	oleObject->SetClientSite(this);
	OleSetContainedObject(oleObject, TRUE);

	RECT posRect;
	::SetRect(&posRect, -300, -300, 300, 300);
	oleObject->DoVerb(OLEIVERB_INPLACEACTIVATE,
	                  NULL, this, -1, hWndParent, &posRect);
	oleObject->QueryInterface(&webBrowser2);

	ShowWindow(GetControlWindow(), SW_SHOW);

	RECT rcClient;
	GetClientRect(hWndParent, &rcClient);
	SetRect(rcClient);

	Navigate(url.c_str());
}

WebView::~WebView()
{
	clear();
}

void WebView::clear()
{
	hWndParent = NULL;

	if (webBrowser2)
	{
		// webBrowser2->stop();
		webBrowser2->put_Visible(VARIANT_FALSE);

		webBrowser2->Release();
		webBrowser2 = NULL;
	}

	if (oleInPlaceObject)
	{
		oleInPlaceObject->Release();
		oleInPlaceObject = NULL;
	}

	if (oleObject)
	{
		oleObject->DoVerb(OLEIVERB_HIDE, NULL, this, 0, hWndParent, NULL);
		oleObject->Close(OLECLOSE_NOSAVE);
		OleSetContainedObject(oleObject, FALSE);
		oleObject->SetClientSite(NULL);
		CoDisconnectObject(oleObject, 0);

		oleObject->Release();
		oleObject = NULL;
	}
}

LRESULT CALLBACK WebView::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WebView* webBrowser1;

	switch (uMsg)
	{
	case WM_SIZE:
		webBrowser1 = (WebView*)GetWindowLongPtr(hWnd, 0);
		if (webBrowser1 != 0)
		{
			RECT rcClient;
			GetClientRect(hWnd, &rcClient);
			webBrowser1->SetRect(rcClient);
		}
		break;
	case WM_DESTROY:
		webBrowser1 = (WebView*)GetWindowLongPtr(hWnd, 0);
		if (webBrowser1 != 0)
		{
			SetWindowLongPtr(hWnd, 0, 0);
			webBrowser1->_trigger("close", (Variant*)NULL, 0);
			webBrowser1->clear();
			webBrowser1->Release();
		}
		break;
	default:
		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

result_t WebView::onclose(v8::Local<v8::Function> func, int32_t& retVal)
{
	return on("close", func, retVal);
}

RECT WebView::PixelToHiMetric(const RECT& _rc)
{
	static bool s_initialized = false;
	static int s_pixelsPerInchX, s_pixelsPerInchY;
	if (!s_initialized)
	{
		HDC hdc = ::GetDC(0);
		s_pixelsPerInchX = ::GetDeviceCaps(hdc, LOGPIXELSX);
		s_pixelsPerInchY = ::GetDeviceCaps(hdc, LOGPIXELSY);
		::ReleaseDC(0, hdc);
		s_initialized = true;
	}

	RECT rc;
	rc.left = MulDiv(2540, _rc.left, s_pixelsPerInchX);
	rc.top = MulDiv(2540, _rc.top, s_pixelsPerInchY);
	rc.right = MulDiv(2540, _rc.right, s_pixelsPerInchX);
	rc.bottom = MulDiv(2540, _rc.bottom, s_pixelsPerInchY);
	return rc;
}

void WebView::SetRect(const RECT& _rc)
{
	rObject = _rc;

	RECT hiMetricRect = PixelToHiMetric(rObject);
	SIZEL sz;
	sz.cx = hiMetricRect.right - hiMetricRect.left;
	sz.cy = hiMetricRect.bottom - hiMetricRect.top;
	oleObject->SetExtent(DVASPECT_CONTENT, &sz);

	if (oleInPlaceObject != 0)
		oleInPlaceObject->SetObjectRects(&rObject, &rObject);
}

// ----- Control methods -----

void WebView::GoBack()
{
	webBrowser2->GoBack();
}

void WebView::GoForward()
{
	webBrowser2->GoForward();
}

void WebView::Refresh()
{
	webBrowser2->Refresh();
}

void WebView::Navigate(exlib::string szUrl)
{
	bstr_t url(UTF8_W(szUrl));
	variant_t flags(0x02u); //navNoHistory
	webBrowser2->Navigate(url, &flags, 0, 0, 0);
}

// IUnknown
HRESULT STDMETHODCALLTYPE WebView::QueryInterface(REFIID riid,
        void**ppvObject)
{
	if (riid == IID_IUnknown)
		*ppvObject = static_cast<IOleClientSite*>(this);
	else if (riid == IID_IOleInPlaceSite)
		*ppvObject = static_cast<IOleInPlaceSite*>(this);
	else
		return E_NOINTERFACE;

	AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE WebView::AddRef(void)
{
	Ref();
	return 1;
}

ULONG STDMETHODCALLTYPE WebView::Release(void)
{
	Unref();
	return 1;
}

// IOleWindow
HRESULT STDMETHODCALLTYPE WebView::GetWindow(
    HWND* phwnd)
{
	(*phwnd) = hWndParent;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE WebView::ContextSensitiveHelp(
    BOOL fEnterMode)
{
	return E_NOTIMPL;
}

// IOleInPlaceSite
HRESULT STDMETHODCALLTYPE WebView::CanInPlaceActivate(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE WebView::OnInPlaceActivate(void)
{
	OleLockRunning(oleObject, TRUE, FALSE);
	if (oleInPlaceObject == NULL)
	{
		oleObject->QueryInterface(&oleInPlaceObject);
		oleInPlaceObject->SetObjectRects(&rObject, &rObject);
	}

	return S_OK;

}

HRESULT STDMETHODCALLTYPE WebView::OnUIActivate(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE WebView::GetWindowContext(
    IOleInPlaceFrame **ppFrame,
    IOleInPlaceUIWindow **ppDoc,
    LPRECT lprcPosRect,
    LPRECT lprcClipRect,
    LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	HWND hwnd = hWndParent;

	(*ppFrame) = NULL;
	(*ppDoc) = NULL;
	(*lprcPosRect).left = rObject.left;
	(*lprcPosRect).top = rObject.top;
	(*lprcPosRect).right = rObject.right;
	(*lprcPosRect).bottom = rObject.bottom;
	*lprcClipRect = *lprcPosRect;

	lpFrameInfo->fMDIApp = false;
	lpFrameInfo->hwndFrame = hwnd;
	lpFrameInfo->haccel = NULL;
	lpFrameInfo->cAccelEntries = 0;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE WebView::Scroll(
    SIZE scrollExtant)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebView::OnUIDeactivate(
    BOOL fUndoable)
{
	return S_OK;
}

HWND WebView::GetControlWindow()
{
	if (hWndControl != 0)
		return hWndControl;

	if (oleInPlaceObject == 0)
		return 0;

	oleInPlaceObject->GetWindow(&hWndControl);
	return hWndControl;
}

HRESULT STDMETHODCALLTYPE WebView::OnInPlaceDeactivate(void)
{
	hWndControl = 0;
	oleInPlaceObject = 0;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE WebView::DiscardUndoState(void)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebView::DeactivateAndUndo(void)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebView::OnPosRectChange(
    LPCRECT lprcPosRect)
{
	return E_NOTIMPL;
}

// IOleClientSite
HRESULT STDMETHODCALLTYPE WebView::SaveObject(void)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebView::GetMoniker(
    DWORD dwAssign,
    DWORD dwWhichMoniker,
    IMoniker **ppmk)
{
	if ((dwAssign == OLEGETMONIKER_ONLYIFTHERE) &&
	        (dwWhichMoniker == OLEWHICHMK_CONTAINER))
		return E_FAIL;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebView::GetContainer(
    IOleContainer **ppContainer)
{
	return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE WebView::ShowObject(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE WebView::OnShowWindow(
    BOOL fShow)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE WebView::RequestNewObjectLayout(void)
{
	return E_NOTIMPL;
}

// IStorage
HRESULT STDMETHODCALLTYPE WebView::CreateStream(
    const OLECHAR* pwcsName,
    DWORD grfMode,
    DWORD reserved1,
    DWORD reserved2,
    IStream **ppstm)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebView::OpenStream(
    const OLECHAR* pwcsName,
    void *reserved1,
    DWORD grfMode,
    DWORD reserved2,
    IStream **ppstm)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebView::CreateStorage(
    const OLECHAR* pwcsName,
    DWORD grfMode,
    DWORD reserved1,
    DWORD reserved2,
    IStorage **ppstg)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebView::OpenStorage(
    const OLECHAR* pwcsName,
    IStorage* pstgPriority,
    DWORD grfMode,
    SNB snbExclude,
    DWORD reserved,
    IStorage **ppstg)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebView::CopyTo(
    DWORD ciidExclude,
    const IID* rgiidExclude,
    SNB snbExclude,
    IStorage* pstgDest)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebView::MoveElementTo(
    const OLECHAR* pwcsName,
    IStorage* pstgDest,
    const OLECHAR* pwcsNewName,
    DWORD grfFlags)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebView::Commit(
    DWORD grfCommitFlags)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebView::Revert(void)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebView::EnumElements(
    DWORD reserved1,
    void *reserved2,
    DWORD reserved3,
    IEnumSTATSTG **ppenum)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebView::DestroyElement(
    const OLECHAR* pwcsName)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebView::RenameElement(
    const OLECHAR* pwcsOldName,
    const OLECHAR* pwcsNewName)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebView::SetElementTimes(
    const OLECHAR* pwcsName,
    const FILETIME* pctime,
    const FILETIME* patime,
    const FILETIME* pmtime)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebView::SetClass(REFCLSID clsid)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE WebView::SetStateBits(
    DWORD grfStateBits,
    DWORD grfMask)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WebView::Stat(
    STATSTG* pstatstg,
    DWORD grfStatFlag)
{
	return E_NOTIMPL;
}

}

#endif