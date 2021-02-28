const char* s_Console_apis[] = {
	"clearMessages",
	"disable",
	"enable",
	NULL
};

const char* s_Debugger_apis[] = {
	"continueToLocation",
	"disable",
	"enable",
	"evaluateOnCallFrame",
	"getPossibleBreakpoints",
	"getScriptSource",
	"getWasmBytecode",
	"getStackTrace",
	"pause",
	"pauseOnAsyncCall",
	"removeBreakpoint",
	"restartFrame",
	"resume",
	"searchInContent",
	"setAsyncCallStackDepth",
	"setBlackboxPatterns",
	"setBlackboxedRanges",
	"setBreakpoint",
	"setInstrumentationBreakpoint",
	"setBreakpointByUrl",
	"setBreakpointOnFunctionCall",
	"setBreakpointsActive",
	"setPauseOnExceptions",
	"setReturnValue",
	"setScriptSource",
	"setSkipAllPauses",
	"setVariableValue",
	"stepInto",
	"stepOut",
	"stepOver",
	NULL
};

const char* s_HeapProfiler_apis[] = {
	"addInspectedHeapObject",
	"collectGarbage",
	"disable",
	"enable",
	"getHeapObjectId",
	"getObjectByHeapObjectId",
	"getSamplingProfile",
	"startSampling",
	"startTrackingHeapObjects",
	"stopSampling",
	"stopTrackingHeapObjects",
	"takeHeapSnapshot",
	NULL
};

const char* s_Profiler_apis[] = {
	"disable",
	"enable",
	"getBestEffortCoverage",
	"setSamplingInterval",
	"start",
	"startPreciseCoverage",
	"startTypeProfile",
	"stop",
	"stopPreciseCoverage",
	"stopTypeProfile",
	"takePreciseCoverage",
	"takeTypeProfile",
	"enableCounters",
	"disableCounters",
	"getCounters",
	"enableRuntimeCallStats",
	"disableRuntimeCallStats",
	"getRuntimeCallStats",
	NULL
};

const char* s_Runtime_apis[] = {
	"awaitPromise",
	"callFunctionOn",
	"compileScript",
	"disable",
	"discardConsoleEntries",
	"enable",
	"evaluate",
	"getIsolateId",
	"getHeapUsage",
	"getProperties",
	"globalLexicalScopeNames",
	"queryObjects",
	"releaseObject",
	"releaseObjectGroup",
	"runIfWaitingForDebugger",
	"runScript",
	"setAsyncCallStackDepth",
	"setCustomObjectFormatterEnabled",
	"setMaxCallStackSizeToCapture",
	"terminateExecution",
	"addBinding",
	"removeBinding",
	NULL
};

const char* s_Schema_apis[] = {
	"getDomains",
	NULL
};

const char* s_Accessibility_apis[] = {
	"disable",
	"enable",
	"getPartialAXTree",
	"getFullAXTree",
	"getChildAXNodes",
	"queryAXTree",
	NULL
};

const char* s_Animation_apis[] = {
	"disable",
	"enable",
	"getCurrentTime",
	"getPlaybackRate",
	"releaseAnimations",
	"resolveAnimation",
	"seekAnimations",
	"setPaused",
	"setPlaybackRate",
	"setTiming",
	NULL
};

const char* s_ApplicationCache_apis[] = {
	"enable",
	"getApplicationCacheForFrame",
	"getFramesWithManifests",
	"getManifestForFrame",
	NULL
};

const char* s_Audits_apis[] = {
	"getEncodedResponse",
	"disable",
	"enable",
	"checkContrast",
	NULL
};

const char* s_BackgroundService_apis[] = {
	"startObserving",
	"stopObserving",
	"setRecording",
	"clearEvents",
	NULL
};

const char* s_Browser_apis[] = {
	"setPermission",
	"grantPermissions",
	"resetPermissions",
	"setDownloadBehavior",
	"close",
	"crash",
	"crashGpuProcess",
	"getVersion",
	"getBrowserCommandLine",
	"getHistograms",
	"getHistogram",
	"getWindowBounds",
	"getWindowForTarget",
	"setWindowBounds",
	"setDockTile",
	"executeBrowserCommand",
	NULL
};

const char* s_CSS_apis[] = {
	"addRule",
	"collectClassNames",
	"createStyleSheet",
	"disable",
	"enable",
	"forcePseudoState",
	"getBackgroundColors",
	"getComputedStyleForNode",
	"getInlineStylesForNode",
	"getMatchedStylesForNode",
	"getMediaQueries",
	"getPlatformFontsForNode",
	"getStyleSheetText",
	"trackComputedStyleUpdates",
	"takeComputedStyleUpdates",
	"setEffectivePropertyValueForNode",
	"setKeyframeKey",
	"setMediaText",
	"setRuleSelector",
	"setStyleSheetText",
	"setStyleTexts",
	"startRuleUsageTracking",
	"stopRuleUsageTracking",
	"takeCoverageDelta",
	"setLocalFontsEnabled",
	NULL
};

const char* s_CacheStorage_apis[] = {
	"deleteCache",
	"deleteEntry",
	"requestCacheNames",
	"requestCachedResponse",
	"requestEntries",
	NULL
};

const char* s_Cast_apis[] = {
	"enable",
	"disable",
	"setSinkToUse",
	"startTabMirroring",
	"stopCasting",
	NULL
};

const char* s_DOM_apis[] = {
	"collectClassNamesFromSubtree",
	"copyTo",
	"describeNode",
	"scrollIntoViewIfNeeded",
	"disable",
	"discardSearchResults",
	"enable",
	"focus",
	"getAttributes",
	"getBoxModel",
	"getContentQuads",
	"getDocument",
	"getFlattenedDocument",
	"getNodesForSubtreeByStyle",
	"getNodeForLocation",
	"getOuterHTML",
	"getRelayoutBoundary",
	"getSearchResults",
	"hideHighlight",
	"highlightNode",
	"highlightRect",
	"markUndoableState",
	"moveTo",
	"performSearch",
	"pushNodeByPathToFrontend",
	"pushNodesByBackendIdsToFrontend",
	"querySelector",
	"querySelectorAll",
	"redo",
	"removeAttribute",
	"removeNode",
	"requestChildNodes",
	"requestNode",
	"resolveNode",
	"setAttributeValue",
	"setAttributesAsText",
	"setFileInputFiles",
	"setNodeStackTracesEnabled",
	"getNodeStackTraces",
	"getFileInfo",
	"setInspectedNode",
	"setNodeName",
	"setNodeValue",
	"setOuterHTML",
	"undo",
	"getFrameOwner",
	NULL
};

const char* s_DOMDebugger_apis[] = {
	"getEventListeners",
	"removeDOMBreakpoint",
	"removeEventListenerBreakpoint",
	"removeInstrumentationBreakpoint",
	"removeXHRBreakpoint",
	"setBreakOnCSPViolation",
	"setDOMBreakpoint",
	"setEventListenerBreakpoint",
	"setInstrumentationBreakpoint",
	"setXHRBreakpoint",
	NULL
};

const char* s_DOMSnapshot_apis[] = {
	"disable",
	"enable",
	"getSnapshot",
	"captureSnapshot",
	NULL
};

const char* s_DOMStorage_apis[] = {
	"clear",
	"disable",
	"enable",
	"getDOMStorageItems",
	"removeDOMStorageItem",
	"setDOMStorageItem",
	NULL
};

const char* s_Database_apis[] = {
	"disable",
	"enable",
	"executeSQL",
	"getDatabaseTableNames",
	NULL
};

const char* s_DeviceOrientation_apis[] = {
	"clearDeviceOrientationOverride",
	"setDeviceOrientationOverride",
	NULL
};

const char* s_Emulation_apis[] = {
	"canEmulate",
	"clearDeviceMetricsOverride",
	"clearGeolocationOverride",
	"resetPageScaleFactor",
	"setFocusEmulationEnabled",
	"setCPUThrottlingRate",
	"setDefaultBackgroundColorOverride",
	"setDeviceMetricsOverride",
	"setScrollbarsHidden",
	"setDocumentCookieDisabled",
	"setEmitTouchEventsForMouse",
	"setEmulatedMedia",
	"setEmulatedVisionDeficiency",
	"setGeolocationOverride",
	"setIdleOverride",
	"clearIdleOverride",
	"setNavigatorOverrides",
	"setPageScaleFactor",
	"setScriptExecutionDisabled",
	"setTouchEmulationEnabled",
	"setVirtualTimePolicy",
	"setLocaleOverride",
	"setTimezoneOverride",
	"setVisibleSize",
	"setDisabledImageTypes",
	"setUserAgentOverride",
	NULL
};

const char* s_HeadlessExperimental_apis[] = {
	"beginFrame",
	"disable",
	"enable",
	NULL
};

const char* s_IO_apis[] = {
	"close",
	"read",
	"resolveBlob",
	NULL
};

const char* s_IndexedDB_apis[] = {
	"clearObjectStore",
	"deleteDatabase",
	"deleteObjectStoreEntries",
	"disable",
	"enable",
	"requestData",
	"getMetadata",
	"requestDatabase",
	"requestDatabaseNames",
	NULL
};

const char* s_Input_apis[] = {
	"dispatchKeyEvent",
	"insertText",
	"dispatchMouseEvent",
	"dispatchTouchEvent",
	"emulateTouchFromMouseEvent",
	"setIgnoreInputEvents",
	"synthesizePinchGesture",
	"synthesizeScrollGesture",
	"synthesizeTapGesture",
	NULL
};

const char* s_Inspector_apis[] = {
	"disable",
	"enable",
	NULL
};

const char* s_LayerTree_apis[] = {
	"compositingReasons",
	"disable",
	"enable",
	"loadSnapshot",
	"makeSnapshot",
	"profileSnapshot",
	"releaseSnapshot",
	"replaySnapshot",
	"snapshotCommandLog",
	NULL
};

const char* s_Log_apis[] = {
	"clear",
	"disable",
	"enable",
	"startViolationsReport",
	"stopViolationsReport",
	NULL
};

const char* s_Memory_apis[] = {
	"getDOMCounters",
	"prepareForLeakDetection",
	"forciblyPurgeJavaScriptMemory",
	"setPressureNotificationsSuppressed",
	"simulatePressureNotification",
	"startSampling",
	"stopSampling",
	"getAllTimeSamplingProfile",
	"getBrowserSamplingProfile",
	"getSamplingProfile",
	NULL
};

const char* s_Network_apis[] = {
	"canClearBrowserCache",
	"canClearBrowserCookies",
	"canEmulateNetworkConditions",
	"clearBrowserCache",
	"clearBrowserCookies",
	"continueInterceptedRequest",
	"deleteCookies",
	"disable",
	"emulateNetworkConditions",
	"enable",
	"getAllCookies",
	"getCertificate",
	"getCookies",
	"getResponseBody",
	"getRequestPostData",
	"getResponseBodyForInterception",
	"takeResponseBodyForInterceptionAsStream",
	"replayXHR",
	"searchInResponseBody",
	"setBlockedURLs",
	"setBypassServiceWorker",
	"setCacheDisabled",
	"setCookie",
	"setCookies",
	"setDataSizeLimitsForTest",
	"setExtraHTTPHeaders",
	"setAttachDebugStack",
	"setRequestInterception",
	"setUserAgentOverride",
	"getSecurityIsolationStatus",
	"loadNetworkResource",
	NULL
};

const char* s_Overlay_apis[] = {
	"disable",
	"enable",
	"getHighlightObjectForTest",
	"getGridHighlightObjectsForTest",
	"getSourceOrderHighlightObjectForTest",
	"hideHighlight",
	"highlightFrame",
	"highlightNode",
	"highlightQuad",
	"highlightRect",
	"highlightSourceOrder",
	"setInspectMode",
	"setShowAdHighlights",
	"setPausedInDebuggerMessage",
	"setShowDebugBorders",
	"setShowFPSCounter",
	"setShowGridOverlays",
	"setShowFlexOverlays",
	"setShowPaintRects",
	"setShowLayoutShiftRegions",
	"setShowScrollBottleneckRects",
	"setShowHitTestBorders",
	"setShowWebVitals",
	"setShowViewportSizeOnResize",
	"setShowHinge",
	NULL
};

const char* s_Page_apis[] = {
	"addScriptToEvaluateOnLoad",
	"addScriptToEvaluateOnNewDocument",
	"bringToFront",
	"captureScreenshot",
	"captureSnapshot",
	"clearDeviceMetricsOverride",
	"clearDeviceOrientationOverride",
	"clearGeolocationOverride",
	"createIsolatedWorld",
	"deleteCookie",
	"disable",
	"enable",
	"getAppManifest",
	"getInstallabilityErrors",
	"getManifestIcons",
	"getCookies",
	"getFrameTree",
	"getLayoutMetrics",
	"getNavigationHistory",
	"resetNavigationHistory",
	"getResourceContent",
	"getResourceTree",
	"handleJavaScriptDialog",
	"navigate",
	"navigateToHistoryEntry",
	"printToPDF",
	"reload",
	"removeScriptToEvaluateOnLoad",
	"removeScriptToEvaluateOnNewDocument",
	"screencastFrameAck",
	"searchInResource",
	"setAdBlockingEnabled",
	"setBypassCSP",
	"getPermissionsPolicyState",
	"setDeviceMetricsOverride",
	"setDeviceOrientationOverride",
	"setFontFamilies",
	"setFontSizes",
	"setDocumentContent",
	"setDownloadBehavior",
	"setGeolocationOverride",
	"setLifecycleEventsEnabled",
	"setTouchEmulationEnabled",
	"startScreencast",
	"stopLoading",
	"crash",
	"close",
	"setWebLifecycleState",
	"stopScreencast",
	"setProduceCompilationCache",
	"addCompilationCache",
	"clearCompilationCache",
	"generateTestReport",
	"waitForDebugger",
	"setInterceptFileChooserDialog",
	NULL
};

const char* s_Performance_apis[] = {
	"disable",
	"enable",
	"setTimeDomain",
	"getMetrics",
	NULL
};

const char* s_PerformanceTimeline_apis[] = {
	"enable",
	NULL
};

const char* s_Security_apis[] = {
	"disable",
	"enable",
	"setIgnoreCertificateErrors",
	"handleCertificateError",
	"setOverrideCertificateErrors",
	NULL
};

const char* s_ServiceWorker_apis[] = {
	"deliverPushMessage",
	"disable",
	"dispatchSyncEvent",
	"dispatchPeriodicSyncEvent",
	"enable",
	"inspectWorker",
	"setForceUpdateOnPageLoad",
	"skipWaiting",
	"startWorker",
	"stopAllWorkers",
	"stopWorker",
	"unregister",
	"updateRegistration",
	NULL
};

const char* s_Storage_apis[] = {
	"clearDataForOrigin",
	"getCookies",
	"setCookies",
	"clearCookies",
	"getUsageAndQuota",
	"overrideQuotaForOrigin",
	"trackCacheStorageForOrigin",
	"trackIndexedDBForOrigin",
	"untrackCacheStorageForOrigin",
	"untrackIndexedDBForOrigin",
	"getTrustTokens",
	NULL
};

const char* s_SystemInfo_apis[] = {
	"getInfo",
	"getProcessInfo",
	NULL
};

const char* s_Target_apis[] = {
	"activateTarget",
	"attachToTarget",
	"attachToBrowserTarget",
	"closeTarget",
	"exposeDevToolsProtocol",
	"createBrowserContext",
	"getBrowserContexts",
	"createTarget",
	"detachFromTarget",
	"disposeBrowserContext",
	"getTargetInfo",
	"getTargets",
	"sendMessageToTarget",
	"setAutoAttach",
	"setDiscoverTargets",
	"setRemoteLocations",
	NULL
};

const char* s_Tethering_apis[] = {
	"bind",
	"unbind",
	NULL
};

const char* s_Tracing_apis[] = {
	"end",
	"getCategories",
	"recordClockSyncMarker",
	"requestMemoryDump",
	"start",
	NULL
};

const char* s_Fetch_apis[] = {
	"disable",
	"enable",
	"failRequest",
	"fulfillRequest",
	"continueRequest",
	"continueWithAuth",
	"getResponseBody",
	"takeResponseBodyAsStream",
	NULL
};

const char* s_WebAudio_apis[] = {
	"enable",
	"disable",
	"getRealtimeData",
	NULL
};

const char* s_WebAuthn_apis[] = {
	"enable",
	"disable",
	"addVirtualAuthenticator",
	"removeVirtualAuthenticator",
	"addCredential",
	"getCredential",
	"getCredentials",
	"removeCredential",
	"clearCredentials",
	"setUserVerified",
	"setAutomaticPresenceSimulation",
	NULL
};

const char* s_Media_apis[] = {
	"enable",
	"disable",
	NULL
};

const void* s_domains[][2] = {
	{ "Console", s_Console_apis },
	{ "Debugger", s_Debugger_apis },
	{ "HeapProfiler", s_HeapProfiler_apis },
	{ "Profiler", s_Profiler_apis },
	{ "Runtime", s_Runtime_apis },
	{ "Schema", s_Schema_apis },
	{ "Accessibility", s_Accessibility_apis },
	{ "Animation", s_Animation_apis },
	{ "ApplicationCache", s_ApplicationCache_apis },
	{ "Audits", s_Audits_apis },
	{ "BackgroundService", s_BackgroundService_apis },
	{ "Browser", s_Browser_apis },
	{ "CSS", s_CSS_apis },
	{ "CacheStorage", s_CacheStorage_apis },
	{ "Cast", s_Cast_apis },
	{ "DOM", s_DOM_apis },
	{ "DOMDebugger", s_DOMDebugger_apis },
	{ "DOMSnapshot", s_DOMSnapshot_apis },
	{ "DOMStorage", s_DOMStorage_apis },
	{ "Database", s_Database_apis },
	{ "DeviceOrientation", s_DeviceOrientation_apis },
	{ "Emulation", s_Emulation_apis },
	{ "HeadlessExperimental", s_HeadlessExperimental_apis },
	{ "IO", s_IO_apis },
	{ "IndexedDB", s_IndexedDB_apis },
	{ "Input", s_Input_apis },
	{ "Inspector", s_Inspector_apis },
	{ "LayerTree", s_LayerTree_apis },
	{ "Log", s_Log_apis },
	{ "Memory", s_Memory_apis },
	{ "Network", s_Network_apis },
	{ "Overlay", s_Overlay_apis },
	{ "Page", s_Page_apis },
	{ "Performance", s_Performance_apis },
	{ "PerformanceTimeline", s_PerformanceTimeline_apis },
	{ "Security", s_Security_apis },
	{ "ServiceWorker", s_ServiceWorker_apis },
	{ "Storage", s_Storage_apis },
	{ "SystemInfo", s_SystemInfo_apis },
	{ "Target", s_Target_apis },
	{ "Tethering", s_Tethering_apis },
	{ "Tracing", s_Tracing_apis },
	{ "Fetch", s_Fetch_apis },
	{ "WebAudio", s_WebAudio_apis },
	{ "WebAuthn", s_WebAuthn_apis },
	{ "Media", s_Media_apis },
	{ NULL, NULL }
};