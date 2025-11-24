#include <Windows.h>
#include <string>
#include <wrl.h>
#include <wil/com.h>
#include "../API/RainmeterAPI.h"

// WebView2 includes
#include <WebView2.h>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

using namespace Microsoft::WRL;

// Global COM initialization tracking
static bool g_comInitialized = false;

struct Measure
{
    void* rm;
    void* skin;
    HWND skinWindow;
    HWND webViewWindow;
    LPCWSTR measureName;
    
    std::wstring url;
    int width;
    int height;
    int x;
    int y;
    bool visible;
    bool initialized;
    
    wil::com_ptr<ICoreWebView2Controller> webViewController;
    wil::com_ptr<ICoreWebView2> webView;
    
    Measure() : rm(nullptr), skin(nullptr), skinWindow(nullptr), 
                webViewWindow(nullptr), measureName(nullptr),
                width(800), height(600), x(0), y(0), 
                visible(true), initialized(false)
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
    
    ~Measure()
    {
        if (webViewController)
        {
            webViewController->Close();
            webViewController = nullptr;
        }
        
        if (webViewWindow && IsWindow(webViewWindow))
        {
            DestroyWindow(webViewWindow);
            webViewWindow = nullptr;
        }
    }
};

// Window procedure for WebView2 host window
LRESULT CALLBACK WebViewWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_SIZE:
        {
            Measure* measure = reinterpret_cast<Measure*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            if (measure && measure->webViewController)
            {
                RECT bounds;
                GetClientRect(hwnd, &bounds);
                measure->webViewController->put_Bounds(bounds);
            }
            return 0;
        }
        case WM_DESTROY:
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Register window class for WebView2 host
void RegisterWebViewWindowClass()
{
    static bool registered = false;
    if (!registered)
    {
        WNDCLASSEX wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WebViewWndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = GetModuleHandle(nullptr);
        wcex.hIcon = nullptr;
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = L"RainmeterWebView2Host";
        wcex.hIconSm = nullptr;
        
        RegisterClassEx(&wcex);
        registered = true;
    }
}

// Create WebView2 environment and controller
void CreateWebView2(Measure* measure)
{
    if (!measure || !measure->skinWindow)
    {
        if (measure && measure->rm)
            RmLog(measure->rm, LOG_ERROR, L"WebView2: Invalid measure or skin window");
        return;
    }
    
    // Register window class
    RegisterWebViewWindowClass();
    
    // Create host window as child of skin window
    measure->webViewWindow = CreateWindowEx(
        0,
        L"RainmeterWebView2Host",
        L"WebView2",
        WS_CHILD | (measure->visible ? WS_VISIBLE : 0),
        measure->x, measure->y,
        measure->width, measure->height,
        measure->skinWindow,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );
    
    if (!measure->webViewWindow)
    {
        if (measure->rm)
            RmLog(measure->rm, LOG_ERROR, L"WebView2: Failed to create host window");
        return;
    }
    
    // Store measure pointer in window
    SetWindowLongPtr(measure->webViewWindow, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(measure));
    
    // Create user data folder in TEMP directory to avoid permission issues
    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    std::wstring userDataFolder = std::wstring(tempPath) + L"RainmeterWebView2";
    
    // Create the directory if it doesn't exist
    CreateDirectoryW(userDataFolder.c_str(), nullptr);
    
    // Create WebView2 environment with user data folder
    HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
        nullptr, userDataFolder.c_str(), nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [measure](HRESULT result, ICoreWebView2Environment* env) -> HRESULT
            {
                if (FAILED(result))
                {
                    if (measure->rm)
                    {
                        wchar_t errorMsg[256];
                        swprintf_s(errorMsg, L"WebView2: Failed to create environment (HRESULT: 0x%08X)", result);
                        RmLog(measure->rm, LOG_ERROR, errorMsg);
                    }
                    return result;
                }
                
                // Create WebView2 controller
                env->CreateCoreWebView2Controller(
                    measure->webViewWindow,
                    Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                        [measure](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT
                        {
                            if (FAILED(result))
                            {
                                if (measure->rm)
                                {
                                    wchar_t errorMsg[256];
                                    swprintf_s(errorMsg, L"WebView2: Failed to create controller (HRESULT: 0x%08X)", result);
                                    RmLog(measure->rm, LOG_ERROR, errorMsg);
                                }
                                return result;
                            }
                            
                            measure->webViewController = controller;
                            measure->webViewController->get_CoreWebView2(&measure->webView);
                            
                            // Set bounds
                            RECT bounds;
                            GetClientRect(measure->webViewWindow, &bounds);
                            measure->webViewController->put_Bounds(bounds);
                            
                            // Navigate to URL
                            if (!measure->url.empty())
                            {
                                measure->webView->Navigate(measure->url.c_str());
                            }
                            
                            measure->initialized = true;
                            
                            if (measure->rm)
                                RmLog(measure->rm, LOG_NOTICE, L"WebView2: Initialized successfully");
                            
                            return S_OK;
                        }
                    ).Get()
                );
                
                return S_OK;
            }
        ).Get()
    );
    
    if (FAILED(hr))
    {
        if (measure->rm)
        {
            wchar_t errorMsg[512];
            swprintf_s(errorMsg, L"WebView2: Failed to start creation process (HRESULT: 0x%08X). Make sure WebView2 Runtime is installed.", hr);
            RmLog(measure->rm, LOG_ERROR, errorMsg);
        }
    }
}

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
    Measure* measure = new Measure;
    *data = measure;
    
    measure->rm = rm;
    measure->skin = RmGetSkin(rm);
    measure->skinWindow = RmGetSkinWindow(rm);
    measure->measureName = RmGetMeasureName(rm);
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
    Measure* measure = (Measure*)data;
    
    // Read URL option
    LPCWSTR urlOption = RmReadString(rm, L"Url", L"");
    if (urlOption && *urlOption)
    {
        // Check if it's a file path
        std::wstring urlStr = urlOption;
        if (urlStr.find(L"://") == std::wstring::npos)
        {
            // It's a file path, convert to absolute path
            LPCWSTR absolutePath = RmPathToAbsolute(rm, urlOption);
            
            // Convert to file:/// URL
            std::wstring fileUrl = L"file:///";
            std::wstring path = absolutePath;
            
            // Replace backslashes with forward slashes
            for (size_t i = 0; i < path.length(); i++)
            {
                if (path[i] == L'\\')
                    path[i] = L'/';
            }
            
            measure->url = fileUrl + path;
        }
        else
        {
            // It's already a URL
            measure->url = urlStr;
        }
    }
    
    // Read dimensions
    measure->width = RmReadInt(rm, L"Width", 800);
    measure->height = RmReadInt(rm, L"Height", 600);
    measure->x = RmReadInt(rm, L"X", 0);
    measure->y = RmReadInt(rm, L"Y", 0);
    
    // Read visibility
    measure->visible = RmReadInt(rm, L"Visible", 1) != 0;
    
    // Create WebView2 if not already created
    if (!measure->initialized)
    {
        CreateWebView2(measure);
    }
    else
    {
        // Update existing WebView
        if (measure->webView && !measure->url.empty())
        {
            measure->webView->Navigate(measure->url.c_str());
        }
        
        // Update window position and size
        if (measure->webViewWindow)
        {
            SetWindowPos(
                measure->webViewWindow,
                nullptr,
                measure->x, measure->y,
                measure->width, measure->height,
                SWP_NOZORDER | SWP_NOACTIVATE
            );
            
            ShowWindow(measure->webViewWindow, measure->visible ? SW_SHOW : SW_HIDE);
        }
    }
}

PLUGIN_EXPORT double Update(void* data)
{
    Measure* measure = (Measure*)data;
    return measure->initialized ? 1.0 : 0.0;
}

PLUGIN_EXPORT LPCWSTR GetString(void* data)
{
    Measure* measure = (Measure*)data;
    static std::wstring result;
    
    if (measure->initialized)
    {
        result = L"WebView2 Initialized";
    }
    else
    {
        result = L"WebView2 Initializing...";
    }
    
    return result.c_str();
}

PLUGIN_EXPORT void ExecuteBang(void* data, LPCWSTR args)
{
    Measure* measure = (Measure*)data;
    
    if (!measure || !measure->webView)
        return;
    
    std::wstring command = args;
    
    // Parse command
    size_t spacePos = command.find(L' ');
    std::wstring action = (spacePos != std::wstring::npos) ? 
                          command.substr(0, spacePos) : command;
    std::wstring param = (spacePos != std::wstring::npos) ? 
                         command.substr(spacePos + 1) : L"";
    
    if (_wcsicmp(action.c_str(), L"Navigate") == 0)
    {
        if (!param.empty())
        {
            measure->webView->Navigate(param.c_str());
        }
    }
    else if (_wcsicmp(action.c_str(), L"Reload") == 0)
    {
        measure->webView->Reload();
    }
    else if (_wcsicmp(action.c_str(), L"GoBack") == 0)
    {
        measure->webView->GoBack();
    }
    else if (_wcsicmp(action.c_str(), L"GoForward") == 0)
    {
        measure->webView->GoForward();
    }
    else if (_wcsicmp(action.c_str(), L"Show") == 0)
    {
        if (measure->webViewWindow)
        {
            ShowWindow(measure->webViewWindow, SW_SHOW);
            measure->visible = true;
        }
    }
    else if (_wcsicmp(action.c_str(), L"Hide") == 0)
    {
        if (measure->webViewWindow)
        {
            ShowWindow(measure->webViewWindow, SW_HIDE);
            measure->visible = false;
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
}

PLUGIN_EXPORT void Finalize(void* data)
{
    Measure* measure = (Measure*)data;
    delete measure;
}
