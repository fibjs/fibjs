#pragma once
#include <Windows.h>

struct ICoreWebView2EnvironmentOptions;
struct ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler;

STDAPI CreateCoreWebView2EnvironmentWithOptions(
    PCWSTR browserExecutableFolder, PCWSTR userDataFolder,
    ICoreWebView2EnvironmentOptions *environmentOptions,
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler
        *environmentCreatedHandler);

STDAPI CreateCoreWebView2Environment(
    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler
        *environmentCreatedHandler);

STDAPI GetAvailableCoreWebView2BrowserVersionString(
    PCWSTR browserExecutableFolder, LPWSTR *versionInfo);

STDAPI CompareBrowserVersions(PCWSTR version1, PCWSTR version2, int *result);
