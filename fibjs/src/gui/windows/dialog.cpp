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

result_t gui_base::chooseFile(ChooseFileOptions* options, std::vector<exlib::string>& retVal)
{
    result_t gui_hr = check_gui_available();
    if (gui_hr.failed())
        return gui_hr;

    gui_prepare_call();

    // The modal runs inline on the host (GUI) thread, see input.cpp.
    HRESULT hr = 0;
    result_t err;

    ChooseFileOptions* opts = options;

    bool isSaveDialog = opts->type.value() == "saveFile";

    IFileDialog* pfd = nullptr;
    if (isSaveDialog)
        hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
    else
        hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
    if (FAILED(hr)) {
        err = result_t::make(CALL_E_INVALID_CALL).with_type_name(errtype::kError).with_code("ERR_NOT_SUPPORTED").with_message("failed to create the file dialog");
        return err;
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
            err = result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("invalid dialog type: " + opts->type.value());
        return err;
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
            err = result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("invalid defaultPath: " + opts->defaultPath.value());
        return err;
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
            DialogFilter* filter = opts->filters->at(i);
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
        err = result_t::return_undefined();
        return err;
    }

    if (FAILED(hr)) {
        pfd->Release();
        err = result_t::make(CALL_E_INVALID_CALL).with_type_name(errtype::kError).with_code("ERR_NOT_SUPPORTED").with_message("failed to show the file dialog");
        return err;
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
                retVal.clear();
                for (DWORD i = 0; i < count; ++i) {
                    IShellItem* pItem;
                    hr = pResults->GetItemAt(i, &pItem);
                    if (SUCCEEDED(hr)) {
                        PWSTR pszFilePath;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                        if (SUCCEEDED(hr)) {
                            retVal.push_back(utf16to8String((const char16_t*)pszFilePath));
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
                    retVal.clear();
                    retVal.push_back(utf16to8String((const char16_t*)pszFilePath));
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }
    }

    pfd->Release();

    return err;
}

}

#endif
