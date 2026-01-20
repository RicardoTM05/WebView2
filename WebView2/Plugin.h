// Copyright (C) 2025 nstechbytes. All rights reserved.
#pragma once

#include <Windows.h>
#include <WebView2.h>
#include "SimpleIni.h"
#include <wil/com.h>
#include <wrl.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <iomanip>
#include <sstream>

using namespace Microsoft::WRL;

// Global TypeLib for COM objects
extern wil::com_ptr<ITypeLib> g_typeLib;

#define WM_APP_CTRL_CHANGED (WM_APP + 100) // Custom message for Ctrl key state change
#define WM_APP_REGION_RMB (WM_APP + 200) // Custom message for app-region RMB

struct SkinSubclassData;

extern bool g_extensions_checked;

// Measure structure containing WebView2 state
struct Measure
{
	void* rm;
	void* skin;
	HWND skinWindow;
	LPCWSTR measureName;
	LPCWSTR skinName;
	SkinSubclassData* skinData = nullptr;

	wchar_t osLocale[LOCALE_NAME_MAX_LENGTH] = { 0 };
	std::wstring userDataFolder;
	std::wstring configPath;
	std::wstring extensionsPath;
	std::wstring url;
	std::wstring currentUrl;
	std::wstring currentTitle;
	std::wstring hostName;
	std::wstring hostPath;
	std::wstring userAgent;
	
	int width = 800;
	int height = 600;
	int x = 0;
	int y = 0;
	int clickthrough = 1;
	double zoomFactor = 1.0;
	bool disabled = false;
	bool autoStart = true;
	bool visible = true;
	bool notifications = false;
	bool zoomControl = true;
	bool newWindow = false;
	bool assistiveFeatures = true;
	bool hostSecurity = true;
	bool hostOrigin = true;

	bool initialized = false;
	bool isCreationInProgress = false;
	bool isClickthroughActive = false;
	bool isStopping = false;
	bool isViewSource = false;
	bool isFirstLoad = true;
	bool isCtrlPressed = false;
	bool isWebViewFocused = false;

	std::wstring onWebViewLoadAction;
	std::wstring onWebViewFailAction;
	std::wstring onWebViewStopAction;
	std::wstring onStateChangeAction;
	std::wstring onUrlChangeAction;
	std::wstring onPageFirstLoadAction;
	std::wstring onPageLoadStartAction;
	std::wstring onPageLoadingAction;
	std::wstring onPageDOMLoadAction;
	std::wstring onPageLoadFinishAction;
	std::wstring onPageReloadAction;

	CSimpleIniW userSettingsFile;
	CSimpleIniW extensionsFile;
	bool userSettingsChanged = false;
	bool extensionsChanged = false;

	wil::com_ptr<ICoreWebView2Environment> webViewEnvironment;
	wil::com_ptr<ICoreWebView2Controller> webViewController;
	wil::com_ptr<ICoreWebView2ControllerOptions2>webViewControllerOptions2;
	wil::com_ptr<ICoreWebView2> webView;
	wil::com_ptr<ICoreWebView2_3> webView3;
	wil::com_ptr<ICoreWebView2_6> webView6;
	wil::com_ptr<ICoreWebView2Settings> webViewSettings;
	wil::com_ptr<ICoreWebView2Settings2> webViewSettings2;
	RECT webViewArea;

	EventRegistrationToken webMessageToken;

	std::wstring buffer;  // Buffer for section variable return values
	std::map<std::wstring, std::wstring> jsResults; // Cache for CallJS results
	bool isRuntimeInstalled = false;
	int state = -1; // Integer number to show the internal state of WebView and Navigation
	wil::unique_cotaskmem_string runtimeVersion = nullptr;

	Measure();
	~Measure();

	// Member callback functions for WebView2 creation
	HRESULT CreateEnvironmentHandler(HRESULT result, ICoreWebView2Environment* env);
	HRESULT CreateControllerHandler(HRESULT result, ICoreWebView2Controller* controller);
	void Measure::SetStateAndNotify(int newState);
	HRESULT Measure::FailWebView(HRESULT hr, const wchar_t* logMessage, bool resetCreationFlag = true);
};

// WebView2 functions
void CreateWebView2(Measure* measure);
void StopWebView2(Measure* measure);
void RestartWebView2(Measure* measure);
void UpdateChildWindowState(Measure* measure, bool enabled, bool shouldDefocus = true);

// Taken from: https://github.com/MicrosoftEdge/WebView2Samples/blob/main/SampleApps/WebView2APISample/CheckFailure.h
// Notify the user of a failure with a message box.
void ShowFailure(HRESULT hr, const std::wstring& message = L"Error");
// If something failed, show the error code and fail fast.
void CheckFailure(HRESULT hr, const std::wstring& message = L"Error");
// Notify the user that a feature is not available
void FeatureNotAvailable();

#define CHECK_FAILURE_STRINGIFY(arg) #arg
#define CHECK_FAILURE_FILE_LINE(file, line) ([](HRESULT hr){ CheckFailure(hr, L"Failure at " CHECK_FAILURE_STRINGIFY(file) L"(" CHECK_FAILURE_STRINGIFY(line) L")"); })
#define CHECK_FAILURE CHECK_FAILURE_FILE_LINE(__FILE__, __LINE__)
#define CHECK_FAILURE_BOOL(value) CHECK_FAILURE((value) ? S_OK : E_UNEXPECTED)

// Data structure for skin subclassing
struct SkinSubclassData
{
	HWND hwnd = nullptr;
	int refCount = 0;
	bool destroying = false;

	std::unordered_set<Measure*> measures = {};
};

static std::unordered_map<HWND, SkinSubclassData*> g_SkinSubclassMap;