# WebView2 Plugin for Rainmeter

<div align="center">

![WebView2 Plugin Banner](https://img.shields.io/badge/WebView2-Rainmeter_Plugin-0078D4?style=for-the-badge&logo=microsoft-edge&logoColor=white)

**Embed modern web content directly into your Rainmeter skins with full JavaScript interactivity**

[![Version](https://img.shields.io/badge/version-0.0.3-blue?style=flat-square)](../../releases)
[![License](https://img.shields.io/badge/license-MIT-green?style=flat-square)](LICENSE)
[![Rainmeter](https://img.shields.io/badge/rainmeter-4.5+-orange?style=flat-square)](https://www.rainmeter.net/)
[![Windows](https://img.shields.io/badge/windows-10%2B-0078D6?style=flat-square&logo=windows&logoColor=white)](https://www.microsoft.com/windows)

[📥 Download](#-installation) • [📖 Documentation](#-quick-start) • [💡 Examples](#-examples) • [🤝 Contributing](#-contributing)

</div>

---

## 🚀 Quick Navigation

<div align="center">
<table>
  <tr>
    <td>🚀 <a href="#-what-can-you-build">What Can You Build?</a></td>
    <td>🚀 <a href="#-quick-start">Quick Start</a></td>
    <td>❓ <a href="#-how-to-make-a-draggable-skin">Draggable WebView2 Skin</a></td>
    <td>🆘 <a href="#-troubleshooting">Troubleshooting</a></td>
  </tr>
  <tr>
    <td>🎯 <a href="#-key-features">Key Features</a></td>
    <td>⚙️ <a href="#️-measure">Measure</a></td>
    <td>☄️ <a href="#️-setting-up-a-virtual-host">Virtual Host Setup</a></td>
    <td>📄 <a href="#-license">License</a></td>
  </tr>
  <tr>
    <td>📋 <a href="#-requirements">Requirements</a></td>
    <td>🔥 <a href="#-javascript-integration">JavaScript Integration</a></td>
    <td>💡 <a href="#-examples">Examples</a></td>
    <td>🤝 <a href="#-contributing">Contributing</a></td>
  </tr>
  <tr>
    <td>📥 <a href="#-installation">Installation</a></td>
    <td>🌉 <a href="#-rainmeterapi-bridge">RainmeterAPI Bridge</a></td>
    <td>🛠️ <a href="#️-building-from-source">Building from Source</a></td>
    <td>🙏 <a href="#-acknowledgments">Acknowledgments</a></td>
  </tr>
</table>
</div>

---

## ✨ What Can You Build?

<table>
<tr>
<td width="33%" align="center">
<img src="https://img.icons8.com/fluency/96/000000/clock.png" width="64" alt="Clock"/><br/>
<b>Animated Widgets</b><br/>
<sub>Create stunning animated clocks, weather displays, and visualizers</sub>
</td>
<td width="33%" align="center">
<img src="https://img.icons8.com/fluency/96/000000/web.png" width="64" alt="Web"/><br/>
<b>Web Dashboards</b><br/>
<sub>Embed live web content and interactive dashboards</sub>
</td>
<td width="33%" align="center">
<img src="https://img.icons8.com/fluency/96/000000/api.png" width="64" alt="API"/><br/>
<b>Smart Integrations</b><br/>
<sub>Connect to APIs and control Rainmeter with JavaScript</sub>
</td>
</tr>
</table>

---

## 🎯 Key Features

<details open>
<summary><b>🚀 Modern Web Engine</b></summary>

Powered by Microsoft Edge WebView2, supporting:
- ✅ HTML5, CSS3, JavaScript ES6+
- ✅ Modern frameworks (React, Vue, Svelte)
- ✅ WebGL, Canvas, SVG animations
- ✅ Transparent backgrounds by default

</details>

<details open>
<summary><b>🔌 Seamless JavaScript Bridge</b></summary>

Two-way communication between web and Rainmeter:
- ✅ Call Rainmeter API from JavaScript
- ✅ Execute JavaScript from Rainmeter
- ✅ Real-time data synchronization
- ✅ Custom events and callbacks

</details>

<details open>
<summary><b>⚡ Dynamic & Flexible</b></summary>

- ✅ Load local HTML or remote URLs
- ✅ Multiple WebView instances per skin
- ✅ Hot-reload without flickering
- ✅ Developer tools (F12) built-in

</details>

---

## 📋 Requirements

> **Before you begin**, make sure you have:

| Requirement | Version | Status |
|------------|---------|---------|
| **Windows** | 10 (1803+) or 11 | ![Windows](https://img.shields.io/badge/required-critical-red?style=flat-square) |
| **Rainmeter** | 4.5 or higher | ![Rainmeter](https://img.shields.io/badge/required-critical-red?style=flat-square) |
| **WebView2 Runtime** | Latest | ![WebView2](https://img.shields.io/badge/required-critical-red?style=flat-square) |

<details>
<summary>📦 <b>Don't have WebView2 Runtime?</b></summary>

<br/>

**Good news!** Windows 11 includes it by default. For Windows 10:

1. 🔗 [Download WebView2 Runtime](https://developer.microsoft.com/en-us/microsoft-edge/webview2/)
2. 🎯 Choose "Evergreen Standalone Installer"
3. ⚡ Run the installer (takes ~1 minute)

</details>

---

## 📥 Installation

### 🎁 Method 1: One-Click Install (Recommended)

**The easiest way to get started!**

1. 📦 [Download the .rmskin file](../../releases/latest)
2. 🖱️ Double-click to install
3. ✨ Done! Plugin and examples are ready to use

<sub>Rainmeter will automatically install everything you need</sub>

### 🛠️ Method 2: Manual Installation

<details>
<summary>Click to expand manual installation steps</summary>

<br/>

1. **Download** the plugin DLLs from [Releases](../../releases)

2. **Choose** the right version:
   ```
   📁 x64/WebView2.dll  ← For 64-bit Rainmeter (most common)
   📁 x32/WebView2.dll  ← For 32-bit Rainmeter
   ```

3. **Copy** to your Rainmeter plugins folder:
   ```
   %AppData%\Rainmeter\Plugins\
   ```

4. **Restart** Rainmeter

</details>

---

## 🚀 Quick Start

### Your First WebView Skin

Create a new skin with this minimal configuration:

<details>
<summary><b>skin.ini</b></summary>

```ini
[Rainmeter]
Update=1000

[WebView2]
Measure=Plugin
Plugin=WebView2
URL=file:///#@#index.html
W=800
H=600
X=0
Y=0

[Background]
Meter=Image
W=800
H=600
x=0
Y=0
SolidColor=0,0,0,1
```
</details>

Create `index.html` in your `@Resources` folder:

<details>
<summary><b>index.html</b></summary>

```html
<!DOCTYPE html>
<html>
	<head>
		<style>
			body {
				background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
				color: white;
				font-family: 'Segoe UI', sans-serif;
				display: flex;
				flex-direction: column;
				justify-content: center;
				align-items: center;
				height: 100vh;
				width: 100vw;
				margin: 0;
			}
			h1, p { animation: fadeIn 1s; }
			h1 { font-size: 2.8em; user-select: none;}
			p { font-size: 1.3em;}
			p + p { margin-top: 0.1em; }
			@keyframes fadeIn { from { opacity: 0; } to { opacity: 1; } }
		</style>
	</head>
	<body>
		<h1>🎉 Hello Rainmeter!</h1>
		<p>Hold CTRL to drag the skin</p>
		<p>CTRL + Right Click to open the Skin Menu</p>
	</body>
</html>
```
</details>

**That's it!** Load the skin and see your first WebView in action.

---

## ⚙️ Measure

<details>
<summary><b>👑 Measure Values</b></summary>

### 👑 Measure Values

WebView's measure returns the following values:

---

**Number Value**

The number value represents the internal state of the WebView2 instance while it's initializing. When the WebView2 instance is initialized, the number value represent's the navigation state.
* `-2` WebView failed during initialization.
* `-1` WebView2 is idle.
* `0`  WebView2 is initializing.
* `1`  WebView2 is initialized.
* `100` Navigation has started.
* `200` Web content is loading.
* `300` Web is loaded.
* `400` Navigation has finished.

Whenever the state changes, the plugin triggers `OnStateChangeAction`.

---

**String Value**

The string value represents the current URL. This URL will change when the user navigates through WebView either internally by clicking on links or externally by using commands.
* `CurrentURL`

Whenever the URL changes, the plugin triggers `OnURLChangeAction`.

</details>

<details>
<summary><b>🛠️ Measure Options</b></summary>

### 🛠️ Measure Options


<table>
<thead>
<tr>
<th scope="col">Option</th>
<th scope="col">Description</th>
<th scope="col">Default</th>
<th scope="col">Examples</th>
</tr>
</thead>

<tbody>

<!-- WebView Options -->

<tr>
<th colspan="4" align="center">WebView Options</th>
</tr>
<tr>

<tr>
<th scope="row"><code>AutoStart</code></th>
<td>
Automatically load WebView on skin load.<br />
<code>0</code> = Disabled,
<code>1</code> = Enabled
</td>
<th><code>1</code></th>
<td><code>AutoStart=0</code></td>
</tr>

<tr>
<th scope="row"><code>URL</code></th>
<td>
The URL WebView will navigate to when it starts.<br />
This is the URL the <code>Navigate Home</code> command navigates to.<br />
Supports: file paths and web URLs<br />
Supported schemes: <code>file:///</code>, <code>http://</code>, <code>https://</code>, <code>view-source:</code><br />
Relative paths are also supported: <code>URL=file.html</code><br />
Paths are relative to <code>#CURRENTPATH#</code>.<br />
When <code>HostPath</code> is set to a valid folder path, this option is relative to that folder and will navigate to the default virtual host: <code>https://rootconfig/file.html</code><br />
See <a href="#setting-up-a-virtual-host">Setting Up a Virtual Host</a>.
</td>
<th><em>Required</em></th>
<td>
<code>URL=file:///#@#file.html</code><br />
<code>URL=http://example.com</code><br />
<code>URL=path\to\file.html</code><br />
<code>URL=path\to\img.gif</code><br />
<code>URL=file.html</code>
</td>
</tr>

<!-- Virtual Host Options -->

<tr>
<th colspan="4" align="center">Virtual Host Options</th>
</tr>
<tr>

<tr>
<th scope="row"><code>HostSecurity</code></th>
<td>
Set a preferred protocol for the virtual host:<br />
<code>0</code> = http (not secure), <code>1</code> = https (secure)<br />
Https allows to use JavaScript APIs that are normally blocked by CORS.<br />
See <a href="#setting-up-a-virtual-host">Setting Up a Virtual Host</a>.
</td>
<th><code>1</code></th>
<td><code>HostSecurity=0</code></td>
</tr>

<tr>
<th scope="row"><code>HostOrigin</code></th>
<td>
Set a preferred host name for the virtual host:<br />
<code>0</code> = current-config (not shared), <code>1</code> = rootconfig (shared)<br />
When current-config <code>0</code> is used, the local storage is isolated to the current skin's origin, eg. <code>https://illustro-clock</code><br />
When rootconfig <code>1</code> is used, the local storage is shared between all configs that are part of the same suite, eg. <code>https://illustro</code><br />
See <a href="#setting-up-a-virtual-host">Setting Up a Virtual Host</a>.
</td>
<th><code>1</code></th>
<td><code>HostOrigin=0</code></td>
</tr>

<tr>
<th scope="row"><code>HostPath</code></th>
<td>
The path to the folder that contains the <code>index.html</code> that the virtual host will load.<br />
When this option is set to a folder path, the <code>URL</code> option will become relative to the path set on this option.<br />
See <a href="#setting-up-a-virtual-host">Setting Up a Virtual Host</a>.
</td>
<th><code>""</code></th>
<td>
<code>HostPath=path\to\folder</code><br />
<code>HostPath=#CURRENTPATH#</code><br />
<code>HostPath=#@#</code>
</td>
</tr>

<!-- Window Options -->

<tr>
<th colspan="4" align="center">Window Options</th>
</tr>
<tr>

<tr>
<th scope="row"><code>W</code></th>
<td>Window width (pixels)</td>
<th><code>800</code></th>
<td><code>W=1920</code></td>
</tr>

<tr>
<th scope="row"><code>H</code></th>
<td>Window height (pixels)</td>
<th><code>600</code></th>
<td><code>H=1080</code></td>
</tr>

<tr>
<th scope="row"><code>X</code></th>
<td>Horizontal position offset</td>
<th><code>0</code></th>
<td><code>X=100</code></td>
</tr>

<tr>
<th scope="row"><code>Y</code></th>
<td>Vertical position offset</td>
<th><code>0</code></th>
<td><code>Y=50</code></td>
</tr>

<tr>
<th scope="row"><code>ZoomFactor</code></th>
<td>Site zoom factor</td>
<th><code>1.0</code></th>
<td><code>ZoomFactor=1.5</code></td>
</tr>

<tr>
<th scope="row"><code>Hidden</code></th>
<td>
Window visibility<br />
<small>
<code>0</code> = Visible,
<code>1</code> = Hidden
</small>
</td>
<th><code>0</code></th>
<td><code>Hidden=1</code></td>
</tr>

<tr>
<th scope="row"><code>Clickthrough</code></th>
<td>
Mouse interaction mode<br />
<small>
<code>0</code> = Interactive,
<code>1</code> = Click Through,
<code>2</code> = Press <code>CTRL</code> to toggle<br />
This option is very usefull to drag the skin, while set to <code>2</code> press <code>CTRL</code> to drag the skin and <code>RMB</code> to open the Skin Menu.
</small>
</td>
<th><code>2</code></th>
<td><code>Clickthrough=1</code></td>
</tr>

<!-- Browser Options -->

<tr>
<th colspan="4" align="center">Browser Options</th>
</tr>
<tr>

<tr>
<th scope="row"><code>UserAgent</code></th>
<td>
Custom user-agent string override
</td>
<th><code>""</code></th>
<td><code>UserAgent=MyBrowser/1.0</code></td>
</tr>

<tr>
<th scope="row"><code>ZoomControl</code></th>
<td>
Allow user-controlled zoom through:<br />
<code>CTRL+SCROLL</code>, <code>CTRL + PLUS</code>, <code>CTRL + MINUS</code> and <code>CTRL + 0</code><br />
<code>0</code> = Disabled,
<code>1</code> = Enabled
</td>
<th><code>1</code></th>
<td><code>ZoomControl=0</code></td>
</tr>

<tr>
<th scope="row"><code>NewWindow</code></th>
<td>
Allow opening links in a new window<br />
<code>0</code> = Disabled,
<code>1</code> = Enabled
</td>
<th><code>0</code></th>
<td><code>NewWindow=1</code></td>
</tr>

<tr>
<th scope="row"><code>Notifications</code></th>
<td>
Override JavaScript's Notification API permission.<br />
<code>0</code> = Deny,
<code>1</code> = Allow
</td>
<th><code>0</code></th>
<td><code>Notifications=1</code></td>
</tr>

<tr>
<th scope="row"><code>AssistiveFeatures</code></th>
<td>
Allow <code>Print</code>, <code>Find</code> and <code>Caret Browsing</code> features.<br />
<code>0</code> = Disabled,
<code>1</code> = Enabled
</td>
<th><code>1</code></th>
<td><code>AssistiveFeatures=0</code></td>
</tr>
</tbody>
</table>

> **💡 Pro Tip:** When `DynamicVariables=1`, the WebView updates smartly:
> - **URL changes** → Sets a new Home Page
> - **Size/Position changes** → Applied instantly, no flicker
> - **Visibility changes** → Instant toggle
> - All options can be updated dynamically

</details>

<details>
<summary><b>▶️ Measure Actions</b></summary>

### ▶️ Measure Actions


<table>
<thead>
<tr>
<th scope="col">Action</th>
<th scope="col">Description</th>
<th scope="col">Example</th>
</tr>
</thead>

<!-- WebView State Actions -->

<tr>
<th colspan="3" align="center">WebView State Actions</th>
</tr>
<tr>

<tr>
<th scope="row"><code>OnWebViewLoadAction</code></th>
<td>
   Triggers when WebView starts.
</td>
<td><code>OnWebViewLoadActio=[!log "WebView2 loaded succesfully!"]</code></td>
</tr>

<tr>
<th scope="row"><code>OnWebViewFailAction</code></th>
<td>
Triggers when WebView fails.
</td>
<td><code>OnWebViewFailAction=[!log "WebView2 failed :("]</code></td>
</tr>

<tr>
<th scope="row"><code>OnWebViewStopAction</code></th>
<td>
Triggers when WebView stops.
</td>
<td><code>OnWebViewStopAction=[!log "WebView2 has stopped!"]</code></td>
</tr>

<tr>
<th scope="row"><code>OnStateChangeAction</code></th>
<td>
Triggers when WebView initialization or navigation states change.
</td>
<td><code>OnStateChangeAction=[!UpdateMeasure #CURRENTSECTION#][!UpdateMeter CurrentState][!Redraw]</code></td>
</tr>

<!-- Navigation Actions -->

<tr>
<th colspan="3" align="center">Navigation Actions</th>
</tr>
<tr>

<tr>
<th scope="row"><code>OnUrlChangeAction</code></th>
<td>
Triggers when the current URL changes.
</td>
<td><code>OnUrlChangeAction=[!UpdateMeasure #CURRENTSECTION#][!UpdateMeter CurrentURL][!Redraw]</code></td>
</tr>

<tr>
<th scope="row"><code>OnPageLoadStartAction</code></th>
<td>
Triggers when navigation starts.
</td>
<td><code>OnPageLoadStartAction=[!log "Navigation has started!"]</code></td>
</tr>

<tr>
<th scope="row"><code>OnPageLoadingAction</code></th>
<td>
Triggers when the page starts loading.
</td>
<td><code>OnPageLoadingAction=[!log "Page is loading!"]</code></td>
</tr>

<tr>
<th scope="row"><code>OnPageDOMLoadAction</code></th>
<td>
Triggers when the DOM content is loaded.
</td>
<td><code>OnPageDOMLoadAction=[!log "DOM content loaded!"]</code></td>
</tr>

<tr>
<th scope="row"><code>OnPageFirstLoadAction</code></th>
<td>
Triggers the first time a page is loaded.
</td>
<td><code>OnPageFirstLoadAction=[!log "First time on this page!"]</code></td>
</tr>

<tr>
<th scope="row"><code>OnPageReloadAction</code></th>
<td>
Triggers when the page is reloaded.
</td>
<td><code>OnPageReloadAction=[!log "Page has been reloaded!"]</code></td>
</tr>

<tr>
<th scope="row"><code>OnPageLoadFinishAction</code></th>
<td>
Triggers when the navigation is finished.
</td>
<td><code>OnPageLoadFinishAction=[!log "Navigation has finished!"]</code></td>
</tr>
</tbody>
</table>

</details>

<details>
<summary><b>💥 Bang Commands</b></summary>

### 💥 Bang Commands

Control your WebView with Rainmeter bangs:

<table>
<thead>
<tr>
<th>Command</th>
<th>Description</th>
<th>Example</th>
</tr>
</thead>
<tbody>

<!-- WebView -->
<tr>
<th colspan="3" align="center">WebView</th>
</tr>
<tr>

<th><code>WebView Start</code></th>
<td>Starts the WebView instance.</td>
<td><code>[!CommandMeasure WebView2 "WebView Start"]</code></td>
</tr>
<tr>

<th><code>WebView Stop</code></th>
<td>Stops the WebView instance.</td>
<td><code>[!CommandMeasure WebView2 "WebView Stop"]</code></td>
</tr>
<tr>

<th><code>WebView Restart</code></th>
<td>Restarts the WebView instance.</td>
<td><code>[!CommandMeasure WebView2 "WebView Restart"]</code></td>
</tr>

<!-- Navigate -->
<tr>
<th colspan="3" align="center">Navigate</th>
</tr>
<tr>

<th><code>Navigate Home</code></th>
<td>Navigates to <code>URL</code> option.</td>
<td><code>[!CommandMeasure WebView2 "Navigate Home"]</code></td>
</tr>
<tr>

<th><code>Navigate Back</code></th>
<td>Navigates to the previous page.</td>
<td><code>[!CommandMeasure WebView2 "Navigate Back"]</code></td>
</tr>
<tr>

<th><code>Navigate Forward</code></th>
<td>Navigates to the next page.</td>
<td><code>[!CommandMeasure WebView2 "Navigate Forward"]</code></td>
</tr>
<tr>

<th><code>Navigate Reload</code></th>
<td>Reloads the current page.</td>
<td><code>[!CommandMeasure WebView2 "Navigate Reload"]</code></td>
</tr>
<tr>

<th><code>Navigate Stop</code></th>
<td>Stops any navigation.</td>
<td><code>[!CommandMeasure WebView2 "Navigate Stop"]</code></td>
</tr>
<tr>

<th><code>Navigate URL</code></th>
<td>Navigates to a URL.</td>
<td><code>[!CommandMeasure WebView2 "Navigate http://example.com"]</code></td>
</tr>

<!-- Open -->
<tr>
<th colspan="3" align="center">Open</th>
</tr>
<tr>

<th><code>Open DevTools</code></th>
<td>Opens DeveloperTools.</td>
<td><code>[!CommandMeasure WebView2 "Open DevTools"]</code></td>
</tr>

<tr>
<th><code>Open TaskManager</code></th>
<td>Opens the web task manager.</td>
<td><code>[!CommandMeasure WebView2 "Open TaskManager"]</code></td>
</tr>

<!-- Execute -->
<tr>
<th colspan="3" align="center">Execute</th>
</tr>
<tr>
<th>
<code>Execute Script</code><br />
<code>Execute File.js</code><br />
</th>
<td>Executes given JS script or .js script file</td>
<td>
<code>[!CommandMeasure WebView2 "Execute alert('Hello Rainmeter!')"]</code> <br />
<code>[!CommandMeasure WebView2 "Execute #@#script.js"]</code>
</td>
</tr>

</tbody>
</table>

</details>


<details>
<summary><b>🗺️ Defaults Map</b></summary>

### 🗺️ Defaults Map


```ini
[WebView2]
Measure=Plugin
Plugin=WebView2

; WebView Options
AutoStart=1
URL=""

; Virtual Host Options
HostSecurity=1
HostOrigin=1
HostPath=""

; Window Options
W=800
H=600
X=0
Y=0
ZoomFactor=1.0
Hidden=0
Clickthrough=2

;Browser Options
UserAgent=""
ZoomControl=1
NewWindow=0
Notifications=0
AssistiveFeatures=1

; WebView State Actions
OnWebViewLoadAction=[]
OnWebViewFailAction=[]
OnWebViewStopAction=[]

; Navigation State Actions
OnStateChangeAction=[]
OnUrlChangeAction=[]
OnPageLoadStartAction=[]
OnPageLoadingAction=[]
OnPageDOMLoadAction=[]
OnPageFirstLoadAction=[]
OnPageReloadAction=[]
OnPageLoadFinishAction=[]

---

; WebView Commands
[!CommandMeasure WebView2 "WebView Start"]
[!CommandMeasure WebView2 "WebView Stop"]
[!CommandMeasure WebView2 "WebView Restart"]

; Navigation Commands
[!CommandMeasure WebView2 "Navigate Home"]
[!CommandMeasure WebView2 "Navigate Back"]
[!CommandMeasure WebView2 "Navigate Forward"]
[!CommandMeasure WebView2 "Navigate Reload"]
[!CommandMeasure WebView2 "Navigate Stop"]
[!CommandMeasure WebView2 "Navigate http://www.example.com"]

; Open Commands
[!CommandMeasure WebView2 "Open DevTools"]
[!CommandMeasure WebView2 "Open TaskManager"]

; Execute Commands
[!CommandMeasure WebView2 "Execute alert('Hello Rainmeter!')"]
[!CommandMeasure WebView2 "Execute path\to\file.js"]

;Section Variables
[WebView2:CallJS('alert("Example script")')]

;User Data Folder Path
C:\Users\User\AppData\Local\Temp\RainmeterWebView2\

```
</details>

<details>
<summary><b>📂 User Data Folder</b></summary>

### User Data Folder

```
📁 RainmeterWebView2\
  ├── 📁 EBWebView\
  │   └── 📁 [WebView2 Data]\
  ├── 📁 Extensions\
  │   └── Extensions.ini
  │   └── 📁 MyExtension\
  │   └── 📁 MyOtherExtension\
  └── UserSettings.ini
```

When the plugin loads for the first time, a user data folder (UDP) is created. This folder contains all the data related to WebView2.

*Path: `C:\Users\User\AppData\Local\Temp\RainmeterWebView2\`*

If deleted, the folder will be re-created the next time the plugin is loaded.

To delete all your WebView2 data, it is recommended to delete `RainmeterWebView2\EBWebView\` instead. This way you preserve your [User Settings File](#user-settings-file) and [Extensions](#extensions).

> **💡 IMPORTANT:**
> - Exit Rainmeter before modifying anything in this folder, failing to do so will make WebView2 instances fail to start.
> - Restart Rainmeter if this happens.


### User Settings File

There are certain settings that affect all WebView2 instances, for this reason they can't be exposed through the measure's options.

Such settings can be found in a file called `UserSettings.ini`.

*Path: `C:\Users\User\AppData\Local\Temp\RainmeterWebView2\UserSettings.ini`*

<details>
<summary><b>Options</b></summary>

<table>
<thead>
<tr>
<th>Option</th>
<th>Description</th>
<th>Default</th>
</tr>
</thead>
<tbody>

<!-- Environment -->
<tr>
<th colspan="3" align="center">Environment Options</th>
</tr>

<tr>
<th><code>Extensions</code></th>
<td>Allows to use <a href="#extensions">extensions</a>.</td>
<th><code>false</code></th>
</tr>

<tr>
<th><code>FluentOverlayScrollBars</code></th>
<td>Enable Fluent Overlay scrollbars.</td>
<th><code>true</code></th>
</tr>

<tr>
<th><code>TrackingPrevention</code></th>
<td>Enables Microsoft Edge tracking prevention.</td>
<th><code>true</code></th>
</tr>

<tr>
<th><code>BrowserArguments</code></th>
<td>Additional command-line arguments passed to the WebView2 browser process.</td>
<th><code>--allow-file-access-from-files</code></th>
</tr>

<tr>
<th><code>BrowserLocale</code></th>
<td>Sets the browser UI locale. Use <code>system</code> to follow the OS language.<br/>
Locales need to be in the format of BCP 47 Language Tags. e.g:<br/>
<code>en-US</code> <code>es-MX</code> <code>fr-FR</code><br/>
A list of locales can be found <a href="https://appmakers.substack.com/p/bcp-47-language-codes-list"> here </a> 
</td>
<th><code>system</code></th>
</tr>

<!-- Controller -->
<tr>
<th colspan="3" align="center">Controller Options</th>
</tr>

<tr>
<th><code>PrivateMode</code></th>
<td>Enables private mode, also commonly called incognito mode.</td>
<th><code>false</code></th>
</tr>

<tr>
<th><code>ScriptLocale</code></th>
<td>
Locale used for JavaScript APIs such as date, time, and number formatting.<br/>
Use <code>system</code> to follow the OS language.<br/>
Locales need to be in the format of BCP 47 Language Tags. e.g:<br/>
<code>en-US</code> <code>es-MX</code> <code>fr-FR</code><br/>
A list of locales can be found <a href="https://appmakers.substack.com/p/bcp-47-language-codes-list"> here </a> 
</td>
<th><code>system</code></th>
</tr>

<!-- Core -->
<tr>
<th colspan="3" align="center">Core Options</th>
</tr>

<tr>
<th><code>StatusBar</code></th>
<td>Shows or hides the browser status bar.</td>
<th><code>true</code></th>
</tr>

<tr>
<th><code>PinchZoom</code></th>
<td>Enables pinch-to-zoom gestures.</td>
<th><code>true</code></th>
</tr>

<tr>
<th><code>SwipeNavigation</code></th>
<td>Enables swipe gestures for back and forward navigation.</td>
<th><code>true</code></th>
</tr>

<tr>
<th><code>SmartScreen</code></th>
<td>
Enables SmartScreen protection. <a href="https://learn.microsoft.com/en-us/legal/microsoft-edge/privacy#smartscreen">More info.</a>
</td>
<th><code>true</code></th>
</tr>

<!-- Profile -->
<tr>
<th colspan="3" align="center">Profile Options</th>
</tr>

<tr>
<th><code>DownloadsFolderPath</code></th>
<td>Custom folder path where downloaded files are saved. Empty uses the system default.</td>
<th><code></code></th>
</tr>

<tr>
<th><code>ColorScheme</code></th>
<td>Controls the browser color scheme (<code>light</code>, <code>dark</code>, or <code>system</code>).</td>
<th><code>system</code></th>
</tr>

<tr>
<th><code>PasswordAutoSave</code></th>
<td>Allows automatic saving of passwords.</td>
<th><code>false</code></th>
</tr>

<tr>
<th><code>GeneralAutoFill</code></th>
<td>Enables form autofill for non-password fields.</td>
<th><code>true</code></th>
</tr>

</tbody>
</table>

<details>
<summary>UserSettings.ini</summary>

```ini
[Environment]
Extensions = false
FluentOverlayScrollBars = true
TrackingPrevention = true
BrowserLocale = system
BrowserArguments = --allow-file-access-from-files

[Controller]
ScriptLocale = system
PrivateMode = false

[Core]
StatusBar = true
PinchZoom = true
SwipeNavigation = true
SmartScreen = true

[Profile]
DownloadsFolderPath = 
ColorScheme = system
PasswordAutoSave = false
GeneralAutoFill = true

```

</details>

</details>

### Extensions

Using extensions on WebView2 is now possible!

Unfortunately, this feature doesn't come without limitations.

**Limitations**

- Extensions' UI may not show up at all, or they may show up on the DevTools(F5) window.

> ⚠️ Important: 
> - Manipulating Extensions require Rainmeter to be exited.

*Path: `C:\Users\User\AppData\Local\Temp\RainmeterWebView2\Extensions\`*

<details>
<summary><b>Installing Extensions</b></summary>

### Installing Extensions

To install an extension:

1. Exit Rainmeter
2. Go to `UserSettings.ini` and enable extensions -> `Extensions=true`
3. Drop an unpacked extension folder inside the `Extensions\` folder
4. Start Rainmeter 
5. Done

On Rainmeter, `WebView2: "Extension Name" extension installed.` will be logged.

Once an extension is installed, a new ini file will be created at `Extensions\Extensions.ini`, where you can control your extensions.

> ⚠️ Important:
> - Extensions must be **unpacked** folders.

</details>

<details>
<summary><b>Toggling Extensions</b></summary>

### Toggling Extensions

To enable\disable an extension:

1. Exit Rainmeter
2. Open `Extensions\Extensions.ini`
3. Find `[YourExtensionFolderName]` section
4. Set `Enabled=false` or `Enabled=false`
5. Save the file.
6. Launch Rainmeter
7. Done 

On Rainmeter, `WebView2: "Extension Name" extension enabled\disabled.` will be logged.

</details>

<details>
<summary><b>Unninstalling Extensions</b></summary>

### Unninstalling Extensions

To uninstall an extension:

1. Exit Rainmeter
2. Open `Extensions\Extensions.ini`
3. Find `[YourExtensionFolderName]` section
4. Set `Uninstall=true`
5. Save the file
6. Launch Rainmeter
7. Manually remove the extension's unpacked folder from `Extensions\`
8. Done 

On Rainmeter, `WebView2: "Extension Name" extension removed.` will be logged.

> ⚠️ Important: 
> * Uninstalling an extension will automatically delete its `[section]` from `Extensions.ini`, but will **not** remove its folder from the `Extensions\` folder.
> * If the folder is not manually removed after uninstalling the extension, it will be automatically re-installed the next time you launch Rainmeter.

</details>

<details>
<summary><b>Extensions.ini</b></summary>

*Path: `C:\Users\User\AppData\Local\Temp\RainmeterWebView2\Extensions\Extensions.ini`*

```ini
[TheExtensionFolderName]
ID = theextensionid
Name = The Extension Name
Enabled = true
Uninstall = false
```

> ⚠️ Important: 
> - You can only modify `Enabled` and `Unninstall`, other options are informative only.
> - Modifying or deleting `ID` will cause the extension to be reinstalled.
> - Modifying or deleting `Name` won't do anything, the name will be restored next time the plugin loads. 

</details>


</details>

---

## 🔥 JavaScript Integration

### Lifecycle Hooks

Your JavaScript can respond to Rainmeter events:

```javascript
// Called once when navigation starts
window.OnInitialize = function() {
   console.log("🚀 WebView initialized!");
   RainmeterAPI.Bang('[!Log "🚀 WebView initialized!"]')
};

// Called on every Rainmeter update
window.OnUpdate = function() {
    const now = new Date().toLocaleTimeString();
    updateSomething(now);
};
```

### Call JavaScript from Rainmeter

Use section variables to call any JavaScript function:

```ini
[MeterTemperature]
Meter=String
Text=Current temp: [WebView2:CallJS('getTemperature')]°C
DynamicVariables=1
```

```javascript
// In your HTML
window.getTemperature = function() {
    return 72;
};
```
> ⚠️ **Note:** JavaScript execution is asynchronous, so there's a 1-update delay between JS return and Rainmeter display. This is normal!

### Inject JS to Web Sites

From inline one-liner strings:

```ini
[WebView2]
Measure=Plugin
Plugin=WebView2
URL=https://example.com/
OnPageLoadFinishAction=[!CommandMeasure WebView2 "Execute alert('script executed'); console.log('hola!');"]
```

From files:

```js
// @Resources\script.js

alert('script executed from file');
console.log('hola!');
```

```ini
; skin.ini

[WebView2]
Measure=Plugin
Plugin=WebView2
URL=https://example.com/
OnPageLoadFinishAction=[!CommandMeasure WebView2 "Execute #@#script.js"]
```

### Use app-region CSS Style

Dragging elements with `app-region: drag;` set up will move the skin window. Right clicking these elements also opens the Skin Menu.

```css
body {
	app-region: drag;
}

button {
	app-region: no-drag;
}
```

When using `drag` on a div or any other container, you need to manually set `no-drag` on the interactable children of that container, otherwise they won't work properly.

This CSS style is being used on the `YoutubePlayer` example skin.

---

## 🌉 RainmeterAPI Bridge

Access Rainmeter's full power from JavaScript:

### Read Skin Options

```javascript
// Read from your measure
const refreshRate = await RainmeterAPI.ReadInt('UpdateRate', 1000);
const siteName = await RainmeterAPI.ReadString('SiteName', 'Default');

// Read from other sections
const cpuUsage = await RainmeterAPI.ReadStringFromSection('MeasureCPU', 'Value', '0');
```

### Execute Bangs

```javascript
// Set variables
await RainmeterAPI.Bang('!SetVariable MyVar "Hello World"');

// Control skins
await RainmeterAPI.Bang('!ActivateConfig "MySkin" "Variant.ini"');

// Update meters
await RainmeterAPI.Bang('!UpdateMeter MeterName');
await RainmeterAPI.Bang('!Redraw');
```

### Get Skin Information

```javascript
const skinName = await RainmeterAPI.SkinName;
const measureName = await RainmeterAPI.MeasureName;

// Replace variables
const path = await RainmeterAPI.ReplaceVariables('#@#images/logo.png');

// Get variable values
const theme = await RainmeterAPI.GetVariable('CurrentTheme');
```

### Logging

```javascript
await RainmeterAPI.Log('Debug info', 'DEBUG');
await RainmeterAPI.Log('Warning message', 'WARNING');
await RainmeterAPI.Log('Error occurred', 'ERROR');
```

### Complete API Reference

<details>
<summary>📚 <b>Click to see all available methods</b></summary>

<br/>

**Reading Options**
- `ReadString(option, defaultValue)` → `Promise<string>`
- `ReadInt(option, defaultValue)` → `Promise<number>`
- `ReadDouble(option, defaultValue)` → `Promise<number>`
- `ReadFormula(option, defaultValue)` → `Promise<number>`
- `ReadPath(option, defaultValue)` → `Promise<string>`

**Reading from Sections**
- `ReadStringFromSection(section, option, defaultValue)` → `Promise<string>`
- `ReadIntFromSection(section, option, defaultValue)` → `Promise<number>`
- `ReadDoubleFromSection(section, option, defaultValue)` → `Promise<number>`
- `ReadFormulaFromSection(section, option, defaultValue)` → `Promise<number>`

**Utility Functions**
- `ReplaceVariables(text)` → `Promise<string>`
- `GetVariable(variableName)` → `Promise<string>`
- `PathToAbsolute(relativePath)` → `Promise<string>`
- `Bang(command)` → `Promise<void>`
- `Log(message, level)` → `Promise<void>`

**Properties**
- `MeasureName` → `Promise<string>`
- `SkinName` → `Promise<string>`
- `SkinWindowHandle` → `Promise<string>`
- `SettingsFile` → `Promise<string>`

</details>

---

## ❓ How to Make a Draggable Skin

There are currently two ways to make WebView2 skins draggable.

<details>
<summary><b>Using Clickthrough</b></summary>

`Clickthrough` is a powerful option that allows mouse input to pass through the WebView2 window to the skin layer behind it, effectively making the window invisible to mouse interactions.

A new value, `2`, was recently added. This makes `Clickthrough` toggleable by holding the `CTRL` key.

**What do you need to do?** Nothing.

By default, `Clickthrough=2`. To drag the skin, simply **hold `CTRL` and drag**. This works because you are interacting directly with the skin, not with the WebView2 window.  
You can also open the Skin Menu with `CTRL + RMB`.
`Calendar.ini` uses this method.

```ini
[WebView2]
Measure=Plugin
Plugin=WebView2
X=0
Y=0
W=500
H=500
Clickthrough=2
```

Alternatively, you can use `Clickthrough=1` if the skin is only displaying information or images. This removes the need to hold CTRL, but disables all interaction with the WebView2 window.
`Clock.ini` uses this method.

```ini
[WebView2]
Measure=Plugin
Plugin=WebView2
X=0
Y=0
W=500
H=500
Clickthrough=1
```

</details>

<details>
<summary><b>Using app-region</b></summary>

WebView2 supports the `app-region` CSS property to define draggable and non-draggable areas on a page, allowing to drag the skin without holding `CTRL`.
This is a more advanced way to do it, but it will behave much more skin-like than the `clickthrough` way. `YouTubePlayer.ini` uses this method.

The property has two values:

* `drag`
* `no-drag`

---

*`app-region: drag`*

* Dragging the element moves the window
* Right-click opens the Skin Menu
* All child elements inherit this behavior

Use for non-interactive areas such as backgrounds or title bars.

```css
.titlebar {
  app-region: drag;
}
```

---

*`app-region: no-drag`*

* Disables window dragging
* Enables normal mouse interaction
* Overrides inherited `drag`

Required for buttons, inputs, sliders, and other interactive elements.

```css
.button,
input {
  app-region: no-drag;
}
```

---

*Common Pattern*

```css
.window {
  app-region: drag;
}

.controls {
  app-region: no-drag;
}
```

This keeps the window draggable while preserving UI interactivity.

You can also apply `app-region` directly in HTML using inline styles.

```html
<div class="titlebar" style="app-region: drag;">
  <span>My Skin</span>
  <button style="app-region: no-drag;">Nice Button</button>
</div>
```

In this example, the title bar remains draggable while the button stays fully interactive.

<details>
<summary><b>Draggable skin example</b></summary>

Load this file using a default WebView2 measure:

index.html
```html
<!DOCTYPE html>
<html>
	<style>
	body {
		background: black;
		color: white;
		display: flex;
		flex-direction: column;
		justify-content: center;
		align-items: center;
		height: 100vh;
		margin: 0;
		text-align: center;
		<!-- dragging magic -->
		app-region: drag; 
	}
	button {
		background: green;
		color: white;
		<!-- no-dragging magic -->
		app-region: no-drag; 
	}
	button:hover {
		background: lightgreen;
		color: black;
		cursor: pointer;
	}
	p { color: grey; }
	p:hover { color: white; }
	code { color: silver; }
	</style>
	<body>
		<h2>Draggable Skin Example</h1>
		<p>Drag the skin</p>
		<p>you can drag over everything</p>
		<button>but this button.</button>
		<p>hold <code>CTRL</code> and <code>drag</code> over the button</p>
	</body>
</html>
```

</details>

</details>

---

## ☄️ Setting Up a Virtual Host

Some APIs and features do not function correctly when accessed via the `file:///` protocol.
 Previously, this limitation required installing an external `http-server` to enable proper behavior.
 WebView2 now provides a built-in **Virtual Host** feature, which allows a plugin to be served from a virtual `http` or `https` URL, effectively replicating the behavior of a local web server.
 In practice, this means you can generate and use a custom virtual URL such as `https://my-skin-config-name/` or `http://my-skin-config-name/`.

This feature provides the following configuration options:

---

 **`HostSecurity`** (default: `1`)  
  Specifies which protocol the virtual host will use.  
  `0` - Not secure (`http`)  
  `1` - Secure (`https`)

---

 **`HostOrigin`** (default: `1`)  
  Defines which config is used as the origin for the virtual host.  
  `0` - Current config  
  `1` - Root config  

  This setting also determines how Local Storage is scoped:  
  - Using **Current config** restricts Local Storage access to the active skin (config) only.  
  - Using **Root config** allows Local Storage to be shared across all skins (configs) under the same root, which is useful for suites.

---

 **`HostPath`** (default: `""`)  
  Specifies the path to the folder containing your `file.html`.  
  Example: `HostPath = #@#`  

  When this option is set, the plugin enables the Virtual Host feature and generates a virtual URL mapped to the specified folder. The resulting URL uses a protocol defined by `HostSecurity` and a host name derived from the `HostOrigin` setting.

---

<details>
<summary>How it works:</summary>

The plugin detects that a Virtual Host should be used when the `HostPath` option points to a valid folder. Once detected, it initializes the virtual host and generates a new base `URL` that can be used to access the mapped files.

<details>
<summary><b>Example 1: Current Config as Origin (Isolated Storage)</b></summary>

Assume the following code belongs to the `Illustro\Clock` skin (config):

```ini
; HostSecurity:
; 0 = http  (insecure context)
; 1 = https (secure context)
; Using HTTPS allows access to APIs that may be blocked by CORS.
HostSecurity=1

; HostOrigin:
; 0 = current config only
; 1 = root config
; Using current config isolates storage to this skin
HostOrigin=0

; Folder containing the HTML and assets
HostPath=#@#

URL=index.html
```

In this case:

* The protocol is set to `https`

* The origin is derived from the current config: `Illustro\Clock`

* The `index.html` file resides in the `@Resources` folder

The generated base URL will be:

```
https://illustro-clock/
```

You can navigate to the page explicitly:
```ini
URL=https://illustro-clock/index.html
```

Or implicitly:
```ini
URL=index.html
```

When using a Virtual Host, the `URL` option automatically resolves relative paths against the generated virtual host URL.

Local Storage will be isolated to the `Illustro\Clock` skin, meaning it is scoped to the following origin:

```
https://illustro-clock/
```
</details>

<details>
<summary><b>Example 2: Root Config as Origin (Shared Storage)</b></summary>

Assume the following code belongs to the Illustro\Clock skin (config):

```ini
HostSecurity=0
HostOrigin=1
HostPath=#@#Clock\

URL=clock.html
```

In this case:

* The protocol is set to `http`

* The origin is derived from the root config: `Illustro`

* The `clock.html` file resides in the `@Resources\Clock` folder

The generated base URL will be:
```
http://illustro/
```

You can navigate to the page explicitly:

```ini
URL=http://illustro/clock.html
```

Or implicitly:

```ini
URL=clock.html
```

Local Storage will be shared across all skins (configs) that belong to the `Illustro` root config. In other words, Local Storage is scoped to:

```
http://illustro/
```

</details>

<details>
<summary><b>Practical Example</b></summary>

Assume the following skin structure:

```
📁 MyRootConfig\
  ├── 📁 @Resources\
  │   └── index.html
  └── MyRootConfigIni.ini
```

<details>
<summary>MyRootConfigIni.ini</summary>

```ini
[Rainmeter]
Update=1000

[WebView2]
Measure=Plugin
Plugin=WebView2
HostPath=#@#
Url=Index.html
W=300
H=300

[WebView2BG]
Meter=Image
W=300
H=300
SolidColor=0,0,0,255
```
</details>

<details>
<summary>Index.html</summary>

```html
<!DOCTYPE html>
<html>
	<style>
	body {
		color: white;
		display: flex;
		flex-direction: column;
		justify-content: center;
		align-items: center;
		height: 100vh;
		margin: 0;
		text-align: center;
	}
	</style>
	<body>
	
		<h1>Hello, Rainmeter!</h1>
		<p>This is a very simple HTML page.</p>
		<p>Hold CTRL to drag the skin</p>
		<p>Press CTRL + RMB to open SkinMenu.</p>
	
	</body>
</html>
```
</details>

After loading the skin, check the `skins` tab on the `About` window. You'll see that the measure is returning a URL as its string value, which is `https://myrootconfig/Index.html`.

By default, `HostSecurity=1`, therefore, as seen on the URL, it's using the `https` protocol.

Also, `HostOrigin=1` by default, but in this example it doesn't matter given our skin's structure. This skin has only a root config, so that's the only host name we can use. Which is the `myrootconfig/` part on the URL. 

If we now open `DevTools` (press F12 inside the WebView window) and go to the `Aplication` tab, then go to `Storage` -> `Local storage` on the left side panel, we'll see that our `https://myrootconfig` origin is listed.

Clicking on our origin will show all the key-value pairs that are stored. Obviously it is empty if you haven't saved anything yet. Check the storage for `YoutubePlayer` example skin instead. 


---

Now try using this skin's structure and play with both `HostSecurity` and `HostOrigin`. 

```
📁 MyRootConfig\
  ├── 📁 @Resources\
  │   └── index.html
  ├── 📁 MyOtherConfig\
  │    └── MyOtherConfigIni.ini
  └── MyRootConfigIni.ini
```

Try setting `HostOrigin=0` on both configs, you'll see the same `https://myrootconfig` origin on `DevTools` for both configs, which means they share local storage. 

If you then set `HostOrigin=1` on `MyOtherConfig`, you'll see its origin is now `https://myrootconfig-myotherconfig`, which means its local storage is not shared. 


</details>

</details>

</details>

</details>

---

## 💡 Examples

The plugin includes ready-to-use example skins:

<table>
<tr>
<td align="center" width="20%">
<b>🕐 Clock</b><br/>
<sub>Animated liquid clock with smooth animations</sub>
</td>
<td align="center" width="20%">
<b>📅 Calendar</b><br/>
<sub>Interactive month view calendar</sub>
</td>
<td align="center" width="20%">
<b>⚙️ Config Reader</b><br/>
<sub>Read options from measures and sections</sub>
</td>
<td align="center" width="20%">
<b>🔧 Utilities</b><br/>
<sub>Demonstrate all API functions</sub>
</td>
</td>
<td align="center" width="20%">
<b>▶️ Youtube Player</b><br/>
<sub>Youtube Player iFrame API example</sub>
</td>
</tr>
</table>

**To explore examples:**
1. Install the `.rmskin` package
2. Check your Rainmeter skins folder
3. Load example skins from Rainmeter manager

---

## 🛠️ Building from Source

<details>
<summary><b>For Developers: Build Instructions</b></summary>

<br/>

### Prerequisites

- Visual Studio 2022 with C++ desktop development
- Windows SDK
- PowerShell 5.1+

### Build Steps

```bash
# Clone repository
git clone https://github.com/yourusername/WebView2.git
cd WebView2

# Open in Visual Studio
start WebView2-Plugin.sln

# Build with PowerShell
powershell -ExecutionPolicy Bypass -Command "& {. .\Build-CPP.ps1; Dist -major 0 -minor 0 -patch 3}"
```

### Build Output

```
📁 dist/
  ├── 📁 x64/
  │   └── WebView2.dll
  ├── 📁 x32/
  │   └── WebView2.dll
  ├── WebView2_v0.0.3_x64_x86_dll.zip
  └── WebView2_v0.0.3_Alpha.rmskin
```

</details>

---

## 🆘 Troubleshooting

<details>
<summary><b>❌ "WebView2 Runtime is not installed"</b></summary>

<br/>

**Solution:** Install [WebView2 Runtime](https://developer.microsoft.com/en-us/microsoft-edge/webview2/)

Windows 11 has it pre-installed. For Windows 10, download and run the installer.

</details>

<details>
<summary><b>❌ "Failed to create WebView2 controller"</b></summary>

<br/>

**Try these steps:**

1. ✅ Right-click skin → **Refresh skin**
2. ✅ Restart Rainmeter completely
3. ✅ Verify WebView2 Runtime is installed
4. ✅ Check Windows Event Viewer for detailed errors

</details>

<details>
<summary><b>❌ "RainmeterAPI is not defined" in JavaScript</b></summary>

<br/>

**Solution:** Wait for page to load before accessing API:

```javascript
document.addEventListener('DOMContentLoaded', () => {
    // Now you can use RainmeterAPI
    RainmeterAPI.Log('Page loaded!', 'DEBUG');
});
```

</details>

<details>
<summary><b>❌ WebView not visible</b></summary>

<br/>

**Checklist:**

- ✅ Ensure `Hidden=0` in your measure (default is 0)
- ✅ Check URL path is correct
- ✅ Verify HTML file exists
- ✅ Look for errors in Rainmeter log
- ✅ Try: `[!CommandMeasure MeasureName "Open DevTools"]` to debug

**Transparency tip:** The WebView has transparent background by default. Use `background: transparent;` in your CSS.

</details>

---

## 📄 License

<div align="center">

**MIT License** - Free to use, modify, and distribute

See [LICENSE](LICENSE) file for full details

</div>

---

## 🤝 Contributing

We welcome contributions! Here's how:

<table>
<tr>
<td align="center" width="20%">
<b>1. Fork</b><br/>
🍴<br/>
<sub>Fork this repo</sub>
</td>
<td align="center" width="20%">
<b>2. Branch</b><br/>
🌿<br/>
<sub>Create feature branch</sub>
</td>
<td align="center" width="20%">
<b>3. Code</b><br/>
💻<br/>
<sub>Make your changes</sub>
</td>
<td align="center" width="20%">
<b>4. Commit</b><br/>
📝<br/>
<sub>Commit with clear message</sub>
</td>
<td align="center" width="20%">
<b>5. PR</b><br/>
🚀<br/>
<sub>Open Pull Request</sub>
</td>
</tr>
</table>

```bash
git checkout -b feature/AmazingFeature
git commit -m 'Add some AmazingFeature'
git push origin feature/AmazingFeature
```

---

## 🙏 Acknowledgments

<div align="center">

Built with powerful tools and inspired by the community

**[Microsoft Edge WebView2](https://developer.microsoft.com/en-us/microsoft-edge/webview2/)** • **[Rainmeter API](https://docs.rainmeter.net/developers/plugin/plugin-anatomy/)** • **Rainmeter Community**

</div>

---

<div align="center">

### 💖 Made with love for the Rainmeter community

**[⬆ Back to Top](#webview2-plugin-for-rainmeter)**

![Made with Love](https://img.shields.io/badge/Made%20with-❤️-red?style=for-the-badge)

</div>