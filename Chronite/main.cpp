#include "Frontend/GUI/gui.h"
#include "Frontend/util.h"
#include "Frontend/launcher.h"
#include "Discord.h"
#include "resource.h"

static LPDIRECT3D9 g_pD3D = nullptr;
static LPDIRECT3DDEVICE9 g_pd3dDevice = nullptr;
static D3DPRESENT_PARAMETERS g_d3dpp = {};

// Forward declarations of helper functions
void Init(LPDIRECT3DDEVICE9 pDevice, HWND hWnd);
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void ChangeDetail(std::string name)
{
	discordPresence.details = name.c_str();
	Discord_UpdatePresence(&discordPresence);
}


void make_minidump(EXCEPTION_POINTERS* e)
{
	auto hDbgHelp = LoadLibraryA("dbghelp");
	if (hDbgHelp == nullptr) return;
	auto pMiniDumpWriteDump = (decltype(&MiniDumpWriteDump))GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
	if (pMiniDumpWriteDump == nullptr) return;

	char name[MAX_PATH];
	{
		auto nameEnd = name + GetModuleFileNameA(GetModuleHandleA(nullptr), name, MAX_PATH);
		SYSTEMTIME t;
		GetSystemTime(&t);
		wsprintfA(nameEnd - strlen(".exe"),
		          "_%4d%02d%02d_%02d%02d%02d.dmp",
		          t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
	}

	auto hFile = CreateFileA(name, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE) return;

	MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
	exceptionInfo.ThreadId = GetCurrentThreadId();
	exceptionInfo.ExceptionPointers = e;
	exceptionInfo.ClientPointers = FALSE;

	auto dumped = pMiniDumpWriteDump(
		GetCurrentProcess(),
		GetCurrentProcessId(),
		hFile,
		MINIDUMP_TYPE(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory),
		e ? &exceptionInfo : nullptr,
		nullptr,
		nullptr);

	CloseHandle(hFile);
}

LONG CALLBACK unhandled_handler(EXCEPTION_POINTERS* e)
{
	make_minidump(e);
	return EXCEPTION_CONTINUE_SEARCH;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetUnhandledExceptionFilter(unhandled_handler);
	//ShowWindow(GetConsoleWindow(), SW_HIDE);
	ImGui_ImplWin32_EnableDpiAwareness();

	// Create application window
	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L,
		GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, _T("Neonite"),
		nullptr
	};
	::RegisterClassEx(&wc);
	HWND hwnd = ::CreateWindow(wc.lpszClassName, _T(XOR("Neonite++")), WS_POPUP, 500,
	                           100, 535, 320, NULL, NULL, wc.hInstance, NULL);

	window = hwnd;

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);
	Init(g_pd3dDevice, hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	// Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	// Enable Multi-Viewport / Platform Windows

	// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		style.Colors[ImGuiCol_TitleBg] = ImVec4(88.0f, 9.0f, 251.0f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(88.0f, 9.0f, 251.0f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(88.0f, 9.0f, 251.0f, 1.0f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(88.0f, 9.0f, 251.0f, 1.0f);
	}

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);

	// Our state
	bool show = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		{
			ImGui::ShowLoader(&show);
			StartRPC();
		}

		// Rendering
		ImGui::EndFrame();
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA(static_cast<int>(clear_color.x * 255.0f),
		                                      static_cast<int>(clear_color.y * 255.0f),
		                                      static_cast<int>(clear_color.z * 255.0f),
		                                      static_cast<int>(clear_color.w * 255.0f));
		g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		                    clear_col_dx, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);

		// Handle loss of D3D9 device
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() ==
			D3DERR_DEVICENOTRESET)
			ResetDevice();
	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr) return false;

	// Create the D3DDevice
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // Present with vsync
	//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
	                         D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp,
	                         &g_pd3dDevice) < 0)
		return false;

	return true;
}

void CleanupDeviceD3D()
{
	if (g_pd3dDevice)
	{
		g_pd3dDevice->Release();
		g_pd3dDevice = nullptr;
	}
	if (g_pD3D)
	{
		g_pD3D->Release();
		g_pD3D = nullptr;
	}
}

void ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

bool do_ini = true;

void Init(LPDIRECT3DDEVICE9 pDevice, HWND hWnd)
{
	if (do_ini)
	{
		do_ini = false;
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO* io = &ImGui::GetIO();
		io->IniFilename = nullptr;
		io->ConfigWindowsMoveFromTitleBarOnly = true;
		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplDX9_Init(pDevice);

		ImGui::StyleColorsDark();
		console.AddLog(XOR("                        [>] Welcome to Neonite++! \n"));
	}
}

// Win32 message handler
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg,
                                              WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;

	switch (msg)
	{
	case WM_CREATE:
	{
		HINSTANCE hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
		SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		return 0;
	}
	case WM_SIZE:
		if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
		{
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			ResetDevice();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_LBUTTONDOWN:
		int pos = GetMessagePos();
		POINTS ps = MAKEPOINTS(pos);
		POINT p = {ps.x, ps.y};

		ScreenToClient(hWnd, &p);

		if (p.y < 25 && p.x < 460)
		{
			ReleaseCapture();
			SendMessage(hWnd, WM_SYSCOMMAND, 0xf012, 0);
		}
		break;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
