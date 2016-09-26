/*
 * WebView.h
 *
 *  Created on: Sep 23, 2016
 *      Author: lion
 */

#include "ifs/WebView.h"
#include <comdef.h>
#include <Exdisp.h>

#ifndef WEBVIEW_H_
#define WEBVIEW_H_

namespace fibjs
{

class WebView : public WebView_base,
	public IOleClientSite,
	public IOleInPlaceSite,
	public IStorage
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
	// IStorage
	virtual HRESULT STDMETHODCALLTYPE CreateStream(const OLECHAR *pwcsName,
	        DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm);
	virtual HRESULT STDMETHODCALLTYPE OpenStream(const OLECHAR *pwcsName,
	        void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm);
	virtual HRESULT STDMETHODCALLTYPE CreateStorage(const OLECHAR *pwcsName,
	        DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg);
	virtual HRESULT STDMETHODCALLTYPE OpenStorage(const OLECHAR *pwcsName,
	        IStorage *pstgPriority, DWORD grfMode, SNB snbExclude,
	        DWORD reserved, IStorage **ppstg);
	virtual HRESULT STDMETHODCALLTYPE CopyTo(DWORD ciidExclude,
	        const IID *rgiidExclude, SNB snbExclude, IStorage *pstgDest);
	virtual HRESULT STDMETHODCALLTYPE MoveElementTo(const OLECHAR *pwcsName,
	        IStorage *pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags);
	virtual HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags);
	virtual HRESULT STDMETHODCALLTYPE Revert(void);
	virtual HRESULT STDMETHODCALLTYPE EnumElements(DWORD reserved1,
	        void *reserved2, DWORD reserved3, IEnumSTATSTG **ppenum);
	virtual HRESULT STDMETHODCALLTYPE DestroyElement(const OLECHAR *pwcsName);
	virtual HRESULT STDMETHODCALLTYPE RenameElement(const OLECHAR *pwcsOldName,
	        const OLECHAR *pwcsNewName);
	virtual HRESULT STDMETHODCALLTYPE SetElementTimes(const OLECHAR *pwcsName,
	        const FILETIME *pctime, const FILETIME *patime, const FILETIME *pmtime);
	virtual HRESULT STDMETHODCALLTYPE SetClass(REFCLSID clsid);
	virtual HRESULT STDMETHODCALLTYPE SetStateBits(DWORD grfStateBits, DWORD grfMask);
	virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG *pstatstg, DWORD grfStatFlag);

private:
	void clear();

protected:
	IOleObject* oleObject;
	IOleInPlaceObject* oleInPlaceObject;
	IWebBrowser2* webBrowser2;

	RECT rObject;

	HWND hWndParent;
	HWND hWndControl;
};

} /* namespace fibjs */
#endif /* WEBVIEW_H_ */
