/*
 * dialog.mm
 *
 *  Created on: Oct 29, 2024
 *      Author: lion
 */

#ifdef _WIN32

#include "object.h"
#include "ifs/gui.h"
#include "../gui.h"
#include <windows.h>
#include <shobjidl.h>

namespace fibjs {

result_t gui_base::chooseFile(v8::Local<v8::Object> options, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        Isolate* isolate = Isolate::current(options);

        obj_ptr<DialogOptions> opts;
        result_t hr = DialogOptions::load(options, opts);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = opts;

        return CHECK_ERROR(CALL_E_GUICALL);
    }

    HRESULT hr = 0;
    exlib::Event ev;
    async([&hr, &ev, &retVal, &ac]() {
        DialogOptions* opts = (DialogOptions*)ac->m_ctx[0].object();

        bool isSaveDialog = opts->type.value() == "saveFile";

        IFileDialog* pfd = nullptr;
        if (isSaveDialog)
            hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
        else
            hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
        if (FAILED(hr)) {
            ev.set();
            return;
        }

        DWORD dwFlags;
        pfd->GetOptions(&dwFlags);

        if (isSaveDialog) {
            dwFlags |= FOS_FORCEFILESYSTEM;
        } else {
            if (opts->type.value() == "openDirectory")
                dwFlags |= FOS_PICKFOLDERS;
            else if (opts->type.value() == "openFile")
                dwFlags |= FOS_FORCEFILESYSTEM;
            else {
                pfd->Release();
                hr = Runtime::setError("invalid dialog type: " + opts->type.value());
                ev.set();
                return;
            }

            if (opts->multiSelections.value())
                dwFlags |= FOS_ALLOWMULTISELECT;
        }

        pfd->SetOptions(dwFlags);

        if (opts->title.has_value()) {
            pfd->SetTitle((LPCWSTR)utf8to16String(opts->title.value()).c_str());
        }

        if (opts->defaultPath.has_value()) {
            IShellItem* psiFolder;
            hr = SHCreateItemFromParsingName((LPCWSTR)utf8to16String(opts->defaultPath.value()).c_str(), nullptr, IID_PPV_ARGS(&psiFolder));
            if (FAILED(hr)) {
                pfd->Release();
                hr = Runtime::setError("invalid defaultPath: " + opts->defaultPath.value());
                ev.set();
                return;
            }

            pfd->SetDefaultFolder(psiFolder);
            psiFolder->Release();
        }

        if (opts->filters.has_value()) {
            std::vector<COMDLG_FILTERSPEC> rgSpec;

            std::vector<exlib::wstring> names;
            names.resize(opts->filters->size());

            std::vector<exlib::wstring> extensions;
            extensions.resize(opts->filters->size());

            for (int32_t i = 0; i < opts->filters->size(); ++i) {
                DialogOptions::FilterItem* filter = opts->filters->at(i);
                COMDLG_FILTERSPEC spec;
                names[i] = utf8to16String(filter->name);
                spec.pszName = (LPCWSTR)names[i].c_str();
                exlib::wstring& exts = extensions[i];
                for (auto& ext : filter->extensions) {
                    if (!exts.empty()) {
                        exts.append(1, ';');
                    }
                    exts.append((const char16_t*)L"*.");
                    exts.append(utf8to16String(ext));
                }
                spec.pszSpec = (LPCWSTR)exts.c_str();
                rgSpec.push_back(spec);
            }
            pfd->SetFileTypes(rgSpec.size(), rgSpec.data());
        }

        hr = pfd->Show(nullptr);

        if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
            pfd->Release();
            hr = CALL_RETURN_UNDEFINED;
            ev.set();
            return;
        }

        if (SUCCEEDED(hr)) {
            if (opts->multiSelections.value() && !isSaveDialog) {
                IFileOpenDialog* pOpenDialog;
                pfd->QueryInterface(IID_PPV_ARGS(&pOpenDialog));

                IShellItemArray* pResults;
                hr = pOpenDialog->GetResults(&pResults);
                if (SUCCEEDED(hr)) {
                    DWORD count;
                    pResults->GetCount(&count);
                    retVal = new NArray();
                    for (DWORD i = 0; i < count; ++i) {
                        IShellItem* pItem;
                        hr = pResults->GetItemAt(i, &pItem);
                        if (SUCCEEDED(hr)) {
                            PWSTR pszFilePath;
                            hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                            if (SUCCEEDED(hr)) {
                                retVal->append(utf16to8String((const char16_t*)pszFilePath));
                                CoTaskMemFree(pszFilePath);
                            }
                            pItem->Release();
                        }
                    }
                    pResults->Release();
                }
                pOpenDialog->Release();
            } else {
                IShellItem* pItem;
                hr = pfd->GetResult(&pItem);
                if (SUCCEEDED(hr)) {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    if (SUCCEEDED(hr)) {
                        retVal = new NArray();
                        retVal->append(utf16to8String((const char16_t*)pszFilePath));
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
        }

        pfd->Release();

        ev.set();
    },
        CALL_E_LONGSYNC);

    ev.wait();

    return hr;
}

}

#endif
