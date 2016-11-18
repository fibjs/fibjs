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
	public IDispatch,
	public IOleClientSite,
	public IOleInPlaceSite,
	public IDocHostUIHandler,
	public IServiceProvider,
	public IInternetSecurityManager
{
public:
	WebView(exlib::string url, exlib::string title);
	~WebView();

	static RECT PixelToHiMetric(const RECT& _rc);

	virtual void SetRect(const RECT& _rc);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg,
	                                WPARAM wParam, LPARAM lParam);

	EVENT_SUPPORT();

public:
	// WebView_base
	virtual result_t close(AsyncEvent* ac);
	virtual result_t wait(AsyncEvent* ac);
	virtual result_t onclose(v8::Local<v8::Function> func, int32_t& retVal);

private:
	void GoBack();
	void GoForward();
	void Refresh();
	void Navigate(exlib::string szUrl);

public:
	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void**ppvObject);
	STDMETHODIMP_(ULONG) AddRef(void);
	STDMETHODIMP_(ULONG) Release(void);

public:
	// IDispatch
	STDMETHODIMP GetIDsOfNames(REFIID riid, OLECHAR** rgszNames, unsigned int cNames, LCID lcid, DISPID* rgdispid);
	STDMETHODIMP GetTypeInfo(unsigned int itinfo, LCID lcid, ITypeInfo** pptinfo);
	STDMETHODIMP GetTypeInfoCount(unsigned int* pctinfo);
	STDMETHODIMP Invoke(DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult, EXCEPINFO* pexecinfo, unsigned int* puArgErr);

public:
	// IOleWindow
	STDMETHODIMP GetWindow(HWND *phwnd);
	STDMETHODIMP ContextSensitiveHelp(BOOL fEnterMode);

public:
	// IOleInPlaceSite
	STDMETHODIMP CanInPlaceActivate(void);
	STDMETHODIMP OnInPlaceActivate(void);
	STDMETHODIMP OnUIActivate(void);
	STDMETHODIMP GetWindowContext(IOleInPlaceFrame **ppFrame,
	                              IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect,
	                              LPOLEINPLACEFRAMEINFO lpFrameInfo);
	STDMETHODIMP Scroll(SIZE scrollExtant);
	STDMETHODIMP OnUIDeactivate(BOOL fUndoable);
	virtual HWND GetControlWindow();
	STDMETHODIMP OnInPlaceDeactivate(void);
	STDMETHODIMP DiscardUndoState(void);
	STDMETHODIMP DeactivateAndUndo(void);
	STDMETHODIMP OnPosRectChange(LPCRECT lprcPosRect);

public:
	// IOleClientSite
	STDMETHODIMP SaveObject(void);
	STDMETHODIMP GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker,
	                        IMoniker **ppmk);
	STDMETHODIMP GetContainer(IOleContainer **ppContainer);
	STDMETHODIMP ShowObject(void);
	STDMETHODIMP OnShowWindow(BOOL fShow);
	STDMETHODIMP RequestNewObjectLayout(void);

public:
	// IDocHostUIHandler
	STDMETHODIMP ShowContextMenu(DWORD dwID, POINT* ppt, IUnknown* pcmdtReserved,
	                             IDispatch* pdispReserved);
	STDMETHODIMP GetHostInfo(DOCHOSTUIINFO* pInfo);
	STDMETHODIMP ShowUI(DWORD dwID, IOleInPlaceActiveObject* pActiveObject,
	                    IOleCommandTarget* pCommandTarget, IOleInPlaceFrame* pFrame,
	                    IOleInPlaceUIWindow* pDoc);
	STDMETHODIMP HideUI(void);
	STDMETHODIMP UpdateUI(void);
	STDMETHODIMP EnableModeless(BOOL fEnable);
	STDMETHODIMP OnDocWindowActivate(BOOL fActivate);
	STDMETHODIMP OnFrameWindowActivate(BOOL fActivate);
	STDMETHODIMP ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow* pUIWindow,
	                          BOOL fRameWindow);
	STDMETHODIMP TranslateAccelerator(LPMSG lpMsg, const GUID* pguidCmdGroup,
	                                  DWORD nCmdID);
	STDMETHODIMP GetOptionKeyPath(LPOLESTR* pchKey, DWORD dw);
	STDMETHODIMP GetDropTarget(IDropTarget* pDropTarget, IDropTarget** ppDropTarget);
	STDMETHODIMP GetExternal(IDispatch** ppDispatch);
	STDMETHODIMP TranslateUrl(DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut);
	STDMETHODIMP FilterDataObject(IDataObject* pDO, IDataObject** ppDORet);

public:
	// IServiceProvider
	STDMETHODIMP QueryService(REFGUID siid, REFIID riid,
	                          void **ppvObject);

public:
	// IInternetSecurityManager
	STDMETHODIMP SetSecuritySite(IInternetSecurityMgrSite *pSite);
	STDMETHODIMP GetSecuritySite(IInternetSecurityMgrSite **ppSite);
	STDMETHODIMP MapUrlToZone(LPCWSTR pwszUrl, DWORD *pdwZone,
	                          DWORD dwFlags);
	STDMETHODIMP GetSecurityId(LPCWSTR pwszUrl, BYTE *pbSecurityId,
	                           DWORD *pcbSecurityId, DWORD_PTR dwReserved);
	STDMETHODIMP ProcessUrlAction(LPCWSTR pwszUrl, DWORD dwAction,
	                              BYTE *pPolicy, DWORD cbPolicy, BYTE *pContext, DWORD cbContext,
	                              DWORD dwFlags, DWORD dwReserved);
	STDMETHODIMP QueryCustomPolicy(LPCWSTR pwszUrl, REFGUID guidKey,
	                               BYTE **ppPolicy, DWORD *pcbPolicy, BYTE *pContext, DWORD cbContext,
	                               DWORD dwReserved);
	STDMETHODIMP SetZoneMapping(DWORD dwZone, LPCWSTR lpszPattern,
	                            DWORD dwFlags);
	STDMETHODIMP GetZoneMappings(DWORD dwZone, IEnumString **ppenumString,
	                             DWORD dwFlags);

private:
	void clear();
	void OnBeforeNavigate2(DISPPARAMS* pDispParams);
	void OnCommandStateChange(DISPPARAMS* pDispParams);
	void OnDocumentBegin(DISPPARAMS* pDispParams);
	void OnDocumentComplete(DISPPARAMS* pDispParams);
	void OnDownloadBegin(DISPPARAMS* pDispParams);
	void OnDownloadComplete(DISPPARAMS* pDispParams);
	void OnNavigateComplete2(DISPPARAMS* pDispParams);
	void OnNewWindow2(DISPPARAMS* pDispParams);
	void OnProgressChange(DISPPARAMS* pDispParams);
	void OnPropertyChange(DISPPARAMS* pDispParams);
	void OnStatusTextChange(DISPPARAMS* pDispParams);
	void OnTitleChange(DISPPARAMS* pDispParams);

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
