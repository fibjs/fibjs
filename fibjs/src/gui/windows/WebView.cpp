/*
 * WebView.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: lion
 */

#ifdef _WIN32

#include "object.h"
#include "ifs/gui.h"
#include "ifs/fs.h"
#include "ifs/registry.h"
#include "ifs/os.h"
#include "path.h"
#include "WebView.h"
#include "Map.h"
#include "EventInfo.h"
#include "utf8.h"
#include <exlib/include/thread.h>
#include <exdispid.h>
#include <mshtml.h>

namespace fibjs {

DECLARE_MODULE(gui);

static exlib::LockedList<AsyncEvent> s_uiPool;
static uint32_t s_thread;

void putGuiPool(AsyncEvent* ac)
{
    s_uiPool.putTail(ac);
    PostThreadMessage(s_thread, WM_USER + 1000, 0, 0);
}

static HWND s_activeWin = NULL;

class gui_thread : public exlib::OSThread,
                   public IClassFactory {
private:
    class FSProtocol : public IInternetProtocol,
                       public IInternetProtocolInfo {
    public:
        FSProtocol(void)
            : m_cnt(0)
            , m_pProtSink(NULL)
        {
        }

        ~FSProtocol(void)
        {
            Clear();
        }

    public:
        // IUnknown
        STDMETHODIMP_(ULONG)
        AddRef(void)
        {
            return ++m_cnt;
        }

        STDMETHODIMP_(ULONG)
        Release(void)
        {
            if (--m_cnt == 0) {
                delete this;
                return 0;
            }

            return m_cnt;
        }

        STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
        {
            if (riid == IID_IUnknown || riid == IID_IInternetProtocol)
                *ppvObject = static_cast<IInternetProtocol*>(this);
            else if (riid == IID_IInternetProtocolInfo)
                *ppvObject = static_cast<IInternetProtocolInfo*>(this);
            else
                return E_NOINTERFACE;

            AddRef();
            return S_OK;
        }

    public:
        // IInternetProtocol
        STDMETHODIMP Start(LPCWSTR szUrl, IInternetProtocolSink* pIProtSink,
            IInternetBindInfo* pIBindInfo, DWORD grfSTI,
            HANDLE_PTR dwReserved)
        {
            if (!szUrl || !pIProtSink)
                return E_POINTER;

            if (grfSTI & PI_PARSE_URL)
                return S_OK;

            m_pProtSink = pIProtSink;
            m_pProtSink->AddRef();

            result_t hr = fs_base::cc_openFile(utf16to8String(szUrl + 3), "r", m_file);
            if (hr < 0)
                return INET_E_OBJECT_NOT_FOUND;

            LPWSTR pwszMimeType = 0;
            if (SUCCEEDED(FindMimeFromData(NULL, szUrl, NULL, 0, NULL, 0,
                    &pwszMimeType, 0)))
                m_pProtSink->ReportProgress(BINDSTATUS_VERIFIEDMIMETYPEAVAILABLE,
                    pwszMimeType);
            else
                m_pProtSink->ReportProgress(BINDSTATUS_VERIFIEDMIMETYPEAVAILABLE,
                    L"text/html");
            if (pwszMimeType)
                CoTaskMemFree(pwszMimeType);

            return m_pProtSink->ReportData(
                BSCF_FIRSTDATANOTIFICATION | BSCF_LASTDATANOTIFICATION | BSCF_DATAFULLYAVAILABLE, 1, 0);
        }

        STDMETHODIMP Continue(PROTOCOLDATA* pStateInfo)
        {
            return E_NOTIMPL;
        }

        STDMETHODIMP Abort(HRESULT hrReason, DWORD dwOptions)
        {
            HRESULT hr = E_FAIL;

            if (SUCCEEDED(hrReason))
                hrReason = E_ABORT;

            if (m_pProtSink)
                hr = m_pProtSink->ReportResult(hrReason, 0, 0);

            return hr;
        }

        STDMETHODIMP Terminate(DWORD dwOptions)
        {
            Clear();
            return S_OK;
        }

        STDMETHODIMP Suspend()
        {
            return E_NOTIMPL;
        }

        STDMETHODIMP Resume()
        {
            return E_NOTIMPL;
        }

        STDMETHODIMP Read(void* pv, ULONG cb, ULONG* pcbRead)
        {
            *pcbRead = 0;

            if (m_file) {
                obj_ptr<Buffer_base> buf;

                result_t hr = m_file->cc_read(cb, buf);
                if (hr < 0 || hr == CALL_RETURN_UNDEFINED)
                    *pcbRead = 0;
                else {
                    exlib::string strbuf;

                    buf->toString(strbuf);
                    memcpy(pv, strbuf.c_str(),
                        *pcbRead = (ULONG)strbuf.length());
                }
            }

            if (!*pcbRead) {
                if (m_pProtSink)
                    m_pProtSink->ReportResult(S_OK, 0, 0);

                return S_FALSE;
            }

            return S_OK;
        }

        STDMETHODIMP Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin,
            ULARGE_INTEGER* plibNewPosition)
        {
            return INET_E_DEFAULT_ACTION;
        }

        STDMETHODIMP LockRequest(DWORD dwOptions)
        {
            return INET_E_DEFAULT_ACTION;
        }

        STDMETHODIMP UnlockRequest()
        {
            return INET_E_DEFAULT_ACTION;
        }

    public:
        // IInternetProtocolInfo
        STDMETHODIMP ParseUrl(LPCWSTR pwzUrl, PARSEACTION ParseAction, DWORD dwParseFlags,
            LPWSTR pwzResult, DWORD cchResult, DWORD* pcchResult, DWORD dwReserved)
        {
            switch (ParseAction) {
            case PARSE_SECURITY_DOMAIN:
                wcscpy(pwzResult, L"fs:");
                *pcchResult = 6;
                return S_OK;
            }

            return INET_E_DEFAULT_ACTION;
        }

        STDMETHODIMP CombineUrl(LPCWSTR pwzBaseUrl, LPCWSTR pwzRelativeUrl,
            DWORD dwCombineFlags, LPWSTR pwzResult, DWORD cchResult,
            DWORD* pcchResult, DWORD dwReserved)
        {
            if (pwzBaseUrl == NULL || pwzRelativeUrl == NULL || pwzResult == NULL || pcchResult == NULL)
                return E_POINTER;

            if (qstrcmp(pwzBaseUrl, L"fs:", 3))
                return INET_E_DEFAULT_ACTION;

            LPCWSTR ptr = pwzRelativeUrl;

            if (qisascii(*ptr)) {
                ptr++;

                while (qisascii(*ptr) || qisdigit(*ptr))
                    ptr++;

                if (*ptr == ':')
                    return INET_E_DEFAULT_ACTION;
            }

            exlib::string base(utf16to8String(pwzBaseUrl + 3));
            exlib::string path;
            exlib::string out;

            path_base::dirname(base, path);
            resolvePath(path, utf16to8String(pwzRelativeUrl));
            path_base::normalize(path, out);

            exlib::wstring outw = utf8to16String("fs:" + out);

            if (cchResult < outw.length() + 1)
                return E_POINTER;

            exlib::qmemcpy(pwzResult, outw.c_str(), outw.length() + 1);
            *pcchResult = (ULONG)outw.length() + 1;

            return S_OK;
        }

        STDMETHODIMP CompareUrl(LPCWSTR pwzUrl1, LPCWSTR pwzUrl2, DWORD dwCompareFlags)
        {
            return INET_E_DEFAULT_ACTION;
        }

        STDMETHODIMP QueryInfo(LPCWSTR pwzUrl, QUERYOPTION QueryOption, DWORD dwQueryFlags,
            LPVOID pBuffer, DWORD cbBuffer, DWORD* pcbBuf, DWORD dwReserved)
        {
            return INET_E_DEFAULT_ACTION;
        }

    private:
        void Clear()
        {
            m_file.Release();

            if (m_pProtSink) {
                m_pProtSink->Release();
                m_pProtSink = NULL;
            }
        }

        ULONG m_cnt;
        obj_ptr<SeekableStream_base> m_file;
        IInternetProtocolSink* m_pProtSink;
    };

public:
    virtual void Run()
    {
        Runtime rt(NULL);

        OleInitialize(NULL);

        IInternetSecurityManager* pSecurityManager = NULL;
        CoCreateInstance(CLSID_InternetSecurityManager, NULL,
            CLSCTX_INPROC_SERVER, IID_IInternetSecurityManager,
            (void**)&pSecurityManager);
        if (pSecurityManager) {
            pSecurityManager->SetZoneMapping(URLZONE_TRUSTED, L"fs", SZM_CREATE);
            pSecurityManager->Release();
        }

        IInternetSession* pSession = NULL;
        CoInternetGetSession(0, &pSession, 0);
        if (pSession)
            pSession->RegisterNameSpace(this, IID_NULL, L"fs", 0, 0, 0);

        while (true) {
            AsyncEvent* p = s_uiPool.getHead();
            if (p)
                p->invoke();

            MSG msg;
            GetMessage(&msg, NULL, 0, 0);

            if (s_activeWin) {
                WebView* webView1 = (WebView*)GetWindowLongPtr(s_activeWin, 0);
                if (webView1 && (webView1->TranslateAccelerator(&msg) == S_OK))
                    continue;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

public:
    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
    {
        if (riid == IID_IUnknown || riid == IID_IClassFactory)
            *ppvObject = static_cast<IClassFactory*>(this);
        else
            return E_NOINTERFACE;

        AddRef();
        return S_OK;
    }

    STDMETHODIMP_(ULONG)
    AddRef(void)
    {
        return 1;
    }

    STDMETHODIMP_(ULONG)
    Release(void)
    {
        return 1;
    }

public:
    STDMETHODIMP CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, void** ppvObj)
    {
        if (ppvObj == NULL)
            return E_POINTER;

        if (pUnkOuter != NULL)
            return CLASS_E_NOAGGREGATION;

        if (riid != IID_IUnknown && riid != IID_IInternetProtocol && riid != IID_IInternetProtocolInfo)
            return E_NOINTERFACE;

        return (new FSProtocol())->QueryInterface(riid, ppvObj);
    }

    STDMETHODIMP LockServer(BOOL fLock)
    {
        return S_OK;
    }
};

void init_gui()
{
    gui_thread* _thGUI = new gui_thread();
    _thGUI->start();
    s_thread = _thGUI->thread_id;
    gui_base::setVersion(99999);
}

result_t gui_base::setVersion(int32_t ver)
{
    exlib::string p, exe;

    os_base::get_execPath(p);
    path_base::basename(p, "", exe);

    registry_base::set(registry_base::_CURRENT_USER,
        "SOFTWARE\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION\\" + exe,
        ver, registry_base::_DWORD);

    return 0;
}

static result_t async_open(obj_ptr<WebView> w)
{
    w->open();
    return 0;
}

result_t gui_base::open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal)
{
    obj_ptr<Map_base> o = new Map();
    o->set(opt);

    obj_ptr<WebView> w = new WebView(url, o);
    w->wrap();

    asyncCall(async_open, w, CALL_E_GUICALL);
    retVal = w;

    return 0;
}

const wchar_t* szWndClassMain = L"fibjs_window";

static void RegMainClass()
{
    static bool s_init = false;

    if (!s_init) {
        s_init = true;
        HINSTANCE hInstance = GetModuleHandle(NULL);

        WNDCLASSW wc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = sizeof(void*);
        wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
        wc.hInstance = hInstance;
        wc.lpfnWndProc = WebView::WndProc;
        wc.lpszClassName = szWndClassMain;
        wc.lpszMenuName = NULL;
        wc.style = 0;

        RegisterClassW(&wc);
    }
}

WebView::WebView(exlib::string url, Map_base* opt)
{
    m_url = url;
    m_opt = opt;

    m_ac = NULL;
    oleObject = NULL;
    oleInPlaceObject = NULL;
    oleInPlaceActiveObject = NULL;
    oleCommandTarget = NULL;
    connectionPoint = NULL;
    webBrowser2 = NULL;
    _onmessage = NULL;
    _onclose = NULL;
    hWndParent = NULL;

    m_visible = true;

    RegMainClass();
}

WebView::~WebView()
{
    clear();
}

HRESULT WebView::open()
{
    DWORD dwStyle = WS_POPUP;
    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;
    int nWidth = CW_USEDEFAULT;
    int nHeight = CW_USEDEFAULT;

    m_bSilent = false;
    m_maximize = false;

    if (m_opt) {
        Variant v;

        if (m_opt->get("left", v) == 0)
            x = v;
        if (m_opt->get("top", v) == 0)
            y = v;
        if (m_opt->get("width", v) == 0)
            nWidth = v;
        if (m_opt->get("height", v) == 0)
            nHeight = v;

        if (!(m_opt->get("border", v) == 0 && !v.boolVal())) {
            dwStyle |= WS_BORDER;

            if (!(m_opt->get("caption", v) == 0 && !v.boolVal()))
                dwStyle ^= WS_POPUP | WS_CAPTION | WS_SYSMENU;

            if (!(m_opt->get("resizable", v) == 0 && !v.boolVal()))
                dwStyle ^= WS_THICKFRAME | WS_BORDER | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

            if (m_opt->get("maximize", v) == 0 && v.boolVal()) {
                m_maximize = true;
                dwStyle |= WS_MAXIMIZE;
            }

            if (m_opt->get("visible", v) == 0 && !v.boolVal())
                m_visible = false;
        }

        if (m_opt->get("debug", v) == 0 && !v.boolVal())
            m_bSilent = true;
    } else
        dwStyle = WS_OVERLAPPEDWINDOW;

    int dpix = 0, dpiy = 0;
    GetDPI(&dpix, &dpiy);

    RECT actualDesktop;
    GetWindowRect(GetDesktopWindow(), &actualDesktop);

    if (nWidth != CW_USEDEFAULT)
        nWidth = nWidth * dpix / 96;
    else
        nWidth = actualDesktop.right * 3 / 4;

    if (nHeight != CW_USEDEFAULT)
        nHeight = nHeight * dpix / 96;
    else
        nHeight = actualDesktop.bottom * 3 / 4;

    if (x != CW_USEDEFAULT)
        x = x * dpix / 96;
    else
        x = (actualDesktop.right - nWidth) / 2;

    if (y != CW_USEDEFAULT)
        y = y * dpix / 96;
    else
        y = (actualDesktop.bottom - nHeight) / 2;

    hWndParent = CreateWindowExW(0, szWndClassMain, L"", dwStyle, x, y, nWidth, nHeight,
        NULL, NULL, GetModuleHandle(NULL), NULL);
    if (m_visible) {
        ShowWindow(hWndParent, m_maximize ? SW_MAXIMIZE : SW_SHOW);
        m_maximize = false;
    }

    ::SetRect(&rObject, -300, -300, 300, 300);

    SetWindowLongPtr(hWndParent, 0, (LONG_PTR)this);
    AddRef();

    StgCreateDocfile(NULL, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT | STGM_CREATE,
        0, &storage);
    ::OleCreate(CLSID_WebBrowser, IID_IOleObject, OLERENDER_DRAW, 0, this, storage,
        (void**)&oleObject);

    RECT posRect;
    ::SetRect(&posRect, -300, -300, 300, 300);
    oleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, this, -1, hWndParent, &posRect);
    oleObject->QueryInterface(&webBrowser2);
    oleObject->QueryInterface(&oleInPlaceActiveObject);

    oleObject->QueryInterface(&oleCommandTarget);

    if (m_bSilent)
        webBrowser2->put_Silent(VARIANT_TRUE);

    webBrowser2->put_RegisterAsDropTarget(VARIANT_FALSE);

    IConnectionPointContainer* cpc;

    webBrowser2->QueryInterface(IID_IConnectionPointContainer, (void**)&cpc);
    cpc->FindConnectionPoint(DIID_DWebBrowserEvents2, &connectionPoint);
    cpc->Release();

    connectionPoint->Advise((IDispatch*)this, &eventCookie);

    ShowWindow(GetControlWindow(), SW_SHOW);

    RECT rcClient;
    GetClientRect(hWndParent, &rcClient);
    SetRect(rcClient);

    Navigate(m_url.c_str());

    return 0;
}

void WebView::clear()
{
    if (_onmessage) {
        _onmessage->Release();
        _onmessage = NULL;
    }

    if (_onclose) {
        _onclose->Release();
        _onclose = NULL;
    }

    if (webBrowser2) {
        webBrowser2->Stop();
        webBrowser2->put_Visible(VARIANT_FALSE);

        webBrowser2->Release();
        webBrowser2 = NULL;
    }

    if (connectionPoint) {
        connectionPoint->Unadvise(eventCookie);
        connectionPoint->Release();
        connectionPoint = NULL;
    }

    if (oleInPlaceObject) {
        oleInPlaceObject->Release();
        oleInPlaceObject = NULL;
    }

    if (oleInPlaceActiveObject) {
        oleInPlaceActiveObject->Release();
        oleInPlaceActiveObject = NULL;
    }

    if (oleCommandTarget) {
        oleCommandTarget->Release();
        oleCommandTarget = NULL;
    }

    if (oleObject) {
        oleObject->DoVerb(OLEIVERB_HIDE, NULL, this, 0, hWndParent, NULL);
        oleObject->Close(OLECLOSE_NOSAVE);
        OleSetContainedObject(oleObject, FALSE);
        oleObject->SetClientSite(NULL);
        CoDisconnectObject(oleObject, 0);

        oleObject->Release();
        oleObject = NULL;
    }

    if (storage) {
        storage->Release();
        storage = NULL;
    }

    if (m_ac) {
        m_ac->post(0);
        m_ac = NULL;
    }

    if (s_activeWin == hWndParent)
        s_activeWin = NULL;

    hWndParent = NULL;
}

LRESULT CALLBACK WebView::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WebView* webView1;

    switch (uMsg) {
    case WM_ACTIVATE:
        if (WA_INACTIVE == wParam) {
            if (s_activeWin == hWnd)
                s_activeWin = NULL;
        } else
            s_activeWin = hWnd;
        break;

    case WM_SETFOCUS:
        webView1 = (WebView*)GetWindowLongPtr(hWnd, 0);
        if (webView1 != 0)
            webView1->OnDocumentComplete(NULL);
        break;
    case WM_MOVE:
        webView1 = (WebView*)GetWindowLongPtr(hWnd, 0);
        if (webView1 != 0) {
            RECT rcWin;
            GetWindowRect(hWnd, &rcWin);

            int dpix = 0, dpiy = 0;
            webView1->GetDPI(&dpix, &dpiy);

            obj_ptr<EventInfo> ei = new EventInfo(webView1, "move");
            ei->put("left", (int32_t)rcWin.left * 96 / dpix);
            ei->put("top", (int32_t)rcWin.top * 96 / dpiy);

            Variant v = ei;
            webView1->_emit("move", &v, 1);
        }
        break;
    case WM_SIZE:
        webView1 = (WebView*)GetWindowLongPtr(hWnd, 0);
        if (webView1 != 0) {
            RECT rcClient;
            GetClientRect(hWnd, &rcClient);
            webView1->SetRect(rcClient);

            RECT rcWin;
            GetWindowRect(hWnd, &rcWin);
            Variant vars[2];

            int dpix = 0, dpiy = 0;
            webView1->GetDPI(&dpix, &dpiy);

            obj_ptr<EventInfo> ei = new EventInfo(webView1, "resize");
            ei->put("width", (int32_t)(rcWin.right - rcWin.left) * 96 / dpix);
            ei->put("height", (int32_t)(rcWin.bottom - rcWin.top) * 96 / dpiy);

            Variant v = ei;
            webView1->_emit("resize", &v, 1);
        }
        break;
    case WM_CLOSE:
        webView1 = (WebView*)GetWindowLongPtr(hWnd, 0);
        if (webView1 != 0) {

            _variant_t vResult;
            webView1->postClose(vResult);
            if (vResult.vt == VT_BOOL && vResult.boolVal == VARIANT_FALSE)
                return 0;

            SetWindowLongPtr(hWnd, 0, 0);

            Variant v = new EventInfo(webView1, "closed");
            webView1->_emit("closed", &v, 1);

            webView1->clear();
            webView1->Release();
        }
        break;
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

result_t WebView::setHtml(exlib::string html, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    HGLOBAL hTextHandle = ::GlobalAlloc(GPTR, html.length() + 1);
    if (0 == hTextHandle)
        return CHECK_ERROR(LastError());

    memcpy((char*)hTextHandle, html.c_str(), html.length() + 1);

    IStream* pStream = 0;

    HRESULT hr = CreateStreamOnHGlobal(hTextHandle, TRUE, &pStream);

    if (FAILED(hr)) {
        GlobalFree(hTextHandle);
        return CHECK_ERROR(LastError());
    }

    IDispatch* pHtmlDoc = NULL;
    IPersistStreamInit* pPersistStreamInit = NULL;

    hr = webBrowser2->get_Document(&pHtmlDoc);
    if (SUCCEEDED(hr)) {
        hr = pHtmlDoc->QueryInterface(IID_IPersistStreamInit, (void**)&pPersistStreamInit);
        if (SUCCEEDED(hr)) {
            pPersistStreamInit->InitNew();
            pPersistStreamInit->Load(pStream);
            pPersistStreamInit->Release();
        }
        pHtmlDoc->Release();
    }

    pStream->Release();

    return 0;
}

result_t WebView::print(int32_t mode, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    switch (mode) {
    case 0:
        oleCommandTarget->Exec(NULL,
            OLECMDID_PRINT,
            OLECMDEXECOPT_DONTPROMPTUSER,
            NULL,
            NULL);
        break;
    case 1:
        oleCommandTarget->Exec(NULL,
            OLECMDID_PRINT,
            OLECMDEXECOPT_PROMPTUSER,
            NULL,
            NULL);
        break;
    case 2:
        oleCommandTarget->Exec(NULL,
            OLECMDID_PRINTPREVIEW,
            OLECMDEXECOPT_PROMPTUSER,
            NULL,
            NULL);
        break;
    default:
        return CHECK_ERROR(CALL_E_INVALIDARG);
    }

    return 0;
}

result_t WebView::close(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    PostMessage(hWndParent, WM_CLOSE, 0, 0);
    return 0;
}

result_t WebView::wait(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    m_ac = ac;
    return CALL_E_PENDDING;
}

result_t WebView::postClose(_variant_t& retVal)
{
    if (_onclose) {
        DISPPARAMS params = { NULL, NULL, 0, 0 };

        _onclose->Invoke(DISPID_VALUE, IID_NULL, LOCALE_USER_DEFAULT,
            DISPATCH_METHOD, &params, &retVal, NULL, NULL);
    }

    return 0;
}

result_t WebView::postMessage(exlib::string msg, _variant_t& retVal)
{
    if (_onmessage) {
        _variant_t msg(UTF8_W(msg));
        DISPPARAMS params = { &msg, NULL, 1, 0 };

        _onmessage->Invoke(DISPID_VALUE, IID_NULL, LOCALE_USER_DEFAULT,
            DISPATCH_METHOD, &params, &retVal, NULL, NULL);
    }

    return 0;
}

result_t WebView::postMessage(exlib::string msg, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_GUICALL);

    _variant_t vResult;
    return postMessage(msg, vResult);
}

result_t WebView::get_visible(bool& retVal)
{
    retVal = m_visible;
    return 0;
}

result_t WebView::set_visible(bool newVal)
{
    m_visible = newVal;

    if (m_visible) {
        ShowWindow(hWndParent, m_maximize ? SW_MAXIMIZE : SW_SHOW);
        m_maximize = false;
    } else
        ShowWindow(hWndParent, SW_HIDE);

    return 0;
}

HRESULT WebView::TranslateAccelerator(MSG* msg)
{
    return oleInPlaceActiveObject->TranslateAccelerator(msg);
}

void WebView::GetDPI(int* dpix, int* dpiy)
{
    HDC hdc = ::GetDC(hWndParent);
    *dpix = ::GetDeviceCaps(hdc, LOGPIXELSX);
    *dpiy = ::GetDeviceCaps(hdc, LOGPIXELSY);
    ::ReleaseDC(hWndParent, hdc);
}

RECT WebView::PixelToHiMetric(const RECT& _rc)
{
    int dpix, dpiy;

    GetDPI(&dpix, &dpiy);

    RECT rc;
    rc.left = MulDiv(2540, _rc.left, dpix);
    rc.top = MulDiv(2540, _rc.top, dpiy);
    rc.right = MulDiv(2540, _rc.right, dpix);
    rc.bottom = MulDiv(2540, _rc.bottom, dpiy);
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
HRESULT WebView::QueryInterface(REFIID riid, void** ppvObject)
{
    if (riid == IID_IUnknown)
        *ppvObject = static_cast<IDispatch*>(this);
    else if (riid == IID_IDispatch)
        *ppvObject = static_cast<IDispatch*>(this);
    else if (riid == IID_IOleInPlaceSite)
        *ppvObject = static_cast<IOleInPlaceSite*>(this);
    else if (riid == IID_IDocHostUIHandler)
        *ppvObject = static_cast<IDocHostUIHandler*>(this);
    else if (riid == IID_IServiceProvider)
        *ppvObject = static_cast<IServiceProvider*>(this);
    else if (riid == IID_IInternetSecurityManager)
        *ppvObject = static_cast<IInternetSecurityManager*>(this);
    else if (riid == IID_IOleCommandTarget)
        *ppvObject = static_cast<IOleCommandTarget*>(this);
    else
        return E_NOINTERFACE;

    AddRef();
    return S_OK;
}

ULONG WebView::AddRef(void)
{
    Ref();
    return 1;
}

ULONG WebView::Release(void)
{
    Unref();
    return 1;
}

#define DISPID_POSTMESSAGE (1000 + 1)
#define DISPID_LOG (1000 + 2)
#define DISPID_ONMESSAGE (1000 + 3)
#define DISPID_ONCLOSE (1000 + 4)
#define DISPID_CLOSE (1000 + 5)

// IDispatch
HRESULT WebView::GetIDsOfNames(REFIID riid, OLECHAR** rgszNames,
    unsigned int cNames, LCID lcid, DISPID* rgdispid)
{
    uint32_t i;

    for (i = 0; i < cNames; i++) {
        if (!qstrcmp(rgszNames[i], L"postMessage"))
            rgdispid[i] = DISPID_POSTMESSAGE;
        else if (!qstrcmp(rgszNames[i], L"log"))
            rgdispid[i] = DISPID_LOG;
        else if (!qstrcmp(rgszNames[i], L"onmessage"))
            rgdispid[i] = DISPID_ONMESSAGE;
        else if (!qstrcmp(rgszNames[i], L"onclose"))
            rgdispid[i] = DISPID_ONCLOSE;
        else if (!qstrcmp(rgszNames[i], L"close"))
            rgdispid[i] = DISPID_CLOSE;
        else
            rgdispid[i] = DISPID_UNKNOWN;
    }

    return S_OK;
}

HRESULT WebView::GetTypeInfo(unsigned int itinfo, LCID lcid, ITypeInfo** pptinfo)
{
    return E_NOTIMPL;
}

HRESULT WebView::GetTypeInfoCount(unsigned int* pctinfo)
{
    return E_NOTIMPL;
}

HRESULT WebView::Invoke(DISPID dispid, REFIID riid, LCID lcid, WORD wFlags,
    DISPPARAMS* pDispParams, VARIANT* pvarResult,
    EXCEPINFO* pexecinfo, unsigned int* puArgErr)
{
    switch (dispid) {
    case DISPID_BEFORENAVIGATE2:
        return OnBeforeNavigate2(pDispParams);
    case DISPID_COMMANDSTATECHANGE:
        return OnCommandStateChange(pDispParams);
    case DISPID_DOCUMENTCOMPLETE:
        return OnDocumentComplete(pDispParams);
    case DISPID_DOWNLOADBEGIN:
        return OnDownloadBegin(pDispParams);
    case DISPID_DOWNLOADCOMPLETE:
        return OnDownloadComplete(pDispParams);
    case DISPID_NAVIGATECOMPLETE2:
        return OnNavigateComplete2(pDispParams);
    case DISPID_NEWWINDOW2:
        return OnNewWindow2(pDispParams);
    case DISPID_PROGRESSCHANGE:
        return OnProgressChange(pDispParams);
    case DISPID_PROPERTYCHANGE:
        return OnPropertyChange(pDispParams);
    case DISPID_STATUSTEXTCHANGE:
        return OnStatusTextChange(pDispParams);
    case DISPID_TITLECHANGE:
        return OnTitleChange(pDispParams);
    case DISPID_WINDOWCLOSING:
        return OnWindowClosing(pDispParams);

    case DISPID_POSTMESSAGE:
        return OnPostMessage(pDispParams);
    case DISPID_LOG:
        return OnLog(pDispParams);
    case DISPID_ONMESSAGE:
        return OnOnMessage(pDispParams);
    case DISPID_ONCLOSE:
        return OnOnClose(pDispParams);
    case DISPID_CLOSE:
        return OnClose(pDispParams);
    }

    return S_OK;
}

// IOleWindow
HRESULT WebView::GetWindow(HWND* phwnd)
{
    (*phwnd) = hWndParent;
    return S_OK;
}

HRESULT WebView::ContextSensitiveHelp(BOOL fEnterMode)
{
    return E_NOTIMPL;
}

// IOleInPlaceSite
HRESULT WebView::CanInPlaceActivate(void)
{
    return S_OK;
}

HRESULT WebView::OnInPlaceActivate(void)
{
    OleLockRunning(oleObject, TRUE, FALSE);
    if (oleInPlaceObject == NULL) {
        oleObject->QueryInterface(&oleInPlaceObject);
        oleInPlaceObject->SetObjectRects(&rObject, &rObject);
    }

    return S_OK;
}

HRESULT WebView::OnUIActivate(void)
{
    return S_OK;
}

HRESULT WebView::GetWindowContext(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc,
    LPRECT lprcPosRect, LPRECT lprcClipRect,
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

HRESULT WebView::Scroll(SIZE scrollExtant)
{
    return E_NOTIMPL;
}

HRESULT WebView::OnUIDeactivate(BOOL fUndoable)
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

HRESULT WebView::OnInPlaceDeactivate(void)
{
    hWndControl = 0;
    oleInPlaceObject = 0;

    return S_OK;
}

HRESULT WebView::DiscardUndoState(void)
{
    return E_NOTIMPL;
}

HRESULT WebView::DeactivateAndUndo(void)
{
    return E_NOTIMPL;
}

HRESULT WebView::OnPosRectChange(LPCRECT lprcPosRect)
{
    return E_NOTIMPL;
}

// IOleClientSite
HRESULT WebView::SaveObject(void)
{
    return S_OK;
}

HRESULT WebView::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk)
{
    if ((dwAssign == OLEGETMONIKER_ONLYIFTHERE) && (dwWhichMoniker == OLEWHICHMK_CONTAINER))
        return E_FAIL;

    return E_NOTIMPL;
}

HRESULT WebView::GetContainer(IOleContainer** ppContainer)
{
    return E_NOINTERFACE;
}

HRESULT WebView::ShowObject(void)
{
    return S_OK;
}

HRESULT WebView::OnShowWindow(BOOL fShow)
{
    return S_OK;
}

HRESULT WebView::RequestNewObjectLayout(void)
{
    return S_OK;
}

// IDocHostUIHandler
HRESULT WebView::ShowContextMenu(DWORD dwID, POINT* ppt, IUnknown* pcmdtReserved,
    IDispatch* pdispReserved)
{
    if (dwID == CONTEXT_MENU_TEXTSELECT || dwID == CONTEXT_MENU_CONTROL)
        return S_FALSE;

    return S_OK;
}

HRESULT WebView::GetHostInfo(DOCHOSTUIINFO* pInfo)
{
    if (pInfo != NULL)
        pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_FLAT_SCROLLBAR | DOCHOSTUIFLAG_THEME | DOCHOSTUIFLAG_DPI_AWARE;

    return S_OK;
}

HRESULT WebView::ShowUI(DWORD dwID, IOleInPlaceActiveObject* pActiveObject,
    IOleCommandTarget* pCommandTarget, IOleInPlaceFrame* pFrame,
    IOleInPlaceUIWindow* pDoc)
{
    return E_NOTIMPL;
}

HRESULT WebView::HideUI(void)
{
    return E_NOTIMPL;
}

HRESULT WebView::UpdateUI(void)
{
    return E_NOTIMPL;
}

HRESULT WebView::EnableModeless(BOOL fEnable)
{
    return E_NOTIMPL;
}

HRESULT WebView::OnDocWindowActivate(BOOL fActivate)
{
    return E_NOTIMPL;
}

HRESULT WebView::OnFrameWindowActivate(BOOL fActivate)
{
    return E_NOTIMPL;
}

HRESULT WebView::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow* pUIWindow,
    BOOL fRameWindow)
{
    return E_NOTIMPL;
}

HRESULT WebView::TranslateAccelerator(LPMSG lpMsg, const GUID* pguidCmdGroup,
    DWORD nCmdID)
{
    return E_NOTIMPL;
}

HRESULT WebView::GetOptionKeyPath(LPOLESTR* pchKey, DWORD dw)
{
    return E_NOTIMPL;
}

HRESULT WebView::GetDropTarget(IDropTarget* pDropTarget, IDropTarget** ppDropTarget)
{
    return E_NOTIMPL;
}

HRESULT WebView::GetExternal(IDispatch** ppDispatch)
{
    *ppDispatch = this;
    AddRef();

    return S_OK;
}

HRESULT WebView::TranslateUrl(DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut)
{
    return E_NOTIMPL;
}

HRESULT WebView::FilterDataObject(IDataObject* pDO, IDataObject** ppDORet)
{
    return E_NOTIMPL;
}

// IOleCommandTarget
HRESULT WebView::Exec(const GUID* pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt,
    VARIANT* pvaIn, VARIANT* pvaOut)
{
    HRESULT hr = S_OK;

    if (pguidCmdGroup && IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler)) {

        switch (nCmdID) {
        case OLECMDID_SHOWSCRIPTERROR: {
            IHTMLDocument2* pDoc = NULL;
            IHTMLWindow2* pWindow = NULL;
            IHTMLEventObj* pEventObj = NULL;
            BSTR rgwszNames[5] = {
                SysAllocString(L"errorLine"),
                SysAllocString(L"errorCharacter"),
                SysAllocString(L"errorCode"),
                SysAllocString(L"errorMessage"),
                SysAllocString(L"errorUrl")
            };
            DISPID rgDispIDs[5];
            _variant_t rgvaEventInfo[5];
            DISPPARAMS params;
            BOOL fContinueRunningScripts = true;
            int i;

            params.cArgs = 0;
            params.cNamedArgs = 0;

            hr = pvaIn->punkVal->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc);
            hr = pDoc->get_parentWindow(&pWindow);
            pDoc->Release();
            pDoc = NULL;

            hr = pWindow->get_event(&pEventObj);
            pWindow->Release();
            pWindow = NULL;

            // Get the error info from the window.event object.
            for (i = 0; i < 5; i++) {
                // Get the property's dispID.
                hr = pEventObj->GetIDsOfNames(IID_NULL, &rgwszNames[i], 1,
                    LOCALE_SYSTEM_DEFAULT, &rgDispIDs[i]);
                // Get the value of the property.
                hr = pEventObj->Invoke(rgDispIDs[i], IID_NULL,
                    LOCALE_SYSTEM_DEFAULT,
                    DISPATCH_PROPERTYGET, &params, &rgvaEventInfo[i],
                    NULL, NULL);
                SysFreeString(rgwszNames[i]);
            }
            pEventObj->Release();
            pEventObj = NULL;

            exlib::string msg("WebView Error: ");
            char buf[32];

            if (rgvaEventInfo[3].bstrVal) {
                msg += utf16to8String(rgvaEventInfo[3].bstrVal);
                if (rgvaEventInfo[4].bstrVal) {
                    msg += "\n    at " + utf16to8String(rgvaEventInfo[4].bstrVal);
                    sprintf(buf, ":%d:%d", rgvaEventInfo[0].intVal, rgvaEventInfo[1].intVal);
                    msg += buf;
                }
            }

            errorLog(msg);

            (*pvaOut).vt = VT_BOOL;
            if (fContinueRunningScripts) {
                // Continue running scripts on the page.
                (*pvaOut).boolVal = VARIANT_TRUE;
            } else {
                // Stop running scripts on the page.
                (*pvaOut).boolVal = VARIANT_FALSE;
            }
            break;
        }
        default:
            hr = OLECMDERR_E_NOTSUPPORTED;
            break;
        }
    } else {
        hr = OLECMDERR_E_UNKNOWNGROUP;
    }
    return (hr);
}

HRESULT WebView::QueryStatus(const GUID* pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[],
    OLECMDTEXT* pCmdText)
{
    return E_NOTIMPL;
}

// IServiceProvider
HRESULT WebView::QueryService(
    REFGUID siid,
    REFIID riid,
    void** ppvObject)
{
    if (siid == IID_IInternetSecurityManager && riid == IID_IInternetSecurityManager) {
        *ppvObject = static_cast<IInternetSecurityManager*>(this);
        AddRef();
    } else {
        *ppvObject = 0;
        return E_NOINTERFACE;
    }
    return S_OK;
}

// IInternetSecurityManager
HRESULT WebView::SetSecuritySite(IInternetSecurityMgrSite* pSite)
{
    return E_NOTIMPL;
}

HRESULT WebView::GetSecuritySite(IInternetSecurityMgrSite** ppSite)
{
    return E_NOTIMPL;
}

HRESULT WebView::MapUrlToZone(LPCWSTR pwszUrl, DWORD* pdwZone, DWORD dwFlags)
{
    if (!pdwZone)
        return E_INVALIDARG;

    return INET_E_DEFAULT_ACTION;
}

HRESULT WebView::GetSecurityId(LPCWSTR pwszUrl, BYTE* pbSecurityId, DWORD* pcbSecurityId,
    DWORD_PTR dwReserved)
{
    if (!pbSecurityId)
        return E_INVALIDARG;

    return INET_E_DEFAULT_ACTION;
}

#define URLACTION_LOOSE_XAML 0x00002402
#define URLACTION_MANAGED_SIGNED 0x00002001
#define URLACTION_MANAGED_UNSIGNED 0x00002004
#define URLACTION_WINDOWS_BROWSER_APPLICATIONS 0x00002400
#define URLACTION_WINFX_SETUP 0x00002600
#define URLACTION_XPS_DOCUMENTS 0x00002401
#define URLACTION_ALLOW_AUDIO_VIDEO 0x00002701
#define URLACTION_ALLOW_STRUCTURED_STORAGE_SNIFFING 0x00002703
#define URLACTION_ALLOW_XDOMAIN_SUBFRAME_RESIZE 0x00001408
#define URLACTION_FEATURE_CROSSDOMAIN_FOCUS_CHANGE 0x00002107
#define URLACTION_SHELL_PREVIEW 0x0000180F
#define URLACTION_SHELL_REMOTEQUERY 0x0000180E
#define URLACTION_SHELL_SECURE_DRAGSOURCE 0x0000180D
#define URLACTION_ALLOW_APEVALUATION 0x00002301
#define URLACTION_LOWRIGHTS 0x00002500
#define URLACTION_ALLOW_ACTIVEX_FILTERING 0x00002702
#define URLACTION_DOTNET_USERCONTROLS 0x00002005
#define URLACTION_FEATURE_DATA_BINDING 0x00002106
#define URLACTION_FEATURE_CROSSDOMAIN_FOCUS_CHANGE 0x00002107
#define URLACTION_SCRIPT_XSSFILTER 0x00001409
#define URLACTION_INPRIVATE_BLOCKING 0x00002700

HRESULT WebView::ProcessUrlAction(LPCWSTR pwszUrl, DWORD dwAction, BYTE* pPolicy,
    DWORD cbPolicy, BYTE* pContext, DWORD cbContext,
    DWORD dwFlags, DWORD dwReserved)
{
    switch (dwAction) {
    case URLACTION_ACTIVEX_CONFIRM_NOOBJECTSAFETY:
    case URLACTION_ACTIVEX_OVERRIDE_DATA_SAFETY:
    case URLACTION_ACTIVEX_OVERRIDE_SCRIPT_SAFETY:
    case URLACTION_FEATURE_BLOCK_INPUT_PROMPTS:
    case URLACTION_SCRIPT_OVERRIDE_SAFETY:
    case URLACTION_SHELL_EXTENSIONSECURITY:
    case URLACTION_ACTIVEX_NO_WEBOC_SCRIPT:
    case URLACTION_ACTIVEX_OVERRIDE_OBJECT_SAFETY:
    case URLACTION_ACTIVEX_OVERRIDE_OPTIN:
    case URLACTION_ACTIVEX_OVERRIDE_REPURPOSEDETECTION:
    case URLACTION_ACTIVEX_RUN:
    case URLACTION_ACTIVEX_SCRIPTLET_RUN:
    case URLACTION_ACTIVEX_DYNSRC_VIDEO_AND_ANIMATION:
    case URLACTION_ALLOW_RESTRICTEDPROTOCOLS:
    case URLACTION_AUTOMATIC_ACTIVEX_UI:
    case URLACTION_AUTOMATIC_DOWNLOAD_UI:
    case URLACTION_BEHAVIOR_RUN:
    case URLACTION_CLIENT_CERT_PROMPT:
    case URLACTION_COOKIES:
    case URLACTION_COOKIES_ENABLED:
    case URLACTION_COOKIES_SESSION:
    case URLACTION_COOKIES_SESSION_THIRD_PARTY:
    case URLACTION_COOKIES_THIRD_PARTY:
    case URLACTION_CROSS_DOMAIN_DATA:
    case URLACTION_DOWNLOAD_SIGNED_ACTIVEX:
    case URLACTION_DOWNLOAD_UNSIGNED_ACTIVEX:
    case URLACTION_FEATURE_DATA_BINDING:
    case URLACTION_FEATURE_FORCE_ADDR_AND_STATUS:
    case URLACTION_FEATURE_MIME_SNIFFING:
    case URLACTION_FEATURE_SCRIPT_STATUS_BAR:
    case URLACTION_FEATURE_WINDOW_RESTRICTIONS:
    case URLACTION_FEATURE_ZONE_ELEVATION:
    case URLACTION_HTML_FONT_DOWNLOAD:
    case URLACTION_HTML_INCLUDE_FILE_PATH:
    case URLACTION_HTML_JAVA_RUN:
    case URLACTION_HTML_META_REFRESH:
    case URLACTION_HTML_MIXED_CONTENT:
    case URLACTION_HTML_SUBFRAME_NAVIGATE:
    case URLACTION_HTML_SUBMIT_FORMS:
    case URLACTION_HTML_SUBMIT_FORMS_FROM:
    case URLACTION_HTML_SUBMIT_FORMS_TO:
    case URLACTION_HTML_USERDATA_SAVE:
    case URLACTION_LOOSE_XAML:
    case URLACTION_MANAGED_SIGNED:
    case URLACTION_MANAGED_UNSIGNED:
    case URLACTION_SCRIPT_JAVA_USE:
    case URLACTION_SCRIPT_PASTE:
    case URLACTION_SCRIPT_RUN:
    case URLACTION_SCRIPT_SAFE_ACTIVEX:
    case URLACTION_SHELL_EXECUTE_HIGHRISK:
    case URLACTION_SHELL_EXECUTE_LOWRISK:
    case URLACTION_SHELL_EXECUTE_MODRISK:
    case URLACTION_SHELL_FILE_DOWNLOAD:
    case URLACTION_SHELL_INSTALL_DTITEMS:
    case URLACTION_SHELL_MOVE_OR_COPY:
    case URLACTION_SHELL_VERB:
    case URLACTION_SHELL_WEBVIEW_VERB:
    case URLACTION_WINDOWS_BROWSER_APPLICATIONS:
    case URLACTION_WINFX_SETUP:
    case URLACTION_XPS_DOCUMENTS:
    case URLACTION_ALLOW_AUDIO_VIDEO: // ie9
    case URLACTION_ALLOW_STRUCTURED_STORAGE_SNIFFING: // ie9
    case URLACTION_ALLOW_XDOMAIN_SUBFRAME_RESIZE: // ie7
    case URLACTION_FEATURE_CROSSDOMAIN_FOCUS_CHANGE: // ie7
    case URLACTION_SHELL_ENHANCED_DRAGDROP_SECURITY:
    case URLACTION_SHELL_PREVIEW: // win7
    case URLACTION_SHELL_REMOTEQUERY: // win7
    case URLACTION_SHELL_RTF_OBJECTS_LOAD: // ie6sp2
    case URLACTION_SHELL_SECURE_DRAGSOURCE: // ie7
    // ie6sp2, value the same as URLACTION_SHELL_EXECUTE_HIGHRISK
    // case URLACTION_SHELL_SHELLEXECUTE:
    // ie8, probably registry only
    case URLACTION_DOTNET_USERCONTROLS:
    case URLACTION_CHANNEL_SOFTDIST_PERMISSIONS:
    //*pPolicy = URLPOLICY_CHANNEL_SOFTDIST_AUTOINSTALL;
    case URLACTION_JAVA_PERMISSIONS:
    //*pPolicy = URLPOLICY_JAVA_LOW;
    case URLACTION_CREDENTIALS_USE:
        //*pPolicy = URLPOLICY_CREDENTIALS_SILENT_LOGON_OK;
        *pPolicy = URLPOLICY_ALLOW;
        return S_OK;

    case URLACTION_ALLOW_APEVALUATION: // Phishing filter.
    case URLACTION_LOWRIGHTS: // Vista Protected Mode.
    case URLACTION_SHELL_POPUPMGR:
    case URLACTION_SCRIPT_XSSFILTER:
    case URLACTION_ACTIVEX_TREATASUNTRUSTED:
    case URLACTION_ALLOW_ACTIVEX_FILTERING: // ie9
        *pPolicy = URLPOLICY_DISALLOW;
        return S_FALSE;
    }

    return INET_E_DEFAULT_ACTION;
}

HRESULT WebView::QueryCustomPolicy(LPCWSTR pwszUrl, REFGUID guidKey, BYTE** ppPolicy,
    DWORD* pcbPolicy, BYTE* pContext, DWORD cbContext,
    DWORD dwReserved)
{
    return INET_E_DEFAULT_ACTION;
}

HRESULT WebView::SetZoneMapping(DWORD dwZone, LPCWSTR lpszPattern, DWORD dwFlags)
{
    return E_NOTIMPL;
}

HRESULT WebView::GetZoneMappings(DWORD dwZone, IEnumString** ppenumString, DWORD dwFlags)
{
    return E_NOTIMPL;
}

// event
HRESULT WebView::OnBeforeNavigate2(DISPPARAMS* pDispParams)
{
    return E_NOTIMPL;
}

HRESULT WebView::OnCommandStateChange(DISPPARAMS* pDispParams)
{
    return E_NOTIMPL;
}

HRESULT WebView::OnDocumentComplete(DISPPARAMS* pDispParams)
{
    if (pDispParams && pDispParams->cArgs >= 2 && pDispParams->rgvarg[1].vt == VT_DISPATCH) {
        IWebBrowser2* frame = NULL;

        pDispParams->rgvarg[1].pdispVal->QueryInterface(&frame);
        if (frame == webBrowser2) {
            oleObject->DoVerb(OLEIVERB_UIACTIVATE, NULL, this, -1, hWndParent, &rObject);

            Variant v = new EventInfo(this, "load");
            _emit("load", &v, 1);
        }

        frame->Release();
    }

    return E_NOTIMPL;
}

HRESULT WebView::OnDownloadBegin(DISPPARAMS* pDispParams)
{
    return E_NOTIMPL;
}

HRESULT WebView::OnDownloadComplete(DISPPARAMS* pDispParams)
{
    return E_NOTIMPL;
}

extern const wchar_t* g_console_js;

HRESULT WebView::OnNavigateComplete2(DISPPARAMS* pDispParams)
{
    IWebBrowser2* frame = NULL;
    pDispParams->rgvarg[1].pdispVal->QueryInterface(&frame);

    HRESULT hr;
    IDispatch* pHtmlDoc = NULL;

    hr = frame->get_Document(&pHtmlDoc);
    if (SUCCEEDED(hr)) {
        IHTMLDocument2* pDoc = NULL;
        IHTMLWindow2* pWindow = NULL;
        IDispatch* pScript = NULL;

        pHtmlDoc->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc);
        pDoc->get_Script(&pScript);

        DISPID dispid = -1;
        LPOLESTR pszName = L"eval";

        pScript->GetIDsOfNames(IID_NULL, &pszName, 1,
            LOCALE_USER_DEFAULT, &dispid);

        if (dispid != -1) {
            _variant_t arg = g_console_js;
            _variant_t result;
            DISPPARAMS dispparams;
            dispparams.rgvarg = &arg;
            dispparams.cArgs = 1;
            dispparams.cNamedArgs = 0;

            pScript->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD,
                &dispparams, &result, NULL, NULL);
        } else {
            _variant_t result;
            _bstr_t s = g_console_js;
            _bstr_t l = L"JavaScript";

            pDoc->get_parentWindow(&pWindow);
            pWindow->execScript(s, l, &result);

            pWindow->Release();
        }

        pScript->Release();
        pHtmlDoc->Release();
        pDoc->Release();
    }

    frame->Release();

    return E_NOTIMPL;
}

HRESULT WebView::OnNewWindow2(DISPPARAMS* pDispParams)
{
    return E_NOTIMPL;
}

HRESULT WebView::OnProgressChange(DISPPARAMS* pDispParams)
{
    return E_NOTIMPL;
}

HRESULT WebView::OnPropertyChange(DISPPARAMS* pDispParams)
{
    return E_NOTIMPL;
}

HRESULT WebView::OnStatusTextChange(DISPPARAMS* pDispParams)
{
    return E_NOTIMPL;
}

HRESULT WebView::OnTitleChange(DISPPARAMS* pDispParams)
{
    if (pDispParams->cArgs > 0 && pDispParams->rgvarg[0].vt == VT_BSTR)
        SetWindowTextW(hWndParent, pDispParams->rgvarg[0].bstrVal);

    return S_OK;
}

HRESULT WebView::OnWindowClosing(DISPPARAMS* pDispParams)
{
    PostMessage(hWndParent, WM_CLOSE, 0, 0);
    *pDispParams->rgvarg->pboolVal = VARIANT_TRUE;
    return S_OK;
}

HRESULT WebView::OnPostMessage(DISPPARAMS* pDispParams)
{
    if (pDispParams->cArgs != 1)
        return DISP_E_BADPARAMCOUNT;

    Variant v;
    if (pDispParams->rgvarg[0].vt == VT_BSTR) {
        v = utf16to8String(pDispParams->rgvarg[0].bstrVal);
        _emit("message", &v, 1);
    } else {
        _variant_t vstr;
        HRESULT hr = VariantChangeType(&vstr, &pDispParams->rgvarg[0],
            VARIANT_ALPHABOOL, VT_BSTR);
        if (!SUCCEEDED(hr))
            return hr;

        v = utf16to8String(vstr.bstrVal);
        _emit("message", &v, 1);
    }

    return S_OK;
}

HRESULT WebView::OnLog(DISPPARAMS* pDispParams)
{
    if (pDispParams->cArgs != 2)
        return DISP_E_BADPARAMCOUNT;

    if (!m_bSilent) {
        int32_t priority;
        if (pDispParams->rgvarg[1].vt == VT_I4) {
            priority = pDispParams->rgvarg[1].lVal;
        } else {
            _variant_t vint;
            HRESULT hr = VariantChangeType(&vint, &pDispParams->rgvarg[1],
                VARIANT_ALPHABOOL, VT_I4);
            if (!SUCCEEDED(hr))
                return hr;
            priority = vint.lVal;
        }

        exlib::string msg;
        if (pDispParams->rgvarg[0].vt == VT_BSTR) {
            msg = utf16to8String(pDispParams->rgvarg[0].bstrVal);
        } else {
            _variant_t vstr;
            HRESULT hr = VariantChangeType(&vstr, &pDispParams->rgvarg[0],
                VARIANT_ALPHABOOL, VT_BSTR);
            if (!SUCCEEDED(hr))
                return hr;

            if (vstr.bstrVal)
                msg = utf16to8String(vstr.bstrVal);
        }

        outLog(priority, msg);
    }

    return S_OK;
}

HRESULT WebView::OnOnMessage(DISPPARAMS* pDispParams)
{
    if (pDispParams->cArgs != 1)
        return DISP_E_BADPARAMCOUNT;

    if (pDispParams->rgvarg[0].vt != VT_DISPATCH)
        return TYPE_E_TYPEMISMATCH;

    if (_onmessage)
        _onmessage->Release();

    _onmessage = pDispParams->rgvarg[0].pdispVal;
    _onmessage->AddRef();

    return S_OK;
}

HRESULT WebView::OnOnClose(DISPPARAMS* pDispParams)
{
    if (pDispParams->cArgs != 1)
        return DISP_E_BADPARAMCOUNT;

    if (pDispParams->rgvarg[0].vt != VT_DISPATCH)
        return TYPE_E_TYPEMISMATCH;

    if (_onclose)
        _onclose->Release();

    _onclose = pDispParams->rgvarg[0].pdispVal;
    _onclose->AddRef();

    return S_OK;
}

HRESULT WebView::OnClose(DISPPARAMS* pDispParams)
{
    if (pDispParams->cArgs != 0)
        return DISP_E_BADPARAMCOUNT;

    PostMessage(hWndParent, WM_CLOSE, 0, 0);
    return S_OK;
}
}

#endif
