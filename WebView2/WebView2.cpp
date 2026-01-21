/**
*  Copyright (C) 2025 nstechbytes. All rights reserved.
*/ 

#include "Plugin.h"
#include "Utils.h"
#include "PathUtils.h"
#include "HostObjectRmAPI.h"
#include "../API/RainmeterAPI.h"
#include <WebView2EnvironmentOptions.h>
#include <filesystem>

bool g_extensions_checked = false;

std::vector<std::wstring> GetExtensionsID(const std::wstring& input)
{
	std::vector<std::wstring> result;
	std::wstringstream ss(input);
	std::wstring token;

	while (std::getline(ss, token, L',')) {
		token.erase(0, token.find_first_not_of(L" \t"));
		token.erase(token.find_last_not_of(L" \t") + 1);

		if (!token.empty()) {
			result.push_back(token);
		}
	}
	return result;
}

static void EnableExtension(
	ICoreWebView2BrowserExtension* extension,
	BOOL enable)
{
	extension->Enable(
		enable,
		Callback<ICoreWebView2BrowserExtensionEnableCompletedHandler>(
			[](HRESULT hr) -> HRESULT
			{
				if (FAILED(hr))
					ShowFailure(hr, L"Enable extension failed");
				return S_OK;
			}).Get());
}

static void RemoveExtension(void* rm,
	ICoreWebView2BrowserExtension* extension,
	const std::wstring& name)
{
	extension->Remove(
		Callback<ICoreWebView2BrowserExtensionRemoveCompletedHandler>(
			[](HRESULT hr) -> HRESULT
			{
				if (FAILED(hr))
					ShowFailure(hr, L"Uninstall extension failed");
				return S_OK;
			}).Get());

	RmLogF(rm, LOG_DEBUG, L"WebView2: \"%s\" extension removed.", name.c_str());
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

	if (measure->initialized)
	{
		RmLog(measure->rm, LOG_ERROR, L"WebView2: Already started");
		return;
	}

	if (measure->isCreationInProgress)
	{
		RmLog(measure->rm, LOG_ERROR, L"WebView2: Initialization already in progress");
		return;
	}

	measure->isCreationInProgress = true;

	// Load or create UserSettings.ini
	measure->userSettingsFile.SetUnicode();
	measure->userSettingsFile.LoadFile(measure->configPath.c_str());
	measure->userSettingsChanged = false;

	// Read options from UserSettings.ini
	bool fluentBars = GetIniBool(measure->userSettingsFile, measure->userSettingsChanged, L"Environment", L"FluentOverlayScrollBars", true); // Fluent Bars
	bool trackingPrevention = GetIniBool(measure->userSettingsFile, measure->userSettingsChanged, L"Environment", L"TrackingPrevention", true);	// Tracking Prevention (SmartScreen)
	bool extensions = GetIniBool(measure->userSettingsFile, measure->userSettingsChanged, L"Environment", L"Extensions", false); // Extensions
	std::wstring language = GetIniString(measure->userSettingsFile, measure->userSettingsChanged, L"Environment", L"BrowserLocale", L"system"); // Language 
	// Available browser flags: https://learn.microsoft.com/en-us/microsoft-edge/webview2/concepts/webview-features-flags?tabs=win32cpp#available-webview2-browser-flags
	std::wstring userBrowserArgs = GetIniString(measure->userSettingsFile, measure->userSettingsChanged, L"Environment", L"BrowserArguments", L"--allow-file-access-from-files"); // Browser Flags
	std::wstring browserArgs;
	browserArgs.append(L"--enable-features="); // Enable file access from file URLs
	browserArgs.append(userBrowserArgs);
	
	// Add environment options
	auto environmentOptions = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();

	environmentOptions->put_AdditionalBrowserArguments(browserArgs.c_str()); // Flags

	if (language == L"system")
		environmentOptions->put_Language(measure->osLocale); // Browser Locale : System Locale
	else 
		environmentOptions->put_Language(language.c_str()); // Browser Locale : Custom Locale


	Microsoft::WRL::ComPtr<ICoreWebView2EnvironmentOptions5> environmentOptions5;
	if (environmentOptions.As(&environmentOptions5) == S_OK)
	{
		environmentOptions5->put_EnableTrackingPrevention(trackingPrevention); // Tracking Prevention
	}
	Microsoft::WRL::ComPtr<ICoreWebView2EnvironmentOptions6> environmentOptions6;
	if (environmentOptions.As(&environmentOptions6) == S_OK)
	{
		environmentOptions6->put_AreBrowserExtensionsEnabled(extensions); // Extensions
	}
	Microsoft::WRL::ComPtr<ICoreWebView2EnvironmentOptions8> environmentOptions8;
	if (environmentOptions.As(&environmentOptions8) == S_OK)
	{
		environmentOptions8->put_ScrollBarStyle(fluentBars ? COREWEBVIEW2_SCROLLBAR_STYLE_FLUENT_OVERLAY : COREWEBVIEW2_SCROLLBAR_STYLE_DEFAULT); // Set Fluent Scrollbars
	}

	// Create WebView2 environment with user data folder
	HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
		nullptr, measure->userDataFolder.c_str(), environmentOptions.Get(),
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			measure,
			&Measure::CreateEnvironmentHandler
		).Get()

	);

	if (!SUCCEEDED(hr))
	{
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			int result = MessageBox(
				measure->skinWindow,
				L"Failed to start creation process.\n\nMake sure WebView2 Runtime is installed.\n\n"
				L"Would you like to be redirected to the WebView2 Runtime download page?",
				L"Missing Dependency",
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
			measure->FailWebView(hr, L"WebView2: Failed to start environment creation process. Make sure WebView2 Runtime is installed.");
		}
		else
		{
			ShowFailure(hr, L"Failed to create webview environment.");
			measure->FailWebView(hr, L"Failed to create webview environment.");
		}
	}
}

// Environment creation callback
HRESULT Measure::CreateEnvironmentHandler(HRESULT result, ICoreWebView2Environment* env)
{
	if (!SUCCEEDED(result))
	{
		return FailWebView(result, L"WebView2: Failed to create webview environment.");
	}

	webViewEnvironment = env;

	// Create WebView2 controller with options.
	auto webViewEnvironment10 = webViewEnvironment.try_query<ICoreWebView2Environment10>();
	if (!webViewEnvironment10)
	{
		FeatureNotAvailable();

		webViewEnvironment->CreateCoreWebView2Controller(
			skinWindow,
			Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
				this,
				&Measure::CreateControllerHandler
			).Get()
		);
	}
	else
	{
		wil::com_ptr<ICoreWebView2ControllerOptions> controllerOptions;
		HRESULT hr = webViewEnvironment10->CreateCoreWebView2ControllerOptions(&controllerOptions);
		if (hr == E_INVALIDARG)
		{
			ShowFailure(hr, L"Unable to create WebView2 due to an invalid profile name.");
			return S_OK;
		}
		CHECK_FAILURE(hr);

		// Read options from UserSettings.ini
		std::wstring scriptLocale = GetIniString(userSettingsFile, userSettingsChanged, L"Controller", L"ScriptLocale", L"system");
		bool privateMode = GetIniBool(userSettingsFile, userSettingsChanged, L"Controller", L"PrivateMode", false);

		// OPTIONS
		controllerOptions->put_ProfileName(L"rainmeter"); // Profile Name
		controllerOptions->put_IsInPrivateModeEnabled(privateMode); // Private/Incognito Mode
		if (SUCCEEDED(controllerOptions->QueryInterface(
			IID_PPV_ARGS(&webViewControllerOptions2))))
		{

			if (scriptLocale == L"system")
			{
				webViewControllerOptions2->put_ScriptLocale(osLocale); // System Locale
			}
			else
			{
				webViewControllerOptions2->put_ScriptLocale(scriptLocale.c_str()); // Custom Locale
			}
		}

		// Set Transparent Background
		wil::com_ptr<ICoreWebView2ControllerOptions3> controllerOptions3;
		if (SUCCEEDED(controllerOptions->QueryInterface(IID_PPV_ARGS(&controllerOptions3))))
		{
			COREWEBVIEW2_COLOR transparentColor = { 0, 0, 0, 0 };
			controllerOptions3->put_DefaultBackgroundColor(transparentColor); // Background Color
		}

		// Create Controller With Options.
		webViewEnvironment10->CreateCoreWebView2ControllerWithOptions(
			skinWindow,
			controllerOptions.get(),
			Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
				this,
				&Measure::CreateControllerHandler
			).Get()
		);
	}

	return S_OK;
}

void RegisterFrames(Measure* measure, ICoreWebView2Frame* rawFrame, int level)
{
	wil::com_ptr<ICoreWebView2Frame> frame = rawFrame;

	wil::com_ptr<ICoreWebView2Frame2> frame2 = frame.try_query<ICoreWebView2Frame2>();

	// Only proceed if we have valid interfaces
	if (!frame2) return;

	// Add host object
	wil::com_ptr<HostObjectRmAPI> hostObject =
		Microsoft::WRL::Make<HostObjectRmAPI>(measure, g_typeLib);

	wil::unique_variant hostObjectVariant;
	hostObject.query_to<IDispatch>(&hostObjectVariant.pdispVal);
	hostObjectVariant.vt = VT_DISPATCH;

	std::wstring origin = NormalizeUri(measure->currentUrl);
	LPCWSTR origins = L"*"; // all-origins

	CHECK_FAILURE(frame2->AddHostObjectToScriptWithOrigins(L"RainmeterAPI", &hostObjectVariant, 1, &origins));
	VariantClear(&hostObjectVariant);

	// Inject frame ancestor to nested frames to allow framing websites. (Requires virtual host or http-server).
	frame2->add_NavigationStarting(
		Microsoft::WRL::Callback<ICoreWebView2FrameNavigationStartingEventHandler>(
			[measure, origin](ICoreWebView2Frame* sender, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT
			{
				wil::com_ptr<ICoreWebView2NavigationStartingEventArgs2> navigationStartArgs;
				if (SUCCEEDED(args->QueryInterface(IID_PPV_ARGS(&navigationStartArgs))))
				{
					navigationStartArgs->put_AdditionalAllowedFrameAncestors(origin.c_str());
				}
				return S_OK;
			}
		).Get(), nullptr
	);

	wil::com_ptr<ICoreWebView2Frame7> frame7 = frame.try_query<ICoreWebView2Frame7>();
	if (frame7)
	{
		CHECK_FAILURE(frame7->add_FrameCreated(
			Microsoft::WRL::Callback<ICoreWebView2FrameChildFrameCreatedEventHandler>(
				[measure, level](ICoreWebView2Frame* sender, ICoreWebView2FrameCreatedEventArgs* args) -> HRESULT
				{
					wil::com_ptr<ICoreWebView2Frame> childFrame;
					CHECK_FAILURE(args->get_Frame(&childFrame));
					// RECURSIVE CALL:
					RegisterFrames(measure, childFrame.get(), level + 1);
					return S_OK;
				}).Get(), nullptr));
	}
}

// Controller creation callback
HRESULT Measure::CreateControllerHandler(HRESULT result, ICoreWebView2Controller* controller)
{
	if (FAILED(result))
	{
		return FailWebView(result, L"WebView2: Failed to create controller");
	}

	if (!controller)
	{
		return FailWebView(S_FALSE, L"WebView2: Controller is null");
	}

	if (result == S_OK)
	{
		// WebView is initializing
		SetStateAndNotify(0);

		webViewController = controller;
		CHECK_FAILURE(webViewController->get_CoreWebView2(&webView));

		webViewArea = {x, y, x + width, y + height };
		
		// CONTROLLER OPTIONS
		webViewController->put_Bounds(webViewArea); // Set initial bounds
		webViewController->put_IsVisible(visible); // Set initial visibility
		webViewController->put_ZoomFactor(zoomFactor); // Set initial zoom factor

		// Set Focus when required.
		webViewController->add_MoveFocusRequested(
			Microsoft::WRL::Callback<ICoreWebView2MoveFocusRequestedEventHandler>(
				[this](ICoreWebView2Controller* sender, ICoreWebView2MoveFocusRequestedEventArgs* args) -> HRESULT
				{
					enum COREWEBVIEW2_MOVE_FOCUS_REASON reason;
					args->get_Reason(&reason);

					if (reason == COREWEBVIEW2_MOVE_FOCUS_REASON_PROGRAMMATIC)
						args->put_Handled(TRUE);

					return S_OK;
				}
			).Get(), nullptr
		);

		// Set isWebViewFocused variable
		webViewController->add_GotFocus(
			Microsoft::WRL::Callback<ICoreWebView2FocusChangedEventHandler>(
				[this](ICoreWebView2Controller* sender, IUnknown* args) -> HRESULT
				{
					isWebViewFocused = true;
					return S_OK;
				}
			).Get(), nullptr
		);

		// Set isWebViewFocused variable
		webViewController->add_LostFocus(
			Microsoft::WRL::Callback<ICoreWebView2FocusChangedEventHandler>(
				[this](ICoreWebView2Controller* sender, IUnknown* args) -> HRESULT
				{
					isWebViewFocused = false;
					return S_OK;
				}
			).Get(), nullptr
		);

		// Accelerator Keys
		webViewController->add_AcceleratorKeyPressed(
			Microsoft::WRL::Callback<ICoreWebView2AcceleratorKeyPressedEventHandler>(
				[this](ICoreWebView2Controller* sender, ICoreWebView2AcceleratorKeyPressedEventArgs* args) -> HRESULT
				{
					COREWEBVIEW2_KEY_EVENT_KIND kind;
					args->get_KeyEventKind(&kind);
					// We only care about key down events.
					if (kind == COREWEBVIEW2_KEY_EVENT_KIND_KEY_DOWN ||
						kind == COREWEBVIEW2_KEY_EVENT_KIND_SYSTEM_KEY_DOWN)
					{
						UINT key;
						args->get_VirtualKey(&key);

						wil::com_ptr<ICoreWebView2AcceleratorKeyPressedEventArgs2> args2;
						args->QueryInterface(IID_PPV_ARGS(&args2));

						if (args2)
						{
							if (!assistiveFeatures) // Disable Assistive Features Shortcut Keys
							{
								if (key == 'F')
								{
									args2->put_IsBrowserAcceleratorKeyEnabled(FALSE);
								}
								if (key == 'G')
								{
									args2->put_IsBrowserAcceleratorKeyEnabled(FALSE);
								}
								if (key == 'G' && (GetKeyState(VK_CONTROL) < 0) && (GetKeyState(VK_SHIFT) < 0))
								{
									args2->put_IsBrowserAcceleratorKeyEnabled(FALSE);
								}
								if (key == 'P')
								{
									args2->put_IsBrowserAcceleratorKeyEnabled(FALSE);
								}
								if (key == 'P' && (GetKeyState(VK_CONTROL) < 0) && (GetKeyState(VK_SHIFT) < 0))
								{
									args2->put_IsBrowserAcceleratorKeyEnabled(FALSE);
								}
								if (key == VK_F3)
								{
									args2->put_IsBrowserAcceleratorKeyEnabled(FALSE);
								}
								if (key == VK_F7)
								{
									args2->put_IsBrowserAcceleratorKeyEnabled(FALSE);
								}
							}
						}
					}
					return S_OK;
				}
			).Get(), nullptr
		);

		// CORE SETTINGS
		CHECK_FAILURE(webView->get_Settings(&webViewSettings));
		webViewSettings->put_IsScriptEnabled(TRUE);
		webViewSettings->put_AreDefaultScriptDialogsEnabled(TRUE);
		webViewSettings->put_IsWebMessageEnabled(TRUE);
		webViewSettings->put_AreHostObjectsAllowed(TRUE);
		webViewSettings->put_AreDevToolsEnabled(TRUE);
		webViewSettings->put_IsZoomControlEnabled(zoomControl);

		// Read options from UserSettings.ini
		bool statusBar = GetIniBool(userSettingsFile, userSettingsChanged, L"Core", L"StatusBar", true);
		bool pinchZoom = GetIniBool(userSettingsFile, userSettingsChanged, L"Core", L"PinchZoom", true);
		bool swipeNavigation = GetIniBool(userSettingsFile, userSettingsChanged, L"Core", L"SwipeNavigation", true);
		bool reputationChecking = GetIniBool(userSettingsFile, userSettingsChanged, L"Core", L"SmartScreen", true);

		webViewSettings->put_IsStatusBarEnabled(statusBar);

		webViewSettings2 = webViewSettings.try_query<ICoreWebView2Settings2>();
		if (webViewSettings2 && !userAgent.empty())
		{
			webViewSettings2->put_UserAgent(userAgent.c_str()); // User Agent
		}

		auto settings5 = webViewSettings.try_query<ICoreWebView2Settings5>();
		if (settings5)
		{
			settings5->put_IsPinchZoomEnabled(pinchZoom); // Pinch Zoom
		}

		auto settings6 = webViewSettings.try_query<ICoreWebView2Settings6>();
		if (settings6)
		{
			settings6->put_IsSwipeNavigationEnabled(swipeNavigation); // Swipe Navigation
		}

		auto settings7 = webViewSettings.try_query<ICoreWebView2Settings7>();
		if (settings7)
		{
			settings7->put_HiddenPdfToolbarItems(COREWEBVIEW2_PDF_TOOLBAR_ITEMS_FULL_SCREEN); // Hide pdf reader's full-screen button
		}

		auto settings8 = webViewSettings.try_query<ICoreWebView2Settings8>();
		if (settings8)
		{
			settings8->put_IsReputationCheckingRequired(reputationChecking); // SmartScreen reputation checking
		}

		auto settings9 = webViewSettings.try_query<ICoreWebView2Settings9>();
		if (settings9)
		{
			settings9->put_IsNonClientRegionSupportEnabled(TRUE); // Enable app-region css style support
		}

		// PROFILE OPTIONS
		auto webView2_13 = webView.try_query<ICoreWebView2_13>();
		if (webView2_13)
		{
			wil::com_ptr<ICoreWebView2Profile> profile;
			CHECK_FAILURE(webView2_13->get_Profile(&profile));
			
			// Read options from UserSettings.ini
			std::wstring downloadsFolder = GetIniString(userSettingsFile, userSettingsChanged, L"Profile", L"DownloadsFolderPath", L"");
			std::wstring colorScheme = GetIniString(userSettingsFile, userSettingsChanged, L"Profile", L"ColorScheme", L"system");
			bool passAutoSave = GetIniBool(userSettingsFile, userSettingsChanged, L"Profile", L"PasswordAutoSave", false);
			bool generalAutoFill = GetIniBool(userSettingsFile, userSettingsChanged, L"Profile", L"GeneralAutoFill", true);

			profile->put_DefaultDownloadFolderPath(downloadsFolder.c_str()); // Downloads folder path

			if (_wcsicmp(colorScheme.c_str(), L"light") == 0)	// Color Scheme
			{
				profile->put_PreferredColorScheme(COREWEBVIEW2_PREFERRED_COLOR_SCHEME_LIGHT);
			}
			else if (_wcsicmp(colorScheme.c_str(), L"dark") == 0)
			{
				profile->put_PreferredColorScheme(COREWEBVIEW2_PREFERRED_COLOR_SCHEME_DARK);
			}
			else
			{
				profile->put_PreferredColorScheme(COREWEBVIEW2_PREFERRED_COLOR_SCHEME_AUTO);
			}

			auto profile6 = profile.try_query<ICoreWebView2Profile6>();
			if (profile6)
			{
				profile6->put_IsPasswordAutosaveEnabled(passAutoSave); // Password AutoSave
				profile6->put_IsGeneralAutofillEnabled(generalAutoFill); // General AutoFill
			}

			// Extensions
			auto profile7 = profile.try_query<ICoreWebView2Profile7>();
			const std::filesystem::path extensionsRoot = std::filesystem::path(userDataFolder) / L"Extensions";

			if (profile7 && std::filesystem::exists(extensionsRoot) && !g_extensions_checked)
			{
				g_extensions_checked = true;

				profile7->GetBrowserExtensions(
					Callback<ICoreWebView2ProfileGetBrowserExtensionsCompletedHandler>(
						[this, profile7, extensionsRoot]
						(HRESULT error, ICoreWebView2BrowserExtensionList* extensions) -> HRESULT
						{
							if (FAILED(error) || !extensions)
							{
								ShowFailure(error, L"GetBrowserExtensions failed");
								return S_OK;
							}

							// Load Extensions.ini
							extensionsFile.SetUnicode();
							extensionsFile.LoadFile(extensionsPath.c_str());
							extensionsChanged = false;

							UINT extensionsCount = 0;
							extensions->get_Count(&extensionsCount);

							auto addExtension = [&](const std::filesystem::path& path, const std::wstring& folderName)
								{
									profile7->AddBrowserExtension(
										path.c_str(),
										Callback<ICoreWebView2ProfileAddBrowserExtensionCompletedHandler>(
											[this, folderName]
											(HRESULT error, ICoreWebView2BrowserExtension* extension) mutable -> HRESULT
											{
												if (FAILED(error))
												{
													if (error == ERROR_FILE_NOT_FOUND)
														RmLog(rm, LOG_ERROR, L"WebView2: Invalid extension path or manifest.json file is missing.");
													else if (error == ERROR_NOT_SUPPORTED)
														RmLog(rm, LOG_ERROR, L"WebView2: Extensions are disabled.");

													return S_OK;
												}

												wil::unique_cotaskmem_string id;
												wil::unique_cotaskmem_string name;

												extension->get_Id(&id);
												extension->get_Name(&name);

												// Create the extension's section on Extensions.ini
												extensionsFile.SetValue(folderName.c_str(), L"ID", id.get());
												extensionsFile.SetValue(folderName.c_str(), L"Name", name.get());
												extensionsFile.SetValue(folderName.c_str(), L"Enabled", L"true");
												extensionsFile.SetValue(folderName.c_str(), L"Uninstall", L"false");
												extensionsFile.SaveFile(extensionsPath.c_str());
												extensionsChanged = false;

												extension->Enable(
													TRUE,
													Callback<ICoreWebView2BrowserExtensionEnableCompletedHandler>(
														[](HRESULT hr) -> HRESULT
														{
															if (FAILED(hr))
																ShowFailure(hr, L"Enable extension failed");
															return S_OK;
														}).Get());

												RmLogF(rm, LOG_NOTICE, L"WebView2: \"%s\" extension installed.", name.get());
												return S_OK;
											}).Get());
								};

							// Check extensions folders at Extensions\/
							for (const auto& entry : std::filesystem::directory_iterator(extensionsRoot))
							{
								if (!entry.is_directory())
									continue;

								const auto& extensionPath = entry.path();
								const std::wstring folderName = extensionPath.filename().wstring();
								// Look for already saved ID on Extensions.ini for this extension
								const std::wstring savedID = extensionsFile.GetValue(folderName.c_str(), L"ID", L"");

								if (savedID.empty()) // No ID found
								{
									addExtension(extensionPath, folderName);
									continue;
								}

								bool found = false;

								for (UINT i = 0; i < extensionsCount; ++i) 
								{
									wil::com_ptr<ICoreWebView2BrowserExtension> extension;
									extensions->GetValueAtIndex(i, &extension);

									wil::unique_cotaskmem_string id;
									extension->get_Id(&id);

									if (savedID != id.get()) // Check which already installed extension matches ID.
										continue;

									found = true;

									wil::unique_cotaskmem_string name;
									BOOL enabled = FALSE;

									extension->get_Name(&name);
									extension->get_IsEnabled(&enabled);

									// Read options from Extensions.ini
									const bool enable = GetIniBool(extensionsFile, extensionsChanged, folderName.c_str(), L"Enabled", true);
									const bool remove = GetIniBool(extensionsFile, extensionsChanged, folderName.c_str(), L"Uninstall", false);

									if (remove) // Uninstall Extension
									{
										extension->Remove(
											Callback<ICoreWebView2BrowserExtensionRemoveCompletedHandler>(
												[](HRESULT hr) -> HRESULT
												{
													if (FAILED(hr))
														ShowFailure(hr, L"Uninstall extension failed");
													return S_OK;
												}).Get());

										// Delete section from Extensions.ini
										extensionsFile.Delete(folderName.c_str(), nullptr);
										extensionsFile.SaveFile(extensionsPath.c_str());
										extensionsChanged = false;

										RmLogF(rm, LOG_NOTICE, L"WebView2: \"%s\" extension removed.", name.get());
										return S_OK;
									}

									if (enabled != enable) // Toggle Extension
									{
										extension->Enable(
											enable,
											Callback<ICoreWebView2BrowserExtensionEnableCompletedHandler>(
												[](HRESULT hr) -> HRESULT
												{
													if (FAILED(hr))
														ShowFailure(hr, L"Enable extension failed");
													return S_OK;
												}).Get());

										RmLogF(rm, LOG_NOTICE, L"WebView2: \"%s\" extension %s.", name.get(), enable ? L"enabled" : L"disabled");
									}

									break;
								}

								if (!found) // Extension not yet installed.
									addExtension(extensionPath, folderName);
							}

							if (extensionsChanged)
							{
								// Save Extensions.ini
								extensionsFile.SaveFile(extensionsPath.c_str());
								extensionsChanged = false;
							}

							return S_OK;
						}).Get());
			}
		}

		// Create and inject COM Host Object for Rainmeter API
		wil::com_ptr<HostObjectRmAPI> hostObject =
			Microsoft::WRL::Make<HostObjectRmAPI>(this, g_typeLib);

		VARIANT variant = {};
		hostObject.query_to<IDispatch>(&variant.pdispVal);
		variant.vt = VT_DISPATCH;
		CHECK_FAILURE(webView->AddHostObjectToScript(L"RainmeterAPI", &variant));
		VariantClear(&variant);

		// Add script to make RainmeterAPI available globally
		webView->AddScriptToExecuteOnDocumentCreated(
			L"window.RainmeterAPI = chrome.webview.hostObjects.sync.RainmeterAPI;",
			nullptr
		);

		webView3 = webView.try_query<ICoreWebView2_3>();
		if (webView3)
		{
			if (!hostPath.empty() && webView3)
			{
				webView3->SetVirtualHostNameToFolderMapping(
					hostName.c_str(), hostPath.c_str(), COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_ALLOW);
			}
		}
		// Frames
		wil::com_ptr<ICoreWebView2_4> webView4 = webView.try_query<ICoreWebView2_4>();
		if (webView4) {
			webView4->add_FrameCreated(
				Microsoft::WRL::Callback<ICoreWebView2FrameCreatedEventHandler>(
					[this, variant](ICoreWebView2* sender, ICoreWebView2FrameCreatedEventArgs* args) -> HRESULT
					{
						if (!isStopping) 
						{
							wil::com_ptr<ICoreWebView2Frame> frame;
							args->get_Frame(&frame);

							BOOL isDestroyed;
							frame->IsDestroyed(&isDestroyed);

							if (isDestroyed) return S_OK;

							RegisterFrames(this, frame.get(), 1);
						}
						return S_OK;
					}
				).Get(), nullptr
			);
		}

		// Inject frame ancestor to allow framing websites. (Requires http-server).
		webView->add_FrameNavigationStarting(
			Microsoft::WRL::Callback<ICoreWebView2NavigationStartingEventHandler>(
				[this](ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT
				{
					std::wstring nUri = NormalizeUri(currentUrl);

					wil::com_ptr<ICoreWebView2NavigationStartingEventArgs2> navigationStartArgs;
					if (SUCCEEDED(args->QueryInterface(IID_PPV_ARGS(&navigationStartArgs))))
					{
						navigationStartArgs->put_AdditionalAllowedFrameAncestors(nUri.c_str());
					}
					return S_OK;
				}
			).Get(), nullptr
		);

		// For Task Manager
		webView6 = webView.try_query<ICoreWebView2_6>();
		
		// Skin Context Menu
		wil::com_ptr<ICoreWebView2_11> webView11 = webView.try_query<ICoreWebView2_11>();
		if (webView11) {
			webView11->add_ContextMenuRequested(
				Microsoft::WRL::Callback<ICoreWebView2ContextMenuRequestedEventHandler>(
					[this](ICoreWebView2* sender, ICoreWebView2ContextMenuRequestedEventArgs* args) -> HRESULT
					{					
						// Show browser's context menu
						wil::com_ptr<ICoreWebView2ContextMenuItemCollection> items;
						args->get_MenuItems(&items);
						wil::com_ptr<ICoreWebView2ContextMenuTarget> target;
						args->get_ContextMenuTarget(&target);
						COREWEBVIEW2_CONTEXT_MENU_TARGET_KIND context_kind;
						target->get_Kind(&context_kind);
						UINT32 itemsCount;
						items->get_Count(&itemsCount);

						wil::unique_cotaskmem_string documentTile;
						webView->get_DocumentTitle(&documentTile);

						bool isViewSource = documentTile && wcsncmp(documentTile.get(), L"view-source:", 12) == 0;
						
						// Add Task Manager, Downloads and View Page Source items
						wil::com_ptr<ICoreWebView2Environment9> webviewEnvironment9 = webViewEnvironment.try_query<ICoreWebView2Environment9>();
						if (webviewEnvironment9 && webView6)
						{

							wil::com_ptr<ICoreWebView2ContextMenuItem> taskManagerItem;
							webviewEnvironment9->CreateContextMenuItem(L"Task manager", nullptr, COREWEBVIEW2_CONTEXT_MENU_ITEM_KIND_COMMAND, &taskManagerItem);

							taskManagerItem->add_CustomItemSelected( // Task Manager
								Callback<ICoreWebView2CustomItemSelectedEventHandler>(
									[this](
										ICoreWebView2ContextMenuItem* sender,
										IUnknown* args)
									{
										webView6->OpenTaskManagerWindow();
										return S_OK;
									})
								.Get(),
								nullptr);

							wil::com_ptr<ICoreWebView2ContextMenuItem> viewPageSource;

							if (!isViewSource)
							{
								webviewEnvironment9->CreateContextMenuItem(L"View page source", nullptr, COREWEBVIEW2_CONTEXT_MENU_ITEM_KIND_COMMAND, &viewPageSource);

								viewPageSource->add_CustomItemSelected( // View Page Source
									Callback<ICoreWebView2CustomItemSelectedEventHandler>(
										[this, target](
											ICoreWebView2ContextMenuItem* sender,
											IUnknown* args)
										{
											INPUT inputs[4] = {};

											inputs[0].type = INPUT_KEYBOARD;
											inputs[0].ki.wVk = VK_CONTROL;

											inputs[1].type = INPUT_KEYBOARD;
											inputs[1].ki.wVk = 'U';

											inputs[2].type = INPUT_KEYBOARD;
											inputs[2].ki.wVk = 'U';
											inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

											inputs[3].type = INPUT_KEYBOARD;
											inputs[3].ki.wVk = VK_CONTROL;
											inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

											SendInput(4, inputs, sizeof(INPUT));
											return S_OK;
										})
									.Get(),
									nullptr);
							}
							else
							{
								webviewEnvironment9->CreateContextMenuItem(L"Exit View Source", nullptr, COREWEBVIEW2_CONTEXT_MENU_ITEM_KIND_COMMAND, &viewPageSource);
								viewPageSource->add_CustomItemSelected( // View Page Source
									Callback<ICoreWebView2CustomItemSelectedEventHandler>(
										[this](
											ICoreWebView2ContextMenuItem* sender,
											IUnknown* args)
										{
											wil::unique_cotaskmem_string currentUrl;
											webView->get_Source(&currentUrl);
											webView->Navigate(currentUrl.get());
											return S_OK;
										})
									.Get(),
									nullptr);
							}

							wil::com_ptr<ICoreWebView2ContextMenuItem> downloads;
							webviewEnvironment9->CreateContextMenuItem(L"Downloads", nullptr, COREWEBVIEW2_CONTEXT_MENU_ITEM_KIND_COMMAND, &downloads);

							downloads->add_CustomItemSelected( // Downloads
								Callback<ICoreWebView2CustomItemSelectedEventHandler>(
									[this, target](
										ICoreWebView2ContextMenuItem* sender,
										IUnknown* args)
									{
										INPUT inputs[4] = {};

										inputs[0].type = INPUT_KEYBOARD;
										inputs[0].ki.wVk = VK_CONTROL;

										inputs[1].type = INPUT_KEYBOARD;
										inputs[1].ki.wVk = 'J';

										inputs[2].type = INPUT_KEYBOARD;
										inputs[2].ki.wVk = 'J';
										inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

										inputs[3].type = INPUT_KEYBOARD;
										inputs[3].ki.wVk = VK_CONTROL;
										inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

										SendInput(4, inputs, sizeof(INPUT));
										return S_OK;
									})
								.Get(),
								nullptr);

							wil::com_ptr<ICoreWebView2ContextMenuItem> separator;
							webviewEnvironment9->CreateContextMenuItem(L"View page source", nullptr, COREWEBVIEW2_CONTEXT_MENU_ITEM_KIND_SEPARATOR, &separator);

							wil::com_ptr<ICoreWebView2ContextMenuItem> skinMenu;
							webviewEnvironment9->CreateContextMenuItem(L"Skin menu", nullptr, COREWEBVIEW2_CONTEXT_MENU_ITEM_KIND_COMMAND, &skinMenu);

							skinMenu->add_CustomItemSelected( // Skin Menu
								Callback<ICoreWebView2CustomItemSelectedEventHandler>(
									[this, target](
										ICoreWebView2ContextMenuItem* sender,
										IUnknown* args)
									{
										RmExecute(skin, L"[!SkinMenu]");
										return S_OK;
									})
								.Get(),
								nullptr);

							items->InsertValueAtIndex(itemsCount, skinMenu.get());
							items->InsertValueAtIndex(itemsCount, separator.get());
							items->InsertValueAtIndex(itemsCount, taskManagerItem.get());
							items->InsertValueAtIndex(itemsCount, viewPageSource.get());
							items->InsertValueAtIndex(itemsCount - 1, downloads.get());
							items->InsertValueAtIndex(itemsCount, separator.get());
						}

						if (!newWindow)
						{
							// Remove Open link in new window item
							wil::com_ptr<ICoreWebView2ContextMenuItem> current;
							for (UINT32 i = 0; i < itemsCount; i++)
							{
								items->GetValueAtIndex(i, &current);
								wil::unique_cotaskmem_string name;
								current->get_Name(&name);

								if (wcscmp(name.get(), L"openLinkInNewWindow") == 0)
								{
									items->RemoveValueAtIndex(i);
									i--;
									itemsCount--;
									break;
								}
							}
						}

						if (!assistiveFeatures)
						{
							// Remove Find and Print items
							wil::com_ptr<ICoreWebView2ContextMenuItem> current;
							for (UINT32 i = 0; i < itemsCount; i++)
							{
								items->GetValueAtIndex(i, &current);
								wil::unique_cotaskmem_string name;
								current->get_Name(&name);
								if (wcscmp(name.get(), L"find") == 0 ||
									wcscmp(name.get(), L"print") == 0)
								{
									items->RemoveValueAtIndex(i);
									i--;
									itemsCount--;
								}
							}
						}

						return S_OK;
					}
				).Get(), nullptr
			);
		}

		// Avoid browser from opening links on different windows and block not user requested popups
		webView->add_NewWindowRequested(
			Callback<ICoreWebView2NewWindowRequestedEventHandler>(
				[this](ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs* args) -> HRESULT
				{
					BOOL isUserInitiated = FALSE;
					args->get_IsUserInitiated(&isUserInitiated);

					// Block scripted popup
					if (!isUserInitiated)
					{
						args->put_Handled(TRUE);
						return S_OK;
					}

					wil::unique_cotaskmem_string uri;
					args->get_Uri(&uri);

					bool isViewSource = uri && wcsncmp(uri.get(), L"view-source:", 12) == 0;
					std::wstring viewSourceUrl = uri.get() + 12;
					bool isVirtualHost = !viewSourceUrl.empty() && viewSourceUrl.find(hostName.c_str()) != std::wstring::npos;

					// if url is view-source and contains hostname, open in same window:
					if (isViewSource && isVirtualHost)
					{
						sender->Navigate(uri.get());
						args->put_Handled(TRUE);
						return S_OK;
					}
					else if (isViewSource) // if url is view-source only, open in new window:
					{
						args->put_Handled(FALSE);
						return S_OK;
					}

					if (newWindow)
					{
						// Open in new window
						args->put_Handled(FALSE);
					}
					else
					{
						// Open in same window
						sender->Navigate(uri.get());
						args->put_Handled(TRUE);
					}

					return S_OK;
				}
			).Get(), nullptr
		);

		// Handle permissions
		webView->add_PermissionRequested(
			Callback<ICoreWebView2PermissionRequestedEventHandler>(
				[this](ICoreWebView2* sender, ICoreWebView2PermissionRequestedEventArgs* args) -> HRESULT
				{
					COREWEBVIEW2_PERMISSION_KIND kind;
					args->get_PermissionKind(&kind);
					// Allow notifications
					if (kind == COREWEBVIEW2_PERMISSION_KIND_NOTIFICATIONS)
					{
						if (notifications) // Allow
						{
							args->put_State(COREWEBVIEW2_PERMISSION_STATE_ALLOW);
						}
						else // Deny
						{
							args->put_State(COREWEBVIEW2_PERMISSION_STATE_DENY);
						}
					}

					wil::com_ptr<ICoreWebView2PermissionRequestedEventArgs3> args3;
					if (SUCCEEDED(args->QueryInterface(IID_PPV_ARGS(&args3))))
					{
						args3->put_SavesInProfile(FALSE);
					}
					return S_OK;
				}
			).Get(), nullptr
		);

		// Add NavigationStarting event to call action when navigation starts
		webView->add_NavigationStarting(
			Callback<ICoreWebView2NavigationStartingEventHandler>(
				[this](ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT
				{
					wil::unique_cotaskmem_string initialUri;
					sender->get_Source(&initialUri);
					wil::unique_cotaskmem_string destinationUri;
					args->get_Uri(&destinationUri);

					// Determine if this is the first load or a reload
					if (initialUri && destinationUri &&
						wcscmp(initialUri.get(), destinationUri.get()) == 0) // if same URL, it's a reload
					{
						isFirstLoad = false;
					}
					else // different URL, it's a first load
					{
						isFirstLoad = true;
					}

					// Navigation is starting
					SetStateAndNotify(100);
					if (wcslen(onPageLoadStartAction.c_str()) > 0)
					{
						if (skin)
							RmExecute(skin, onPageLoadStartAction.c_str());
					}
					return S_OK;
				}
			).Get(), nullptr
		);

		// Add SourceChanged event to detect changes in URL
		webView->add_SourceChanged(
			Callback<ICoreWebView2SourceChangedEventHandler>(
				[this](ICoreWebView2* sender, ICoreWebView2SourceChangedEventArgs* args) -> HRESULT
				{
					wil::unique_cotaskmem_string updatedUri;

					if (SUCCEEDED(sender->get_Source(&updatedUri)) && updatedUri.get() != nullptr)
					{
						std::wstring newUrl = updatedUri.get();

						if (currentUrl != newUrl)
						{
							currentUrl = newUrl;
							if (wcslen(onUrlChangeAction.c_str()) > 0)
							{
								if (skin) RmExecute(skin, onUrlChangeAction.c_str());
							}
						}
					}

					return S_OK;
				}
			).Get(), nullptr
		);

		webView->add_DocumentTitleChanged(
			Callback<ICoreWebView2DocumentTitleChangedEventHandler>(
				[this](ICoreWebView2* sender, IUnknown* args) -> HRESULT
				{
					
					// Read current Url
					wil::unique_cotaskmem_string uri;
					sender->get_Source(&uri);

					// Read document title.
					wil::unique_cotaskmem_string documentTile;
					sender->get_DocumentTitle(&documentTile);
					currentTitle = documentTile.get();

					bool isViewSource = documentTile && wcsncmp(documentTile.get(), L"view-source:", 12) == 0;
					// Look for view-source on document title:
					if (isViewSource)
					{
						currentUrl = documentTile.get();

						if (wcslen(onUrlChangeAction.c_str()) > 0)
						{
							if (skin) RmExecute(skin, onUrlChangeAction.c_str());
						}
					}
					else
					{
						// Look for view-source on current url and remove it:
						if (wcsncmp(currentUrl.c_str(), L"view-source:", 12) == 0)
						{
							currentUrl = uri.get();
							if (wcslen(onUrlChangeAction.c_str()) > 0)
							{
								if (skin) RmExecute(skin, onUrlChangeAction.c_str());
							}
						}
					}


					return S_OK;
				}
			).Get(), nullptr
		);

		// Add ContentLoading event to call action when page starts loading
		webView->add_ContentLoading(
			Callback<ICoreWebView2ContentLoadingEventHandler>(
				[this](ICoreWebView2* sender, ICoreWebView2ContentLoadingEventArgs* args) -> HRESULT
				{
					// Navigation is loading
					SetStateAndNotify(200);

					if (wcslen(onPageLoadingAction.c_str()) > 0)
					{
						if (skin)
							RmExecute(skin, onPageLoadingAction.c_str());
					}
					return S_OK;
				}
			).Get(), nullptr
		);

		// Add DOMContentLoaded event to call action when DOM is loaded
		wil::com_ptr<ICoreWebView2_2> webView2 = webView.try_query<ICoreWebView2_2>();
		if (webView2) {

			webView2->add_DOMContentLoaded(
				Callback< ICoreWebView2DOMContentLoadedEventHandler>(
					[this](ICoreWebView2* sender, ICoreWebView2DOMContentLoadedEventArgs* args) -> HRESULT
					{
						// DOM content is loaded
						SetStateAndNotify(300);

						if (wcslen(onPageDOMLoadAction.c_str()) > 0)
						{
							if (skin)
								RmExecute(skin, onPageDOMLoadAction.c_str());
						}

						return S_OK;
					}
				).Get(), nullptr
			);
		}

		// Add NavigationCompleted event to call OnInitialize after page loads and handle load actions
		webView->add_NavigationCompleted(
			Callback<ICoreWebView2NavigationCompletedEventHandler>(
				[this](ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT
				{
					// Navigation is complete
					SetStateAndNotify(400);

					// Call JavaScript OnInitialize callback if it exists and capture return value
					webView->ExecuteScript(
						L"(function() { if (typeof window.OnInitialize === 'function') { var result = window.OnInitialize(); return result !== undefined ? String(result) : ''; } return ''; })();",
						Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
							[this](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT
							{
								return S_OK;
							}
						).Get()
					);

					if (isFirstLoad) // First load
					{
						if (wcslen(onPageFirstLoadAction.c_str()) > 0)
						{
							if (skin)
								RmExecute(skin, onPageFirstLoadAction.c_str());
						}
						isFirstLoad = false;
					}
					else // Page reload
					{
						if (wcslen(onPageReloadAction.c_str()) > 0)
						{
							if (skin)
								RmExecute(skin, onPageReloadAction.c_str());
						}
					}
					// Common action after any page load
					if (wcslen(onPageLoadFinishAction.c_str()) > 0)
					{
						if (skin)
							RmExecute(skin, onPageLoadFinishAction.c_str());
					}
					return S_OK;
				}
			).Get(), nullptr
		);

		if (userSettingsChanged)
		{
			userSettingsFile.SaveFile(configPath.c_str());
			userSettingsChanged = false;
		}

		initialized = true;

		//if (rm) RmLog(rm, LOG_DEBUG, L"WebView2: Initialized successfully with COM Host Objects");

		// WebView is initialized
			SetStateAndNotify(1);

		if (wcslen(onWebViewLoadAction.c_str()) > 0)
		{
			RmExecute(skin, onWebViewLoadAction.c_str());
		}

		isCreationInProgress = false;

		// Navigate to URL
		webView->Navigate(url.c_str());

		// Apply initial SkinControl state
		UpdateChildWindowState(this, ((clickthrough == 1 || clickthrough >= 3) ? false : true));
	}
	return S_OK;
}

void Measure::SetStateAndNotify(int newState)
{
	state = newState;

	if (!onStateChangeAction.empty() && skin)
	{
		RmExecute(skin, onStateChangeAction.c_str());
	}
}

HRESULT Measure::FailWebView(HRESULT hr, const wchar_t* logMessage, bool resetCreationFlag)
{
	if (rm && logMessage)
	{
		wchar_t errorMsg[512];
		if (hr != S_OK)
		{
			swprintf_s(errorMsg, L"%s (HRESULT: 0x%08X)", logMessage, hr);
			RmLog(rm, LOG_ERROR, errorMsg);
		}
		else
		{
			RmLog(rm, LOG_ERROR, logMessage);
		}
	}

	SetStateAndNotify(-2);

	if (!onWebViewFailAction.empty() && skin)
	{
		RmExecute(skin, onWebViewFailAction.c_str());
	}

	if (resetCreationFlag)
	{
		isCreationInProgress = false;
	}

	return hr;
}

void RestartWebView2(Measure* measure)
{
	if (!measure || !measure->skinWindow)
		return;

	if (!measure->initialized)
	{
		RmLog(measure->rm, LOG_ERROR, L"WebView2: Not running");
		return;
	}

	// Stop WebView2
	StopWebView2(measure);

	// Start WebView2
	if (!measure->isStopping)
	{
		CreateWebView2(measure);
	}
}

void StopWebView2(Measure* measure)
{
	if (!measure)
		return;
	if (measure->isStopping)
		return;
	if (!measure->initialized && !measure->webView && !measure->webViewController)
	{
		RmLog(measure->rm, LOG_ERROR, L"WebView2: Already stopped");
		return;
	}

	measure->isStopping = true;

	measure->isCreationInProgress = false;

	// Stop navigation
	if (measure->webView)
	{
		measure->webView->Stop();
	}

	// Close the Controller 
	if (measure->webViewController)
	{
		measure->webViewController->put_IsVisible(FALSE);
		measure->webViewController->Close();
	}

	// Release
	measure->webViewController.reset();
	measure->webView.reset();
	measure->webViewSettings.reset();
	measure->webViewSettings2.reset();
	measure->webViewEnvironment.reset();

	// Reset flags
	measure->initialized = false;
	measure->isFirstLoad = true;
	measure->isViewSource = false;

	// Clear url
	measure->currentUrl.clear();

	// WebView is stopped
	measure->SetStateAndNotify(-1);

	if (wcslen(measure->onWebViewStopAction.c_str()) > 0)
	{
		if (measure->skin)
			RmExecute(measure->skin, measure->onWebViewStopAction.c_str());
	}

	measure->isStopping = false;
}