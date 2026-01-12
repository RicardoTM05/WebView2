// Copyright (C) 2025 nstechbytes. All rights reserved.
#include "Plugin.h"
#include "../API/RainmeterAPI.h"
#include <WebView2EnvironmentOptions.h>
#include <CommCtrl.h>
#include <mutex>
#include <algorithm>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

// Global COM initialization tracking
static bool g_comInitialized = false;

// Global TypeLib for COM objects
wil::com_ptr<ITypeLib> g_typeLib;

// Global keyboard hook and state
static HMODULE g_hModule = nullptr;
HHOOK g_kbHook = nullptr;
std::atomic<bool> g_ctrlDown{ false };
std::atomic<int>  g_refCount(0);
static std::atomic<bool> g_hookAlive{ false };

static std::mutex g_skinMapMutex;

std::wstring ToLower(std::wstring s)
{
	if (!s.empty())
	{
		CharLowerBuffW(s.data(), static_cast<DWORD>(s.size()));
	}
	return s;
}

// DllMain to load TypeLib from embedded resources
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// Extract TypeLib from embedded resource and load it
		wchar_t tempPath[MAX_PATH];
		GetTempPath(MAX_PATH, tempPath);
		wcscat_s(tempPath, L"WebView2.tlb");

		// Read embedded resource: ID = 1, Type = TYPELIB
		HRSRC hResInfo = FindResource(hinstDLL, MAKEINTRESOURCE(1), L"TYPELIB");
		if (hResInfo)
		{
			HGLOBAL hRes = LoadResource(hinstDLL, hResInfo);
			if (hRes)  // Check if LoadResource succeeded
			{
				LPVOID memRes = LockResource(hRes);
				DWORD sizeRes = SizeofResource(hinstDLL, hResInfo);

				HANDLE hFile = CreateFile(tempPath, GENERIC_WRITE, 0, NULL,
					CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					DWORD written;
					WriteFile(hFile, memRes, sizeRes, &written, NULL);
					CloseHandle(hFile);

					// Load the TypeLib
					LoadTypeLib(tempPath, &g_typeLib);
				}
			}
		}
	}
	return TRUE;
}

// Measure constructor
Measure::Measure() : rm(nullptr), skin(nullptr), skinWindow(nullptr),
measureName(nullptr),
width(800), height(600), x(0), y(0),
webMessageToken{}
{
	// Initialize COM for this thread if not already done
	if (!g_comInitialized)
	{
		HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
		if (SUCCEEDED(hr) || hr == RPC_E_CHANGED_MODE)
		{
			g_comInitialized = true;
		}
	}
}

// Measure destructor
Measure::~Measure()
{
}

// Helper Functions
void ShowFailure(HRESULT hr, const std::wstring& message)
{
	LPWSTR systemMessage = nullptr;

	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&systemMessage),
		0,
		nullptr);

	std::wstringstream formattedMessage;
	formattedMessage << message
		<< L": 0x"
		<< std::hex << std::setw(8) << std::setfill(L'0')
		<< hr;

	if (systemMessage)
	{
		formattedMessage << L" (" << systemMessage << L")";
		LocalFree(systemMessage);
	}

	MessageBoxW(nullptr, formattedMessage.str().c_str(), nullptr, MB_OK);
}

void CheckFailure(HRESULT hr, const std::wstring& message)
{
	if (FAILED(hr))
	{
		ShowFailure(hr, message);
		FAIL_FAST();
	}
}

void FeatureNotAvailable()
{
	MessageBox(nullptr,
		L"This feature is not available in the WebView2 runtime version currently being used.",
		L"Feature Not Available", MB_OK);
}

void UpdateWindowBounds(Measure* measure)
{
	if (!measure || !measure->webViewController)
		return;

	RECT bounds{
		measure->x,
		measure->y,
		measure->x + measure->width,
		measure->y + measure->height
	};

	measure->webViewController->put_Bounds(bounds);
	measure->webViewController->NotifyParentWindowPositionChanged();
}

void UpdateChildWindowState(Measure* measure, bool enabled, bool shouldDefocus)
{
	if (!measure->skinWindow || !IsWindow(measure->skinWindow))
		return;

	for (HWND child = GetWindow(measure->skinWindow, GW_CHILD);
		child != nullptr;
		child = GetWindow(child, GW_HWNDNEXT))
	{
		if (!IsWindow(child))
			continue;

		EnableWindow(child, enabled);

		if (!enabled && shouldDefocus)
		{
			HWND focused = GetFocus();
			if (focused && IsWindow(focused) &&
				(focused == child || IsChild(child, focused)))
			{
				SetFocus(nullptr);
			}
		}
	}
}

std::wstring GetHostName(const std::wstring& input, bool origin)
{
	std::wstring result;
	result.reserve(input.size());

	for (wchar_t ch : input)
	{
		if (origin && (ch == L'/' || ch == L'\\'))
		{
			break; // stop processing at first '/'
		}

		if ((ch >= L'a' && ch <= L'z') ||
			(ch >= L'A' && ch <= L'Z') ||
			(ch >= L'0' && ch <= L'9'))
		{
			result.push_back(ch);
		}
		else
		{
			result.push_back(L'-');
		}
	}

	// convert to lowercase
	result = ToLower(result);

	return result;
}

// Skin subclass procedure
LRESULT CALLBACK SkinSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	SkinSubclassData* skinData =
		reinterpret_cast<SkinSubclassData*>(dwRefData);

	if (!skinData || skinData->destroying)
		return DefSubclassProc(hWnd, uMsg, wParam, lParam);
	switch (uMsg)
	{
	case WM_APP_CTRL_CHANGED:
	{
		bool isCtrlPressed = (bool)wParam;

		for (Measure* measure : skinData->measures)
		{
			if (!measure || !measure->initialized || !measure->visible)
				continue;

			measure->isCtrlPressed = isCtrlPressed;
			
			if (measure->skinControl <= 1)
				continue;

			if (measure->skinControl == 2) // Hold ctrl to enable SkinControl
			{
				if (!measure->isSkinControlActive && isCtrlPressed)
				{
					measure->isSkinControlActive = true;
					UpdateChildWindowState(measure, false, false);
					//RmLog(measure->rm, LOG_DEBUG, L"Plugin: Ctrl was pressed");
				}
				else if (measure->isSkinControlActive && !isCtrlPressed)
				{
					measure->isSkinControlActive = false;
					UpdateChildWindowState(measure, true, false);
					//RmLog(measure->rm, LOG_DEBUG, L"Plugin: Ctrl was released");
				}
			}
			else if (measure->skinControl >= 3)
			{
				if (!measure->isSkinControlActive && isCtrlPressed)
				{
					measure->isSkinControlActive = true;
					UpdateChildWindowState(measure, true, false);
					//RmLog(measure->rm, LOG_DEBUG, L"Plugin: Ctrl was pressed");
				}
				else if (measure->isSkinControlActive && !isCtrlPressed)
				{
					measure->isSkinControlActive = false;
					UpdateChildWindowState(measure, false, false);
					//RmLog(measure->rm, LOG_DEBUG, L"Plugin: Ctrl was released");
				}
			}
		}
		return 0;
	}
	case WM_MOVE:
	case WM_MOVING: // Update bounds during drag so window.screenX/Y work correctly on JS.
		for (Measure* measure : skinData->measures)
		{
			if (measure && measure->initialized)
			{
				UpdateWindowBounds(measure);
			}
		}
		break;
	case WM_NCDESTROY:
		RemoveWindowSubclass(hWnd, SkinSubclassProc, uIdSubclass);
		break;
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Keyboard hook procedure
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (!g_hookAlive.load(std::memory_order_acquire))
	{
		return CallNextHookEx(nullptr, nCode, wParam, lParam);
	}

	if (nCode == HC_ACTION)
	{
		auto* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

		if (kb->vkCode == VK_LCONTROL || kb->vkCode == VK_RCONTROL)
		{
			bool down = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
			bool old = g_ctrlDown.exchange(down, std::memory_order_relaxed);

			if (down != old)
			{
				// Copy HWNDs under lock, then release lock and post messages.
				std::vector<HWND> targets;
				{
					std::lock_guard<std::mutex> lg(g_skinMapMutex);
					targets.reserve(g_SkinSubclassMap.size());
					for (const auto& p : g_SkinSubclassMap)
						targets.push_back(p.first);
				}

				// PostMessage without holding the map lock. Check IsWindow to avoid posting to invalid HWND.
				for (HWND hwnd : targets)
				{
					if (IsWindow(hwnd))
					{
						PostMessage(hwnd, WM_APP_CTRL_CHANGED, static_cast<WPARAM>(down), 0);
					}
				}
			}
		}
	}

	return CallNextHookEx(g_kbHook, nCode, wParam, lParam);
}

void InstallKeyboardHook()
{
	if (!g_hModule)
	{
		GetModuleHandleEx(
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
			reinterpret_cast<LPCWSTR>(&LowLevelKeyboardProc),
			&g_hModule
		);
	}
	if (!g_kbHook && g_hModule)
	{
		g_hookAlive.store(true, std::memory_order_release);
		g_kbHook = SetWindowsHookEx(
			WH_KEYBOARD_LL,
			LowLevelKeyboardProc,
			g_hModule,
			0
		);
	}
}

void RemoveKeyboardHook()
{
	if (g_kbHook)
	{
		g_hookAlive.store(false, std::memory_order_release);
		UnhookWindowsHookEx(g_kbHook);
		g_kbHook = nullptr;
	}
	if (g_hModule)
	{
		FreeLibrary(g_hModule);
		g_hModule = nullptr;
	}
}

// Rainmeter Plugin Exports
PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	Measure* measure = new Measure;
	*data = measure;

	if (g_refCount++ == 0)
	{
		//RmLog(measure->rm, LOG_DEBUG, (L"WebView2: Starting keyboard hook. Current global refCount: " + std::to_wstring(g_refCount)).c_str());
		InstallKeyboardHook();
	}
	else {
		//RmLog(measure->rm, LOG_DEBUG, (L"WebView2: Current global refCount: " + std::to_wstring(g_refCount)).c_str());
	}

	measure->rm = rm;
	measure->skin = RmGetSkin(rm);
	measure->skinWindow = RmGetSkinWindow(rm);
	measure->measureName = RmGetMeasureName(rm);
	measure->skinName = RmGetSkinName(rm);
	measure->hostSecurity = RmReadInt(rm, L"HostSecurity", 1) >= 1;
	measure->hostOrigin = RmReadInt(rm, L"HostOrigin", 1) >= 1;

	measure->hostName = GetHostName(measure->skinName, measure->hostOrigin);

	GetUserDefaultLocaleName(measure->osLocale, LOCALE_NAME_MAX_LENGTH);

	if (!measure->skinWindow)
		return;

	HRESULT hr = GetAvailableCoreWebView2BrowserVersionString(nullptr, &measure->runtimeVersion);
	if (SUCCEEDED(hr) && measure->runtimeVersion != nullptr)
	{
		measure->isRuntimeInstalled = true;
	}
	else
	{
		int result = MessageBox(
			measure->skinWindow,
			L"WebView2 Runtime is not installed.\n\n"
			L"Would you like to be redirected to the WebView2 Runtime download page?",
			L"WebView2 Runtime Error",
			MB_ICONERROR | MB_YESNO
		);

		if (result == IDYES)
		{
			ShellExecuteW(
				nullptr,
				L"open",
				L"https://developer.microsoft.com/microsoft-edge/webview2/",
				nullptr,
				nullptr,
				SW_SHOWNORMAL
			);
		}
		measure->FailWebView(hr, L"WebView2: WebView2 Runtime is not installed.");
		measure->isRuntimeInstalled = false;
		return;
	}

	measure->autoStart = RmReadInt(rm, L"AutoStart", 1) >= 1;

	// Create user data folder in TEMP directory to avoid permission issues
	wchar_t tempPath[MAX_PATH];
	GetTempPathW(MAX_PATH, tempPath);
	measure->userDataFolder = std::wstring(tempPath) + L"RainmeterWebView2";
	measure->configPath = measure->userDataFolder + L"\\WebView2Settings.ini";

	// Create the directory if it doesn't exist
	CreateDirectoryW(measure->userDataFolder.c_str(), nullptr);

	SkinSubclassData* skinData = nullptr;
	bool createdNew = false;

	{
		std::lock_guard<std::mutex> lg(g_skinMapMutex);
		auto it = g_SkinSubclassMap.find(measure->skinWindow);
		if (it == g_SkinSubclassMap.end())
		{
			skinData = new SkinSubclassData;
			skinData->hwnd = measure->skinWindow;
			skinData->refCount = 1;
			g_SkinSubclassMap[measure->skinWindow] = skinData;
			createdNew = true;
		}
		else
		{
			skinData = it->second;
			skinData->refCount++;
		}
	}

	// Set subclass if this is the first measure for the skin
	if (createdNew)
	{
		SetWindowSubclass(
			measure->skinWindow,
			SkinSubclassProc,
			1,
			reinterpret_cast<DWORD_PTR>(skinData)
		);
	}

	// Register this measure with the skin
	skinData->measures.insert(measure);
	measure->skinData = skinData;
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* /*maxValue*/)
{
	Measure* measure = static_cast<Measure*>(data);

	// Disabled check
	measure->disabled = RmReadInt(rm, L"Disabled", 0) >= 1;
	if (!measure->isRuntimeInstalled || measure->disabled)
	{
		return;
	}

	// Read basic configuration
	const int    newWidth = RmReadInt(rm, L"W", 800);
	const int    newHeight = RmReadInt(rm, L"H", 600);
	const int    newX = RmReadInt(rm, L"X", 0);
	const int    newY = RmReadInt(rm, L"Y", 0);
	const int	 newSkinControl = RmReadInt(rm, L"SkinControl", 2);
	const int	 newContextMenu = RmReadInt(rm, L"ContextMenu", 1);
	const double newZoomFactor = RmReadFormula(rm, L"ZoomFactor", 1.0);
	const bool	 newVisible = RmReadInt(rm, L"Hidden", 0) <= 0;
	const bool	 newNotifications = RmReadInt(rm, L"Notifications", 0) >= 1;
	const bool	 newNewWindow = RmReadInt(rm, L"NewWindow", 0) >= 1;
	const bool	 newZoomControl = RmReadInt(rm, L"ZoomControl", 1) >= 1;
	const bool	 newAssistiveFeatures = RmReadInt(rm, L"AssistiveFeatures", 1) >= 1;
	const bool	 newHostSecurity = RmReadInt(rm, L"HostSecurity", 1) >= 1;
	const bool	 newHostOrigin = RmReadInt(rm, L"HostOrigin", 1) >= 1;
	const std::wstring newHostPath = RmReadString(rm, L"HostPath", L"");
	const std::wstring newUserAgent = RmReadString(rm, L"UserAgent", L"");

	// URL handling
	std::wstring newUrl;
	LPCWSTR urlOption = RmReadString(rm, L"Url", L"");

	if (urlOption && *urlOption)
	{
		std::wstring urlStr = urlOption;

		// Protocol present - use as-is
		if (urlStr.find(L"://") != std::wstring::npos)
		{
			newUrl = urlStr;
		}
		else
		{
			// Relative path - convert to absolute
			if (urlStr[0] != L'/' && (urlStr.length() < 2 || urlStr[1] != L':'))
			{
				if (newHostPath.empty()) // Map to file:///
				{
					if (LPCWSTR absolutePath = RmPathToAbsolute(rm, urlStr.c_str()))
					{
						urlStr = absolutePath;
					}

					// Normalize slashes
					for (wchar_t& ch : urlStr)
					{
						if (ch == L'\\') ch = L'/';
					}

					// Ensure file:///
					if (urlStr.find(L"file:///") != 0)
					{
						urlStr = L"file:///" + urlStr;
					}
				}
				else // Map to virtual host
				{
					const std::wstring protocol = newHostSecurity ? L"https://" : L"http://";
					if (urlStr[0] != L'\\')
					{
						urlStr = protocol + std::wstring(measure->hostName.c_str()) + L"/" + urlStr;
					}
					else
					{
						urlStr = protocol + std::wstring(measure->hostName.c_str()) + urlStr;
					}
				}
			}

			newUrl = urlStr;
		}
	}

	// Action strings
	auto ReadAction = [&](LPCWSTR key) -> std::wstring
		{
			LPCWSTR value = RmReadString(rm, key, L"", FALSE);
			return (value && *value) ? value : L"";
		};

	const std::wstring newOnWebViewLoadAction = ReadAction(L"OnWebViewLoadAction");
	const std::wstring newOnWebViewFailAction = ReadAction(L"OnWebViewFailAction");
	const std::wstring newOnWebViewStopAction = ReadAction(L"OnWebViewStopAction");
	const std::wstring newOnStateChangeAction = ReadAction(L"OnStateChangeAction");
	const std::wstring newOnUrlChangeAction = ReadAction(L"OnUrlChangeAction");
	const std::wstring newOnPageLoadStartAction = ReadAction(L"OnPageLoadStartAction");
	const std::wstring newOnPageLoadingAction = ReadAction(L"OnPageLoadingAction");
	const std::wstring newOnPageDOMLoadAction = ReadAction(L"OnPageDOMLoadAction");
	const std::wstring newOnPageLoadFinishAction = ReadAction(L"OnPageLoadFinishAction");
	const std::wstring newOnPageFirstLoadAction = ReadAction(L"OnPageFirstLoadAction");
	const std::wstring newOnPageReloadAction = ReadAction(L"OnPageReloadAction");

	// Change detection
	const bool dimensionsChanged = (newWidth != measure->width || newHeight != measure->height || newX != measure->x || newY != measure->y);
	const bool visibilityChanged = (newVisible != measure->visible);
	const bool zoomFactorChanged = (newZoomFactor != measure->zoomFactor);
	const bool zoomControlChanged = (newZoomControl != measure->zoomControl);
	const bool userAgentChanged = (newUserAgent != measure->userAgent);
	const bool skinControlChanged = (newSkinControl != measure->skinControl);
	const bool hostChanged = (newHostPath != measure->hostPath || newHostSecurity != measure->hostSecurity || newHostOrigin != measure->hostOrigin);

	// Options
	measure->url = newUrl;
	measure->width = newWidth;
	measure->height = newHeight;
	measure->x = newX;
	measure->y = newY;
	measure->zoomFactor = newZoomFactor;
	measure->zoomControl = newZoomControl;
	measure->visible = newVisible;
	measure->skinControl = newSkinControl;
	measure->notifications = newNotifications;
	measure->newWindow = newNewWindow;
	measure->contextMenu = newContextMenu;
	measure->hostPath = newHostPath;
	measure->userAgent = newUserAgent;
	measure->assistiveFeatures = newAssistiveFeatures;

	// Actions
	measure->onWebViewLoadAction = newOnWebViewLoadAction;
	measure->onWebViewFailAction = newOnWebViewFailAction;
	measure->onWebViewStopAction = newOnWebViewStopAction;
	measure->onStateChangeAction = newOnStateChangeAction;
	measure->onUrlChangeAction = newOnUrlChangeAction;

	measure->onPageLoadStartAction = newOnPageLoadStartAction;
	measure->onPageLoadingAction = newOnPageLoadingAction;
	measure->onPageDOMLoadAction = newOnPageDOMLoadAction;
	measure->onPageLoadFinishAction = newOnPageLoadFinishAction;
	measure->onPageFirstLoadAction = newOnPageFirstLoadAction;
	measure->onPageReloadAction = newOnPageReloadAction;

	// Initialization
	if (!measure->initialized && measure->autoStart && !measure->disabled)
	{
		if (measure->isCreationInProgress)
		{
			return;
		}

		CreateWebView2(measure);
		measure->autoStart = false;
		return;
	}

	// Dynamic updates
	if (skinControlChanged)
	{
		measure->isSkinControlActive = false;
		if (measure->skinControl <= 0 || measure->skinControl == 2)
		{
			UpdateChildWindowState(measure, true);
		}
		else if (measure->skinControl == 1 || measure->skinControl >= 3)
		{
			UpdateChildWindowState(measure, false);
		}
	}

	// Controller Options
	if (dimensionsChanged && measure->webViewController)
	{
		UpdateWindowBounds(measure);
	}

	if (zoomFactorChanged && measure->webViewController)
	{
		measure->webViewController->put_ZoomFactor(measure->zoomFactor);
	}

	if (hostChanged)
	{
		if (measure->webView3)
		{
			if (!measure->hostPath.empty())
			{
				if (newHostSecurity != measure->hostSecurity || newHostOrigin != measure->hostOrigin)
				{
					measure->hostName = GetHostName(measure->skinName, measure->hostOrigin);
				}
				measure->webView3->SetVirtualHostNameToFolderMapping(measure->hostName.c_str(), measure->hostPath.c_str(), COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_ALLOW);
			}
			else
			{
				measure->webView3->ClearVirtualHostNameToFolderMapping(measure->hostName.c_str());
			}
		}
	}

	if (visibilityChanged && measure->webViewController)
	{
		measure->webViewController->put_IsVisible(measure->visible);

		if (!measure->visible) {

			SetFocus(nullptr);

			if (measure->webView3) {
				// Suspend to save resources
				measure->webView3->TrySuspend(Callback<ICoreWebView2TrySuspendCompletedHandler>(
					[](HRESULT errorCode, BOOL isSuccessful) -> HRESULT {
						return S_OK;
					})
					.Get());
			}
		}
		else if (measure->webView3)
		{
			// Resume when made visible
			measure->webView3->Resume();
		}
	}

	// Core Options
	if (zoomControlChanged && measure->webViewSettings)
	{
		measure->webViewSettings->put_IsZoomControlEnabled(measure->zoomControl);
	}

	if (userAgentChanged && measure->webViewSettings2)
	{
		measure->webViewSettings2->put_UserAgent(measure->userAgent.c_str());
	}

}

PLUGIN_EXPORT double Update(void* data)
{
	Measure* measure = (Measure*)data;

	// Call JavaScript OnUpdate callback if WebView is initialized
	if (measure->initialized && measure->webView)
	{
		measure->webView->ExecuteScript(
			L"(function() { if (typeof window.OnUpdate === 'function') { var result = window.OnUpdate(); return result !== undefined ? String(result) : ''; } return ''; })();",
			Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
				[measure](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT
				{
					return S_OK;
				}
			).Get()
		);
	}

	return measure->state;
}

PLUGIN_EXPORT LPCWSTR GetString(void* data)
{
	Measure* measure = (Measure*)data;

	// Return the current url if available, otherwise return ""
	if (!measure->currentUrl.empty())
	{
		return measure->currentUrl.c_str();
	}

	return L"";
}

PLUGIN_EXPORT void ExecuteBang(void* data, LPCWSTR args)
{
	Measure* measure = (Measure*)data;
	if (!measure)
		return;

	if (measure->disabled)
	{
		RmLog(measure->rm, LOG_ERROR, L"WebView2: The measure is disabled");
		return;
	}

	if (!measure->isRuntimeInstalled)
	{
		RmLog(measure->rm, LOG_ERROR, L"WebView2: WebView2 Runtime is not installed.");
		return;
	}

	std::wstring command = args;

	// Parse command
	size_t spacePos = command.find(L' ');
	std::wstring action = (spacePos != std::wstring::npos) ?
		command.substr(0, spacePos) : command;
	std::wstring param = (spacePos != std::wstring::npos) ?
		command.substr(spacePos + 1) : L"";

	// WebView Commands
	if (_wcsicmp(action.c_str(), L"WebView") == 0)
	{
		if (_wcsicmp(param.c_str(), L"Start") == 0)
		{
			CreateWebView2(measure);
			return;
		}
		else if (_wcsicmp(param.c_str(), L"Stop") == 0)
		{
			StopWebView2(measure);
			return;
		}
		else if (_wcsicmp(param.c_str(), L"Restart") == 0)
		{
			RestartWebView2(measure);
			return;
		}
		else
		{
			RmLog(measure->rm, LOG_ERROR, L"WebView2: Unknown WebView command");
			return;
		}
	}

	if (!measure->webView)
	{
		RmLog(measure->rm, LOG_ERROR, L"WebView2: Not running");
		return;
	}

	// Navigation Commands
	if (_wcsicmp(action.c_str(), L"Navigate") == 0)
	{
		if (_wcsicmp(param.c_str(), L"Stop") == 0)
		{
			measure->webView->Stop();
		}
		else if (_wcsicmp(param.c_str(), L"Reload") == 0)
		{
			measure->webView->Reload();
		}
		else if (_wcsicmp(param.c_str(), L"Back") == 0)
		{
			measure->webView->GoBack();
		}
		else if (_wcsicmp(param.c_str(), L"Forward") == 0)
		{
			measure->webView->GoForward();
		}
		else if (_wcsicmp(param.c_str(), L"Home") == 0)
		{
			measure->webView->Navigate(measure->url.c_str());
		}
		else
		{
			// Navigate to URL
			measure->webView->Navigate(param.c_str());
		}
	}
	else if (_wcsicmp(action.c_str(), L"Open") == 0)
	{
		if (_wcsicmp(param.c_str(), L"DevTools") == 0)
		{
			measure->webView->OpenDevToolsWindow();

		}
		else if (_wcsicmp(param.c_str(), L"TaskManager") == 0)
		{
			measure->webView6->OpenTaskManagerWindow();
		}
	}
	else if (_wcsicmp(action.c_str(), L"ExecuteScript") == 0)
	{
		if (!param.empty())
		{
			measure->webView->ExecuteScript(
				param.c_str(),
				Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
					[](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT
					{
						return S_OK;
					}
				).Get()
			);
		}
	}
	else
	{
		RmLogF(measure->rm, LOG_ERROR, L"WebView2: Unknown command - %s", action.c_str());
	}
}

// Generic JavaScript function caller
PLUGIN_EXPORT LPCWSTR CallJS(void* data, const int argc, const WCHAR* argv[])
{
	Measure* measure = (Measure*)data;

	if (measure->disabled)
	{
		return L"";
	}

	if (!measure || !measure->initialized || !measure->webView)
		return L"";

	if (argc == 0 || !argv[0])
		return L"";

	// Build unique key for this call: functionName|arg1|arg2...
	std::wstring key = argv[0];
	for (int i = 1; i < argc; i++)
	{
		key += L"|";
		key += argv[i];
	}

	// Build JavaScript call: functionName(arg1, arg2, ...)
	std::wstring jsCode = L"(function() { try { if (typeof " + std::wstring(argv[0]) + L" === 'function') { var result = " + std::wstring(argv[0]) + L"(";

	// Add arguments if provided
	for (int i = 1; i < argc; i++)
	{
		if (i > 1) jsCode += L", ";
		jsCode += L"'" + std::wstring(argv[i]) + L"'";
	}

	jsCode += L"); return result !== undefined ? String(result) : ''; } return 'Function not found'; } catch(e) { return 'Error: ' + e.message; } })();";

	// Execute asynchronously and update cache
	measure->webView->ExecuteScript(
		jsCode.c_str(),
		Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
			[measure, key](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT
			{
				if (SUCCEEDED(errorCode) && resultObjectAsJson)
				{
					std::wstring result = resultObjectAsJson;
					if (result.length() >= 2 && result.front() == L'"' && result.back() == L'"')
					{
						result = result.substr(1, result.length() - 2);
					}

					if (!result.empty() && result != L"null")
					{
						// Update cache for this specific call
						measure->jsResults[key] = result;
					}
				}
				return S_OK;
			}
		).Get()
	);

	// Return cached result if available, otherwise "0"
	if (measure->jsResults.find(key) != measure->jsResults.end())
	{
		measure->buffer = measure->jsResults[key];
	}
	else
	{
		measure->buffer = L"0";
	}

	return measure->buffer.c_str();
}

PLUGIN_EXPORT void Finalize(void* data)
{
	Measure* measure = (Measure*)data;
	if (!measure) return;
	SkinSubclassData* skinData = measure->skinData;

	SkinSubclassData* toDelete = nullptr;

	if (skinData)
	{
		// Modify map + decide deletion under lock
		std::lock_guard<std::mutex> lg(g_skinMapMutex);

		skinData->destroying = true;
		skinData->measures.erase(measure);
		skinData->refCount--;

		// If no more measures for this skin, mark for deletion
		if (skinData->refCount <= 0)
		{
			g_SkinSubclassMap.erase(skinData->hwnd);
			toDelete = skinData;
		}
	}

	// Remove subclass and delete outside lock
	if (toDelete)
	{
		RemoveWindowSubclass(
			toDelete->hwnd,
			SkinSubclassProc,
			1
		);
		delete toDelete;
	}

	// Stop WebView2 and clean up
	StopWebView2(measure);

	g_refCount--;

	// Remove keyboard hook if no more measures exist
	if (g_refCount == 0)
	{
		RemoveKeyboardHook();
	}

	delete measure;
}