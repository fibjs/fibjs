#ifdef _WIN32

#include "WebView2Loader.h"

#include <ShObjIdl.h>
#include <appmodel.h>
#include <wrl/client.h>
#ifndef __MINGW32__
#include <wrl/implements.h>
#endif

#include <cstring>
#include <string>

#include "WebView2.h"


#pragma comment(lib, "version.lib")

namespace {

#pragma region Win32 API Pointer Types

typedef LONG(WINAPI* GetCurrentPackageInfoProc)(UINT32 flags,
                                                UINT32* bufferLength,
                                                BYTE* buffer, UINT32* count);

typedef LONG(WINAPI* GetCurrentApplicationUserModelIdProc)(
    UINT32* applicationUserModelIdLength, PWSTR applicationUserModelId);

#pragma endregion

#pragma region Enumerators

enum class WebView2RunTimeType {
  kInstalled = 0x0,
  kRedistributable = 0x1,
};

enum class WebView2ReleaseChannelPreference {
  kStable = 0x0,
  kCanary = 0x1,
};

#pragma endregion

#pragma region Globals

constexpr int kNumChannels = 5;

const wchar_t* kChannelName[kNumChannels] = {
    L"", L"beta", L"dev", L"canary", L"internal",
};

const wchar_t* kChannelUuid[kNumChannels] = {
    L"{F3017226-FE2A-4295-8BDF-00C3A9A7E4C5}",
    L"{2CD8A007-E189-409D-A2C8-9AF4EF3C72AA}",
    L"{0D50BFEC-CD6A-4F9A-964C-C7416E3ACB10}",
    L"{65C35B14-6C1D-4122-AC46-7148CC9D6497}",
    L"{BE59E8FD-089A-411B-A3B0-051D9E417818}"};

const wchar_t* kChannelPackageFamilyName[kNumChannels] = {
    L"Microsoft.WebView2Runtime.Stable_8wekyb3d8bbwe",
    L"Microsoft.WebView2Runtime.Beta_8wekyb3d8bbwe",
    L"Microsoft.WebView2Runtime.Dev_8wekyb3d8bbwe",
    L"Microsoft.WebView2Runtime.Canary_8wekyb3d8bbwe",
    L"Microsoft.WebView2Runtime.Internal_8wekyb3d8bbwe"};

constexpr wchar_t kInstallKeyPath[] =
    L"Software\\Microsoft\\EdgeUpdate\\ClientState\\";

constexpr wchar_t kRedistOverrideKey[] =
    L"Software\\Policies\\Microsoft\\Edge\\WebView2\\";

constexpr wchar_t kEmbeddedOverrideKey[] =
    L"Software\\Policies\\Microsoft\\EmbeddedBrowserWebView\\LoaderOverride\\";

constexpr UINT32 kMinimumCompatibleVersion[4] = {86, 0, 616, 0};

#if defined(_M_X64) || defined(_M_AMD64)
constexpr wchar_t kEmbeddedWebViewPath[] =
    L"EBWebView\\x64\\EmbeddedBrowserWebView.dll";
#elif defined(_M_IX86)
constexpr wchar_t kEmbeddedWebViewPath[] =
    L"EBWebView\\x86\\EmbeddedBrowserWebView.dll";
#elif defined(_M_ARM64)
constexpr wchar_t kEmbeddedWebViewPath[] =
    L"EBWebView\\arm64\\EmbeddedBrowserWebView.dll";
#endif

bool gShouldCheckPolicyOverride = true;
bool gShouldCheckRegistryOverride = true;

#pragma endregion

#pragma region Structures

struct WebView2EnvironmentParams {
  PCWSTR embeddedEdgeSubFolder;
  PCWSTR userDataDir;
  ICoreWebView2EnvironmentOptions* environmentOptions;
  WebView2ReleaseChannelPreference releaseChannelPreference;
};

#pragma endregion

#pragma region Classes

/// <summary>
/// EnvironmentCreatedRetryHandler is an implementation of
/// ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler that proxies
/// another completion handler but implements retry logic.
/// </summary>
#ifndef __MINGW32__
class EnvironmentCreatedRetryHandler
    : public Microsoft::WRL::RuntimeClass<
          Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
          ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler> {
 public:
#else
// This deviates from actual WebView2Loader, but makes it possible to compile under MinGW.
class EnvironmentCreatedRetryHandler
    : public ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler {
  unsigned __int64 volatile m_cRef = 1;
 public:
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID *ppvObj) override {
    constexpr GUID iidIUnknown = {
      0x00000000,
      0x0000,
      0x0000,
      {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

    constexpr GUID iidICoreICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler = {
      0x4e8a3389,
      0xc9d8,
      0x4bd2,
      {0xb6,0xb5,0x12,0x4f,0xee,0x6c,0xc1,0x4d}}; 

    if (!ppvObj) return E_INVALIDARG;
    *ppvObj = NULL;
    if (riid == iidIUnknown || riid == iidICoreICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler) {
      *ppvObj = (LPVOID)this;
      AddRef();
      return NOERROR;
    }
    return E_NOINTERFACE;
  }
  ULONG STDMETHODCALLTYPE AddRef() override {
    InterlockedIncrement(&m_cRef);
    return m_cRef;
  }
  ULONG STDMETHODCALLTYPE Release() override {
    ULONG ulRefCount = InterlockedDecrement(&m_cRef);
    if (m_cRef == 0) {
      delete this;
    }
    return ulRefCount;
  }
#endif
  EnvironmentCreatedRetryHandler(
      WebView2EnvironmentParams environmentParams,
      ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*
          originalHandler,
      int retries)
      : mEnvironmentParams(environmentParams),
        mOriginalHandler(originalHandler),
        mRetries(retries) {}

  HRESULT STDMETHODCALLTYPE
  Invoke(HRESULT result, ICoreWebView2Environment* createdEnvironment) override;

 private:
  WebView2EnvironmentParams mEnvironmentParams;
  Microsoft::WRL::ComPtr<
      ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>
      mOriginalHandler;
  int mRetries;
};

HRESULT TryCreateWebViewEnvironment(
    WebView2EnvironmentParams params,
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*
        environmentCreatedHandler);

HRESULT STDMETHODCALLTYPE EnvironmentCreatedRetryHandler::Invoke(
    HRESULT result, ICoreWebView2Environment* createdEnvironment) {
  constexpr GUID iidICoreWebView2Environment = {
      0xb96d755e,
      0x0319,
      0x4e92,
      {0xa2, 0x96, 0x23, 0x43, 0x6f, 0x46, 0xa1, 0xfc}};

  if (result >= 0 || this->mRetries <= 0) {
    if (createdEnvironment) {
      // May or may not be necessary, but the official
      // WebView2Loader seems to do it.
      result = createdEnvironment->QueryInterface(
          iidICoreWebView2Environment,
          reinterpret_cast<void**>(&createdEnvironment));
    }
    this->mOriginalHandler->Invoke(result, createdEnvironment);
    if (createdEnvironment) {
      createdEnvironment->Release();
      createdEnvironment = nullptr;
    }
  } else {
    this->mRetries--;
    result = TryCreateWebViewEnvironment(this->mEnvironmentParams, this);
    if (result < 0) {
      this->mOriginalHandler->Invoke(result, nullptr);
    }
  }
  return S_OK;
}

/// <summary>
/// WString implements a simple UTF-16/WTF-16 string class. It is similar to
/// std::wstring but allows handling allocation failures exceptions.
/// </summary>
class WString {
 public:
  WString() = default;
  ~WString() { Release(); }

  WString(const WString&) = delete;
  WString(WString&&) = delete;

  WString& operator=(const WString&) = delete;
  WString& operator=(WString&&) = delete;

  /// <summary>
  /// Releases the buffer and clears the string.
  /// </summary>
  void Release() {
    this->mLength = 0;
    this->mCapacity = 0;
    if (this->mData) {
      *this->mData = 0;
      delete[] this->mData;
      this->mData = nullptr;
    }
  }

  /// <summary>
  /// Reserves enough buffer to store `len` characters.
  /// </summary>
  /// <param name="len">
  /// Length in characters (sans the null terminator.)
  /// </param>
  /// <returns>
  /// True if successful, false if an allocation error occurs.
  /// </returns>
  bool Reserve(size_t len) {
    if (this->mCapacity >= len) {
      return true;
    }

    auto* newString = new wchar_t[len + 1];
    if (this->mLength) {
      std::memcpy(newString, this->mData, this->mLength * 2 + 2);
    } else {
      *newString = 0;
    }

    delete[] this->mData;

    this->mData = newString;
    this->mCapacity = len;

    return true;
  }

  /// <summary>
  /// Assigns the null-terminated string `src` to this instance.
  /// </summary>
  /// <param name="src">A null-terminated string.</param>
  /// <returns>
  /// True if successful, false if an allocation error occurs.
  /// </returns>
  bool Assign(const wchar_t* src) { return Assign(src, src ? wcslen(src) : 0); }

  /// <summary>
  /// Assigns the string `src` with `len` bytes to this instance.
  /// </summary>
  /// <param name="src">A pointer to string data.</param>
  /// <param name="len">Length in characters to copy.</param>
  /// <returns>
  /// True if successful, false if an allocation error occurs.
  /// </returns>
  bool Assign(const wchar_t* src, size_t len) {
    this->mLength = 0;
    if (this->mData) {
      *this->mData = 0;
    }
    if (!src) {
      return true;
    }
    if (!Reserve(len)) {
      return false;
    }
    std::memcpy(this->mData, src, len * 2);
    if (this->mCapacity >= len) {
      this->mLength = len;
      if (this->mData) {
        this->mData[len] = 0;
      }
      return true;
    }
    return false;
  }

  /// <summary>
  /// Clears the string without releasing the underlying buffer.
  /// </summary>
  void Clear() {
    this->mLength = 0;
    if (this->mData) {
      *this->mData = 0;
    }
  }

  /// <summary>
  /// Resizes the string to the given length.
  /// </summary>
  /// <param name="len">
  /// Length in characters (sans the null terminator.)
  /// </param>
  /// <returns>True if successful, false otherwise.</returns>
  bool Resize(size_t len) {
    if (len > this->mCapacity) {
      if (!Reserve(len)) {
        return false;
      }
    }
    this->mLength = len;
    if (this->mData) {
      this->mData[len] = 0;
    }
    return true;
  }

  /// <summary>
  /// Appends the null-terminated string `src` to this instance.
  /// </summary>
  /// <param name="src">A null-terminated string.</param>
  /// <returns>
  /// True if successful, false if an allocation error occurs.
  /// </returns>
  bool Append(const wchar_t* src) { return Append(src, src ? wcslen(src) : 0); }

  /// <summary>
  /// Appends the string `src` with `len` bytes to this instance.
  /// </summary>
  /// <param name="src">A pointer to string data.</param>
  /// <param name="len">Length in characters to copy.</param>
  /// <returns>
  /// True if successful, false if an allocation error occurs.
  /// </returns>
  bool Append(const wchar_t* src, size_t len) {
    if (!src) {
      return true;
    }
    size_t newLen = this->mLength + len;
    if (!Reserve(newLen)) {
      return false;
    }
    std::memcpy(&this->mData[this->mLength], src, len * 2);
    if (this->mCapacity >= newLen) {
      this->mLength = newLen;
      if (this->mData) {
        this->mData[newLen] = 0;
      }
      return true;
    }
    return false;
  }

  /// <summary>
  /// Like Reserve, but works on a count of bytes.
  /// </summary>
  /// <param name="len">Number of bytes of storage to reserve.</param>
  /// <returns>
  /// True if successful, false if an allocation error occurs.
  /// </returns>
  bool ReserveBuffer(size_t len) { return Reserve((len >> 1) + 1); }

  [[nodiscard]] size_t Capacity() const { return this->mCapacity; }

  [[nodiscard]] size_t Length() const { return this->mLength; }

  [[nodiscard]] bool Empty() const { return this->mLength == 0; }

  [[nodiscard]] PVOID Data() const { return mData; }

  [[nodiscard]] PWSTR String() const { return mData; }

 private:
  size_t mLength = 0;
  size_t mCapacity = 0;
  wchar_t* mData = nullptr;
};

#pragma endregion

#pragma region Functions

int GetAppUserModelIdForCurrentProcess(WString* idOut) {
  auto* lpGetCurrentApplicationUserModelId =
      reinterpret_cast<GetCurrentApplicationUserModelIdProc>(
          GetProcAddress(GetModuleHandleW(L"Kernel32.dll"),
                         "GetCurrentApplicationUserModelId"));

  // Win8+: Use GetCurrentApplicationUserModelId.
  if (lpGetCurrentApplicationUserModelId) {
    idOut->Reserve(0x100);
    auto idLength = static_cast<UINT32>(idOut->Capacity());
    if (!lpGetCurrentApplicationUserModelId(&idLength, idOut->String())) {
      if (!idOut->Resize(idLength - 1)) {
        return E_UNEXPECTED;
      }
      return S_OK;
    }
  }

  // Win7: Use GetCurrentProcessExplicitAppUserModelID.
  PWSTR appId = nullptr;
  HRESULT hr = GetCurrentProcessExplicitAppUserModelID(&appId);
  if (FAILED(hr)) {
    CoTaskMemFree(appId);
    appId = nullptr;
    return hr;
  }
  if (!idOut->Assign(appId)) {
    CoTaskMemFree(appId);
    appId = nullptr;
    return HRESULT_FROM_WIN32(GetLastError());
  }
  CoTaskMemFree(appId);
  appId = nullptr;
  return S_OK;
}

HRESULT GetModulePath(HMODULE hModule, WString* outPath) {
  outPath->Reserve(MAX_PATH);
  DWORD result = GetModuleFileNameW(hModule, outPath->String(),
                                    static_cast<DWORD>(outPath->Capacity()));
  if (result == outPath->Capacity() &&
      GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
    outPath->Reserve(0x1000u);
    result = GetModuleFileNameW(hModule, outPath->String(),
                                static_cast<DWORD>(outPath->Capacity()));
  }
  if (!result || result >= outPath->Capacity()) {
    return HRESULT_FROM_WIN32(GetLastError());
  }
  outPath->Resize(result);
  return S_OK;
}

wchar_t* MakeCoMemString(PCWSTR str, SIZE_T len) {
  SIZE_T byteLen = len * 2 + 2;

  // Overflow check
  if (byteLen < len) return nullptr;

  auto* coMem = static_cast<LPWSTR>(CoTaskMemAlloc(byteLen));
  if (coMem) CopyMemory(coMem, str, byteLen);
  return coMem;
}

int DoesPolicyExistInRoot(HKEY hKey) {
  HKEY phkResult = nullptr;
  LSTATUS result =
      RegOpenKeyExW(hKey, L"Software\\Policies\\Microsoft\\Edge\\WebView2\\", 0,
                    0x20019u, &phkResult);
  RegCloseKey(phkResult);
  return result == ERROR_SUCCESS;
}

bool ReadEnvironmentVariable(LPCWSTR lpName, WString* outValue) {
  DWORD len = GetEnvironmentVariableW(lpName, nullptr, 0);
  if (!len || !outValue->Reserve(len)) return false;
  return outValue->Resize(
      GetEnvironmentVariableW(lpName, outValue->String(), len));
}

bool RegGetString(const WCHAR* lpValue, HKEY key, PCWSTR* outBuf,
                  WString* outStr) {
  DWORD pcbData;
  wchar_t pvData[MAX_PATH];

  memset(pvData, 0, sizeof(pvData));
  pcbData = MAX_PATH;
  LSTATUS lStatus =
      RegGetValueW(key, nullptr, lpValue, 2u, nullptr, pvData, &pcbData);
  if (lStatus != ERROR_SUCCESS) {
    return false;
  }
  outStr->Assign(pvData);
  *outBuf = outStr->String();
  return true;
}

bool ReadOverrideFromRegistry(PCWSTR key, HKEY root, const WCHAR* lpValue,
                              PCWSTR* outBuf, WString* outStr, DWORD* outInt,
                              bool redist) {
  DWORD pcbData;
  HKEY phkResult = nullptr;
  if (!key || !*key) return false;
  WString pvData;
  if (redist)
    pvData.Append(kRedistOverrideKey);
  else
    pvData.Append(kEmbeddedOverrideKey);
  pvData.Append(key);
  LSTATUS lStatus =
      RegOpenKeyExW(root, pvData.String(), 0, KEY_QUERY_VALUE, &phkResult);
  if (lStatus != ERROR_SUCCESS) {
    return false;
  }
  gShouldCheckRegistryOverride = true;

  if (outInt) {
    char szData[4];
    szData[0] = 0;
    pcbData = 4;
    if (RegGetValueW(phkResult, nullptr, lpValue, RRF_RT_REG_DWORD, nullptr,
                     szData, &pcbData) == 0) {
      *outInt = szData[0] == 1;
    } else {
      if (!RegGetString(lpValue, phkResult, outBuf, outStr)) {
        RegCloseKey(phkResult);
        return false;
      }
      *outInt = wcstol(*outBuf, nullptr, 10) == 1;
    }
    RegCloseKey(phkResult);
    return true;
  }

  if (!RegGetString(lpValue, phkResult, outBuf, outStr)) {
    RegCloseKey(phkResult);
    return false;
  }

  RegCloseKey(phkResult);
  return true;
}

bool UpdateParamsWithRegOverrides(PCWSTR key, HKEY root, PCWSTR* outBuf,
                                  WString* outStr, DWORD* outInt, bool redist) {
  WString exeName, aumId, modulePath;
  GetAppUserModelIdForCurrentProcess(&aumId);
  if (FAILED(GetModulePath(nullptr, &modulePath))) {
    exeName.Clear();
  } else {
    const wchar_t* lastSlash = wcsrchr(modulePath.String(), L'\\');
    if (!lastSlash) lastSlash = modulePath.String();
    exeName.Assign(lastSlash + 1);
  }

  if (gShouldCheckPolicyOverride && redist) {
    if (ReadOverrideFromRegistry(key, root, aumId.String(), outBuf, outStr,
                                 outInt, redist))
      return true;
    if (ReadOverrideFromRegistry(key, root, exeName.String(), outBuf, outStr,
                                 outInt, redist))
      return true;
    if (ReadOverrideFromRegistry(key, root, L"*", outBuf, outStr, outInt,
                                 redist))
      return true;
    return false;
  }

  if (ReadOverrideFromRegistry(aumId.String(), root, key, outBuf, outStr,
                               outInt, redist))
    return true;
  if (ReadOverrideFromRegistry(exeName.String(), root, key, outBuf, outStr,
                               outInt, redist))
    return true;
  if (ReadOverrideFromRegistry(L"*", root, key, outBuf, outStr, outInt, redist))
    return true;
  return false;
}

bool UpdateParamsWithOverrides(const wchar_t* env, const wchar_t* key,
                               PCWSTR* outBuf, WString* outStr, DWORD* outInt,
                               bool checkOverride) {
  if (checkOverride) {
    gShouldCheckPolicyOverride = true;
    gShouldCheckRegistryOverride = true;
  }

  if (ReadEnvironmentVariable(env, outStr)) {
    *outBuf = outStr->String();
    DWORD intVal = wcstol(outStr->String(), nullptr, 10) == 1;
    if (outInt) {
      *outInt = intVal;
    }
    return intVal;
  }

  if (!gShouldCheckRegistryOverride && !gShouldCheckPolicyOverride &&
      !checkOverride) {
    return false;
  }

  gShouldCheckRegistryOverride = false;
  gShouldCheckPolicyOverride = DoesPolicyExistInRoot(HKEY_CURRENT_USER) ||
                               DoesPolicyExistInRoot(HKEY_LOCAL_MACHINE);

  return UpdateParamsWithRegOverrides(key, HKEY_LOCAL_MACHINE, outBuf, outStr,
                                      outInt, true) ||
         UpdateParamsWithRegOverrides(key, HKEY_CURRENT_USER, outBuf, outStr,
                                      outInt, true) ||
         UpdateParamsWithRegOverrides(key, HKEY_LOCAL_MACHINE, outBuf, outStr,
                                      outInt, false) ||
         UpdateParamsWithRegOverrides(key, HKEY_CURRENT_USER, outBuf, outStr,
                                      outInt, false);
}

void UpdateWebViewEnvironmentParamsWithOverrideValues(
    WebView2EnvironmentParams* params, WString* outStrings) {
  UpdateParamsWithOverrides(
      L"WEBVIEW2_BROWSER_EXECUTABLE_FOLDER", L"browserExecutableFolder",
      &params->embeddedEdgeSubFolder, &outStrings[0], nullptr, true);
  UpdateParamsWithOverrides(L"WEBVIEW2_USER_DATA_FOLDER", L"userDataFolder",
                            &params->userDataDir, &outStrings[1], nullptr,
                            false);
  const wchar_t* tmpStr = nullptr;
  WString tmpWStr;
  UpdateParamsWithOverrides(
      L"WEBVIEW2_RELEASE_CHANNEL_PREFERENCE", L"releaseChannelPreference",
      &tmpStr, &tmpWStr,
      reinterpret_cast<DWORD*>(&params->releaseChannelPreference), false);
}

HRESULT CreateWebViewEnvironmentWithClientDll(
    PCWSTR lpLibFileName, bool unknown, WebView2RunTimeType runtimeType,
    PCWSTR unknown2, IUnknown* unknown3,
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*
        envCompletedHandler) {
  HMODULE clientDll = LoadLibraryW(lpLibFileName);
  if (!clientDll) {
    return HRESULT_FROM_WIN32(GetLastError());
  }

  // This routine is not documented!
  auto createProc = reinterpret_cast<HRESULT(STDMETHODCALLTYPE*)(
      bool, WebView2RunTimeType, PCWSTR, IUnknown*,
      ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*)>(
      GetProcAddress(clientDll, "CreateWebViewEnvironmentWithOptionsInternal"));

  auto canUnloadProc = reinterpret_cast<HRESULT(STDMETHODCALLTYPE*)()>(
      GetProcAddress(clientDll, "DllCanUnloadNow"));

  if (!createProc) {
    return HRESULT_FROM_WIN32(GetLastError());
  }

  HRESULT hr =
      createProc(unknown, runtimeType, unknown2, unknown3, envCompletedHandler);

  if (canUnloadProc && SUCCEEDED(canUnloadProc())) {
    FreeLibrary(clientDll);
  }

  return hr;
}

bool FindClientDllInFolder(WString* folder) {
  folder->Append(L"\\");
  folder->Append(kEmbeddedWebViewPath);
  return GetFileAttributesW(folder->String()) != INVALID_FILE_ATTRIBUTES;
}

void GetInstallKeyPathForChannel(DWORD channel, WString* outRegistryKey) {
  const auto* guid = kChannelUuid[channel];
  outRegistryKey->Reserve(wcslen(guid) + wcslen(kInstallKeyPath));
  outRegistryKey->Assign(L"Software\\Microsoft\\EdgeUpdate\\ClientState\\",
                         wcslen(kInstallKeyPath));
  outRegistryKey->Append(guid, wcslen(guid));
}

bool CheckVersionAndFindClientDllInFolder(const UINT32* version,
                                          WString* path) {
  for (int component = 0; component < 4; component++) {
    if (version[component] < kMinimumCompatibleVersion[component]) {
      return false;
    }
    if (version[component] > kMinimumCompatibleVersion[component]) {
      break;
    }
  }
  return FindClientDllInFolder(path);
}

bool ParseVersionNumbers(PCWSTR versionString, UINT32* outVersion) {
  const wchar_t* start = versionString;
  wchar_t* end = nullptr;
  for (int i = 0; i < 4; i++) {
    outVersion[i] = wcstol(start, &end, 10);
    if (!end || start == end) {
      return false;
    }
    if (*end == L'.') {
      start = end + 1;
    }
  }
  return true;
}

int FindInstalledClientDllForChannel(PCWSTR lpSubKey, bool system,
                                     WString* versionStr, WString* clientPath) {
  HKEY phkResult;
  DWORD cbPath = MAX_PATH;
  UINT32 version[4];
  wchar_t path[MAX_PATH];

  if (RegOpenKeyExW(system ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER, lpSubKey,
                    0, KEY_READ | KEY_WOW64_32KEY, &phkResult)) {
    return false;
  }
  LSTATUS result = RegQueryValueExW(phkResult, L"EBWebView", nullptr, nullptr,
                                    reinterpret_cast<LPBYTE>(path), &cbPath);
  RegCloseKey(phkResult);
  if (result) {
    return false;
  }
  clientPath->Assign(path);
  const wchar_t* versionPart = wcsrchr(clientPath->String(), '\\');
  if (!versionPart) {
    return false;
  }
  if (!ParseVersionNumbers(versionPart + 1, version)) {
    return false;
  }
  if (versionStr) {
    versionStr->Assign(versionPart + 1);
  }
  return CheckVersionAndFindClientDllInFolder(version, clientPath);
}

int FindInstalledClientDll(WString* clientPath,
                           WebView2ReleaseChannelPreference preference,
                           WString* versionStr, WString* channelStr) {
  DWORD channel = 0;
  WString lpSubKey;
  UINT32 version[4];
  WString pkgBuf;

  static auto getCurrentPackageInfo =
      reinterpret_cast<GetCurrentPackageInfoProc>(GetProcAddress(
          GetModuleHandleW(L"kernelbase.dll"), "GetCurrentPackageInfo"));

  for (int i = 0; i < kNumChannels; i++) {
    channel =
        preference == WebView2ReleaseChannelPreference::kCanary ? 4 - i : i;
    GetInstallKeyPathForChannel(channel, &lpSubKey);
    if (FindInstalledClientDllForChannel(lpSubKey.String(), false, versionStr,
                                         clientPath)) {
      break;
    }
    if (FindInstalledClientDllForChannel(lpSubKey.String(), true, versionStr,
                                         clientPath)) {
      break;
    }
    if (!getCurrentPackageInfo) {
      continue;
    }
    unsigned int cPackages;
    unsigned int len = 0;
    if (getCurrentPackageInfo(1, &len, nullptr, &cPackages) !=
        ERROR_INSUFFICIENT_BUFFER) {
      continue;
    }
    if (!len || !pkgBuf.ReserveBuffer(len)) {
      continue;
    }
    if (getCurrentPackageInfo(1, &len, static_cast<BYTE*>(pkgBuf.Data()),
                              &cPackages)) {
      continue;
    }
    if (!cPackages) {
      continue;
    }
    auto* packages = static_cast<PACKAGE_INFO*>(pkgBuf.Data());
    PACKAGE_INFO* package = nullptr;
    for (UINT32 j = 0; j < cPackages; j++) {
      if (_wcsicmp(packages[j].packageFamilyName,
                   kChannelPackageFamilyName[channel]) == 0) {
        package = &packages[j];
        break;
      }
    }
    if (package == nullptr) {
      continue;
    }
    version[0] = package->packageId.version.Major;
    version[1] = package->packageId.version.Minor;
    version[2] = package->packageId.version.Build;
    version[3] = package->packageId.version.Revision;
    clientPath->Assign(package->path);
    if (CheckVersionAndFindClientDllInFolder(version, clientPath)) {
      if (versionStr) {
        wchar_t buffer[12] = {0};
        versionStr->Reserve(15);
        if (_ultow_s(version[0], buffer, _countof(buffer) - 1, 10)) {
          continue;
        }
        versionStr->Assign(buffer);
        if (_ultow_s(version[1], buffer, _countof(buffer) - 1, 10)) {
          continue;
        }
        versionStr->Append(L".");
        versionStr->Append(buffer);
        if (_ultow_s(version[2], buffer, _countof(buffer) - 1, 10)) {
          continue;
        }
        versionStr->Append(L".");
        versionStr->Append(buffer);
        if (_ultow_s(version[3], buffer, _countof(buffer) - 1, 10)) {
          continue;
        }
        versionStr->Append(L".");
        versionStr->Append(buffer);
      }
      break;
    }
  }
  if (channelStr) {
    channelStr->Assign(kChannelName[channel]);
  }
  return 0;
}

HRESULT FindEmbeddedClientDll(const wchar_t* embeddedEdgeSubFolder,
                              WString* outClientPath) {
  outClientPath->Reserve(MAX_PATH);
  outClientPath->Assign(embeddedEdgeSubFolder);
  const wchar_t* path = outClientPath->String();
  if (outClientPath->Length() >= 3 &&
      static_cast<wchar_t>((path[0] & 0xFFDF) - L'A') < 0x1A &&
      path[1] == L':' && path[2] == L'\\') {
    if (!FindClientDllInFolder(outClientPath)) {
      return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }
    return S_OK;
  }
  if (outClientPath->Length() < 3 || path[1] == ':' || path[0] != L'\\' ||
      path[1] != L'\\') {
    WString modulePath;
    HRESULT hr = GetModulePath(nullptr, &modulePath);
    if (hr < 0) {
      return hr;
    }
    outClientPath->Assign(modulePath.String(), modulePath.Length());
    const wchar_t* basenameSlash = wcsrchr(modulePath.String(), L'\\');
    if (!basenameSlash) return E_FAIL;
    outClientPath->Assign(modulePath.String(),
                          basenameSlash - modulePath.String() + 1);
    outClientPath->Append(embeddedEdgeSubFolder);
    if (!FindClientDllInFolder(outClientPath)) {
      return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }
    return S_OK;
  }
  if (!FindClientDllInFolder(outClientPath)) {
    return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
  }
  return S_OK;
}

HRESULT TryCreateWebViewEnvironment(
    WebView2EnvironmentParams params,
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*
        environmentCreatedHandler) {
  WebView2RunTimeType runtimeType;
  HRESULT hr;
  WString dllPath;

  if (params.embeddedEdgeSubFolder && *params.embeddedEdgeSubFolder) {
    runtimeType = WebView2RunTimeType::kRedistributable;
    hr = FindEmbeddedClientDll(params.embeddedEdgeSubFolder, &dllPath);
  } else {
    runtimeType = WebView2RunTimeType::kInstalled;
    hr = FindInstalledClientDll(&dllPath, params.releaseChannelPreference,
                                nullptr, nullptr);
  }
  if (FAILED(hr)) {
    return hr;
  }
  return CreateWebViewEnvironmentWithClientDll(
      dllPath.String(), true, runtimeType, params.userDataDir,
      params.environmentOptions, environmentCreatedHandler);
}

HRESULT FindEmbeddedBrowserVersion(LPCWSTR filename, WString* outBuf) {
  unsigned int puLen = 0;
  LPWSTR lpBuffer = nullptr;
  DWORD dwHandle = 0;
  DWORD cbVerInfo = GetFileVersionInfoSizeW(filename, &dwHandle);
  WString verInfoBuffer;

  if (!cbVerInfo) {
    return HRESULT_FROM_WIN32(GetLastError());
  }

  if (!verInfoBuffer.ReserveBuffer(cbVerInfo)) {
    return E_UNEXPECTED;
  }

  if (!GetFileVersionInfoW(filename, dwHandle, cbVerInfo,
                           verInfoBuffer.Data()) ||
      !VerQueryValueW(verInfoBuffer.Data(),
                      L"\\StringFileInfo\\040904B0\\ProductVersion",
                      reinterpret_cast<LPVOID*>(&lpBuffer), &puLen) ||
      !lpBuffer) {
    return HRESULT_FROM_WIN32(GetLastError());
  }

  outBuf->Assign(lpBuffer);
  return S_OK;
}

#pragma endregion

}  // namespace

#pragma region API Implementation

STDAPI CreateCoreWebView2EnvironmentWithOptions(
    PCWSTR browserExecutableFolder, PCWSTR userDataFolder,
    ICoreWebView2EnvironmentOptions* environmentOptions,
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*
        environmentCreatedHandler) {
  WString outStrings[3];
  WebView2EnvironmentParams params{
      .embeddedEdgeSubFolder = browserExecutableFolder,
      .userDataDir = userDataFolder,
      .environmentOptions = environmentOptions,
      .releaseChannelPreference = WebView2ReleaseChannelPreference::kStable,
  };
  if (!environmentCreatedHandler) return E_POINTER;
  UpdateWebViewEnvironmentParamsWithOverrideValues(&params, outStrings);
#ifndef __MINGW32__
  auto retryHandler = Microsoft::WRL::Make<EnvironmentCreatedRetryHandler>(
      params, environmentCreatedHandler, 1);
  return TryCreateWebViewEnvironment(params, retryHandler.Get());
#else
  auto retryHandler = new EnvironmentCreatedRetryHandler(
      params, environmentCreatedHandler, 1);
  return TryCreateWebViewEnvironment(params, environmentCreatedHandler);
#endif
}

STDAPI GetAvailableCoreWebView2BrowserVersionString(
    PCWSTR browserExecutableFolder, LPWSTR* versionInfo) {
  HRESULT hr;
  WebView2EnvironmentParams params{
      .embeddedEdgeSubFolder = browserExecutableFolder,
      .userDataDir = nullptr,
      .environmentOptions = nullptr,
      .releaseChannelPreference = WebView2ReleaseChannelPreference::kStable,
  };
  if (!versionInfo) {
    return E_POINTER;
  }
  WString outStrings[3];
  
  UpdateWebViewEnvironmentParamsWithOverrideValues(&params, outStrings);

  WString channelStr;
  WString versionStr;
  WString clientPath;
  if (params.embeddedEdgeSubFolder && *params.embeddedEdgeSubFolder) {
    hr = FindEmbeddedClientDll(params.embeddedEdgeSubFolder, &clientPath);
    if (FAILED(hr)) {
      *versionInfo = nullptr;
      return hr;
    }
    hr = FindEmbeddedBrowserVersion(clientPath.String(), &versionStr);
  } else {
    hr = FindInstalledClientDll(&clientPath, params.releaseChannelPreference,
                                &versionStr, &channelStr);
  }
  if (FAILED(hr)) {
    *versionInfo = nullptr;
    return hr;
  }
  if (!channelStr.Empty()) {
    versionStr.Append(L" ");
    versionStr.Append(channelStr.String(), channelStr.Length());
  }
  hr = S_OK;
  *versionInfo = MakeCoMemString(versionStr.String(), versionStr.Length());
  return hr;
}

STDAPI CompareBrowserVersions(PCWSTR version1, PCWSTR version2, int* result) {
  if (!result) {
    return E_POINTER;
  }
  if (!version1 || !version2) {
    return E_INVALIDARG;
  }
  UINT32 v1[4], v2[4];
  if (!ParseVersionNumbers(version1, v2) ||
      !ParseVersionNumbers(version2, v1)) {
    return E_INVALIDARG;
  }
  for (int i = 0; i < 4; ++i) {
    if (v2[i] > v1[i]) {
      *result = 1;
      return S_OK;
    }
    if (v2[i] < v1[i]) {
      *result = -1;
      return S_OK;
    }
  }
  *result = 0;
  return S_OK;
}

STDAPI CreateCoreWebView2Environment(
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*
        environmentCreatedHandler) {
  return CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
                                                  environmentCreatedHandler);
}

#pragma endregion

#endif