/*
 * WebView.h
 *
 *  Created on: Sep 23, 2016
 *      Author: lion
 */

#include "ifs/WebView.h"
#include <comdef.h>
#include <Exdisp.h>
#include <mshtmhst.h>

#ifndef WEBVIEW_H_
#define WEBVIEW_H_

namespace fibjs
{

class WebView : public WebView_base,
	public IOleClientSite,
	public IOleInPlaceSite,
	public IDocHostUIHandler,
	public IServiceProvider,
	public IInternetSecurityManager
{
public:
	WebView(exlib::string url, exlib::string title, AsyncEvent* ac = NULL);
	~WebView();

	static RECT PixelToHiMetric(const RECT& _rc);

	virtual void SetRect(const RECT& _rc);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg,
	                                WPARAM wParam, LPARAM lParam);

	EVENT_SUPPORT();

public:
	// WebView_base
	virtual result_t close(AsyncEvent* ac);
	virtual result_t onclose(v8::Local<v8::Function> func, int32_t& retVal);

private:
	void GoBack();
	void GoForward();
	void Refresh();
	void Navigate(exlib::string szUrl);

public:
	// IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void**ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

public:
	// IOleWindow
	virtual HRESULT STDMETHODCALLTYPE GetWindow(HWND *phwnd);
	virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);

public:
	// IOleInPlaceSite
	virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void);
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivate(void);
	virtual HRESULT STDMETHODCALLTYPE OnUIActivate(void);
	virtual HRESULT STDMETHODCALLTYPE GetWindowContext(IOleInPlaceFrame **ppFrame,
	        IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect,
	        LPOLEINPLACEFRAMEINFO lpFrameInfo);
	virtual HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtant);
	virtual HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable);
	virtual HWND GetControlWindow();
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(void);
	virtual HRESULT STDMETHODCALLTYPE DiscardUndoState(void);
	virtual HRESULT STDMETHODCALLTYPE DeactivateAndUndo(void);
	virtual HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT lprcPosRect);

public:
	// IOleClientSite
	virtual HRESULT STDMETHODCALLTYPE SaveObject(void);
	virtual HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker,
	        IMoniker **ppmk);
	virtual HRESULT STDMETHODCALLTYPE GetContainer(IOleContainer **ppContainer);
	virtual HRESULT STDMETHODCALLTYPE ShowObject(void);
	virtual HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL fShow);
	virtual HRESULT STDMETHODCALLTYPE RequestNewObjectLayout(void);

public:
	// IDocHostUIHandler
	virtual HRESULT STDMETHODCALLTYPE ShowContextMenu(DWORD dwID, POINT* ppt, IUnknown* pcmdtReserved,
	        IDispatch* pdispReserved);
	virtual HRESULT STDMETHODCALLTYPE GetHostInfo(DOCHOSTUIINFO* pInfo);
	virtual HRESULT STDMETHODCALLTYPE ShowUI(DWORD dwID, IOleInPlaceActiveObject* pActiveObject,
	        IOleCommandTarget* pCommandTarget, IOleInPlaceFrame* pFrame,
	        IOleInPlaceUIWindow* pDoc);
	virtual HRESULT STDMETHODCALLTYPE HideUI(void);
	virtual HRESULT STDMETHODCALLTYPE UpdateUI(void);
	virtual HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable);
	virtual HRESULT STDMETHODCALLTYPE OnDocWindowActivate(BOOL fActivate);
	virtual HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(BOOL fActivate);
	virtual HRESULT STDMETHODCALLTYPE ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow* pUIWindow,
	        BOOL fRameWindow);
	virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpMsg, const GUID* pguidCmdGroup,
	        DWORD nCmdID);
	virtual HRESULT STDMETHODCALLTYPE GetOptionKeyPath(LPOLESTR* pchKey, DWORD dw);
	virtual HRESULT STDMETHODCALLTYPE GetDropTarget(IDropTarget* pDropTarget, IDropTarget** ppDropTarget);
	virtual HRESULT STDMETHODCALLTYPE GetExternal(IDispatch** ppDispatch);
	virtual HRESULT STDMETHODCALLTYPE TranslateUrl(DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut);
	virtual HRESULT STDMETHODCALLTYPE FilterDataObject(IDataObject* pDO, IDataObject** ppDORet);

public:
	// IServiceProvider
	virtual HRESULT STDMETHODCALLTYPE QueryService(REFGUID siid, REFIID riid,
	        void **ppvObject);

public:
	// IInternetSecurityManager
	virtual HRESULT STDMETHODCALLTYPE SetSecuritySite(IInternetSecurityMgrSite *pSite);
	virtual HRESULT STDMETHODCALLTYPE GetSecuritySite(IInternetSecurityMgrSite **ppSite);
	virtual HRESULT STDMETHODCALLTYPE MapUrlToZone(LPCWSTR pwszUrl, DWORD *pdwZone,
	        DWORD dwFlags);
	virtual HRESULT STDMETHODCALLTYPE GetSecurityId(LPCWSTR pwszUrl, BYTE *pbSecurityId,
	        DWORD *pcbSecurityId, DWORD_PTR dwReserved);
	virtual HRESULT STDMETHODCALLTYPE ProcessUrlAction(LPCWSTR pwszUrl, DWORD dwAction,
	        BYTE *pPolicy, DWORD cbPolicy, BYTE *pContext, DWORD cbContext,
	        DWORD dwFlags, DWORD dwReserved);
	virtual HRESULT STDMETHODCALLTYPE QueryCustomPolicy(LPCWSTR pwszUrl, REFGUID guidKey,
	        BYTE **ppPolicy, DWORD *pcbPolicy, BYTE *pContext, DWORD cbContext,
	        DWORD dwReserved);
	virtual HRESULT STDMETHODCALLTYPE SetZoneMapping(DWORD dwZone, LPCWSTR lpszPattern,
	        DWORD dwFlags);
	virtual HRESULT STDMETHODCALLTYPE GetZoneMappings(DWORD dwZone, IEnumString **ppenumString,
	        DWORD dwFlags);

private:
	void clear();

protected:
	IStorage *storage;
	IOleObject* oleObject;
	IOleInPlaceObject* oleInPlaceObject;
	IWebBrowser2* webBrowser2;

	RECT rObject;

	HWND hWndParent;
	HWND hWndControl;

	AsyncEvent* m_ac;
};

} /* namespace fibjs */
#endif /* WEBVIEW_H_ */
