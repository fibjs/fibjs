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
	"pause",
	"removeBreakpoint",
	"restartFrame",
	"resume",
	"searchInContent",
	"setAsyncCallStackDepth",
	"setBreakpoint",
	"setInstrumentationBreakpoint",
	"setBreakpointByUrl",
	"setBreakpointsActive",
	"setPauseOnExceptions",
	"setScriptSource",
	"setSkipAllPauses",
	"setVariableValue",
	"stepInto",
	"stepOut",
	"stepOver",
	NULL
};

const char* s_Profiler_apis[] = {
	"disable",
	"enable",
	"getBestEffortCoverage",
	"setSamplingInterval",
	"start",
	"startPreciseCoverage",
	"stop",
	"stopPreciseCoverage",
	"takePreciseCoverage",
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
	"getProperties",
	"globalLexicalScopeNames",
	"queryObjects",
	"releaseObject",
	"releaseObjectGroup",
	"runIfWaitingForDebugger",
	"runScript",
	"setAsyncCallStackDepth",
	NULL
};

const char* s_Schema_apis[] = {
	"getDomains",
	NULL
};

const char* s_Browser_apis[] = {
	"close",
	"getVersion",
	NULL
};

const char* s_DOM_apis[] = {
	"describeNode",
	"disable",
	"enable",
	"focus",
	"getAttributes",
	"getBoxModel",
	"getDocument",
	"getFlattenedDocument",
	"getNodeForLocation",
	"getOuterHTML",
	"hideHighlight",
	"highlightNode",
	"highlightRect",
	"moveTo",
	"querySelector",
	"querySelectorAll",
	"removeAttribute",
	"removeNode",
	"requestChildNodes",
	"requestNode",
	"resolveNode",
	"setAttributeValue",
	"setAttributesAsText",
	"setFileInputFiles",
	"setNodeName",
	"setNodeValue",
	"setOuterHTML",
	NULL
};

const char* s_DOMDebugger_apis[] = {
	"getEventListeners",
	"removeDOMBreakpoint",
	"removeEventListenerBreakpoint",
	"removeXHRBreakpoint",
	"setDOMBreakpoint",
	"setEventListenerBreakpoint",
	"setXHRBreakpoint",
	NULL
};

const char* s_Emulation_apis[] = {
	"canEmulate",
	"clearDeviceMetricsOverride",
	"clearGeolocationOverride",
	"setDefaultBackgroundColorOverride",
	"setDeviceMetricsOverride",
	"setEmulatedMedia",
	"setGeolocationOverride",
	"setScriptExecutionDisabled",
	"setTouchEmulationEnabled",
	"setUserAgentOverride",
	NULL
};

const char* s_IO_apis[] = {
	"close",
	"read",
	"resolveBlob",
	NULL
};

const char* s_Input_apis[] = {
	"dispatchKeyEvent",
	"dispatchMouseEvent",
	"dispatchTouchEvent",
	"setIgnoreInputEvents",
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

const char* s_Network_apis[] = {
	"canClearBrowserCache",
	"canClearBrowserCookies",
	"canEmulateNetworkConditions",
	"clearBrowserCache",
	"clearBrowserCookies",
	"deleteCookies",
	"disable",
	"emulateNetworkConditions",
	"enable",
	"getAllCookies",
	"getCookies",
	"getResponseBody",
	"getRequestPostData",
	"setCacheDisabled",
	"setCookie",
	"setCookies",
	"setExtraHTTPHeaders",
	"setUserAgentOverride",
	NULL
};

const char* s_Page_apis[] = {
	"addScriptToEvaluateOnNewDocument",
	"bringToFront",
	"captureScreenshot",
	"clearGeolocationOverride",
	"createIsolatedWorld",
	"disable",
	"enable",
	"getAppManifest",
	"getFrameTree",
	"getLayoutMetrics",
	"getNavigationHistory",
	"resetNavigationHistory",
	"handleJavaScriptDialog",
	"navigate",
	"navigateToHistoryEntry",
	"printToPDF",
	"reload",
	"removeScriptToEvaluateOnNewDocument",
	"setDocumentContent",
	"setGeolocationOverride",
	"stopLoading",
	NULL
};

const char* s_Performance_apis[] = {
	"disable",
	"enable",
	"getMetrics",
	NULL
};

const char* s_Security_apis[] = {
	"disable",
	"enable",
	"handleCertificateError",
	"setOverrideCertificateErrors",
	NULL
};

const char* s_Target_apis[] = {
	"activateTarget",
	"attachToTarget",
	"closeTarget",
	"createTarget",
	"detachFromTarget",
	"getTargets",
	"sendMessageToTarget",
	"setDiscoverTargets",
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

const void* s_domains[][2] = {
	{ "Console", s_Console_apis },
	{ "Debugger", s_Debugger_apis },
	{ "Profiler", s_Profiler_apis },
	{ "Runtime", s_Runtime_apis },
	{ "Schema", s_Schema_apis },
	{ "Browser", s_Browser_apis },
	{ "DOM", s_DOM_apis },
	{ "DOMDebugger", s_DOMDebugger_apis },
	{ "Emulation", s_Emulation_apis },
	{ "IO", s_IO_apis },
	{ "Input", s_Input_apis },
	{ "Log", s_Log_apis },
	{ "Network", s_Network_apis },
	{ "Page", s_Page_apis },
	{ "Performance", s_Performance_apis },
	{ "Security", s_Security_apis },
	{ "Target", s_Target_apis },
	{ "Fetch", s_Fetch_apis },
	{ NULL, NULL }
};