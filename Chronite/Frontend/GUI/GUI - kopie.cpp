﻿#include "gui.h"
#include "../launcher.h"
#include "../../Backend/server.h"
#include "../settings.h"
#include "../util.h"

#pragma warning( disable : 4267 4244 4996)
using namespace std;

static int FilterNoSpace(ImGuiTextEditCallbackData* data)
{
	if (data->EventChar == ' ') return 1;
	return 0;
}

void ImGui::ShowLoader(bool* p_open)
{
	ImGuiWindowFlags window_flags = 0;

	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoSavedSettings;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	window_flags |= ImGuiWindowFlags_NoScrollWithMouse;

	SetNextWindowPos(ImVec2(500, 100), ImGuiCond_FirstUseEver);
	SetNextWindowSize(ImVec2(535, 320), ImGuiCond_FirstUseEver);


	if (!Begin("Neonite++", p_open, window_flags))
	{
		End();
		return;
	}

	PushItemWidth(GetFontSize() * -12);

	if (BeginTabBar("Neonite"), ImGuiTabBarFlags_AutoSelectNewTabs)
	{
		if (BeginTabItem("Main"))
		{
			if (Button("PlataniumV2"))
			{
				if (hLauncher == INVALID_HANDLE_VALUE)
				{
					if constexpr (&name[0] == "")
					{
						console.AddLog("[x] Please input an username!");
					}

					if constexpr (sizeof(name) < 2)
					{
						console.AddLog("[x] Your name must be long 3 or more characters!");
					}

					hLauncher = CreateThread(nullptr, NULL,
						reinterpret_cast<LPTHREAD_START_ROUTINE>(&launcher::Plat), nullptr,
						NULL, nullptr);
				}
				else
				{
					console.AddLog("[=] The game is already running.");
				}
			}

			SameLine(GetWindowWidth() - 390);

			InputTextWithHint("  ", "Input your Username", name, sizeof(name), ImGuiInputTextFlags_CallbackCharFilter,
			                  FilterNoSpace);

			SameLine(GetWindowWidth() - 100);

			
			if (Button("Cranium"))
			{
				if (hLauncher == INVALID_HANDLE_VALUE)
				{
					if constexpr (&name[0] == "")
					{
						console.AddLog("[x] Please input an username!");
					}

					if constexpr (sizeof(name) < 2)
					{
						console.AddLog("[x] Your name must be long 3 or more characters!");
					}

					hLauncher = CreateThread(nullptr, NULL,
						reinterpret_cast<LPTHREAD_START_ROUTINE>(&launcher::CraniumOnly), nullptr,
						NULL, nullptr);
				}
				else
				{
					console.AddLog("[=] The game is already running.");
				}
			}
			


			if (Button("Aurora.Runtime"))
			{
				if (hLauncher == INVALID_HANDLE_VALUE)
				{
					if constexpr (&name[0] == "")
					{
						console.AddLog("[x] Please input an username!");
					}

					if constexpr (sizeof(name) < 2)
					{
						console.AddLog("[x] Your name must be long 3 or more characters!");
					}

					hLauncher = CreateThread(nullptr, NULL,
						reinterpret_cast<LPTHREAD_START_ROUTINE>(&launcher::AuroraRuntime), nullptr,
						NULL, nullptr);
				}
				else
				{
					console.AddLog("[=] The game is already running.");
				}
			}


			SameLine(GetWindowWidth() - 390);

			InputTextWithHint(" ", "Fortnite Path (Optional)", path, sizeof(path));

			SameLine(GetWindowWidth() - 100);

			if (Button("Close Game"))
			{
				if (hLauncher != INVALID_HANDLE_VALUE)
				{
					try
					{
						TerminateProcess(hEAC, 1);
						TerminateProcess(hFortniteLauncher, 1);
					}
					catch (...)
					{
					}
					TerminateThread(hLauncher, 0);
					hLauncher = INVALID_HANDLE_VALUE;
					console.AddLog("[x] The game was terminated...");
				}
				else
				{
					console.AddLog("[=] The game wasn't running...");
				}
			}

			console.Draw("Server Console", p_open);

			SameLine(GetWindowWidth() - 200);

			if (Button("Save Config"))
			{
				settings::config(true);
			}

			SameLine(GetWindowWidth() - 465);

			//EndTabItem();

			//InputTextWithHint(" ", "Exchange Code (Enables ProdMode)", exchangeCode, sizeof(exchangeCode));

			SameLine(GetWindowWidth() - 100);

			if (Button("Load Config"))
			{
				settings::config(false);
			}

			EndTabItem();
		}
	}

	if (BeginTabItem(XOR("About")))
	{
		SetCursorPosX(GetCursorPosX() + 50);
		SetCursorPosY(GetCursorPosY() + 5);

		Text(XOR("Kemo (@xkem0x): Developer and mantainer of Neonite++"));

		SetCursorPosX(GetCursorPosX() + 50);
		SetCursorPosY(GetCursorPosY() + 5);

		Text(XOR("Sammy (@madSammy): Frontend, General."));

		SetCursorPosX(GetCursorPosX() + 50);
		SetCursorPosY(GetCursorPosY() + 5);

		Text(XOR("Darkblade (@DarkbladeEU): Internals, General."));

		SetCursorPosX(GetCursorPosX() + 50);
		SetCursorPosY(GetCursorPosY() + 5);

		Text(XOR("Taj (@AthenaBigBoi): Internals, General."));

		SetCursorPosX(GetCursorPosX() + 50);
		SetCursorPosY(GetCursorPosY() + 5);

		Text(XOR("Nyamimi (@nyameows): Internals, General."));

		SetCursorPosX(GetCursorPosX() + 50);
		SetCursorPosY(GetCursorPosY() + 5);

		Text(XOR("Irma (@omairma): Frontend, Internals."));

		SetCursorPosX(GetCursorPosX() + 50);
		SetCursorPosY(GetCursorPosY() + 5);

		Text(XOR("Project Polaris and Rift: Inspiration."));

		EndTabItem();
	}

	EndTabBar();


	if (*p_open == false)
	{
		bool EACisTermed;
		bool LauncherisTermed;
		DWORD FortniteLauncherPID;
		DWORD eacPID;

		while (true)
		{
			//making sure all processes are terminated
			eacPID = util::GetProcId("FortniteClient-Win64-Shipping_EAC.exe");

			if (eacPID != 0)
			{
				HANDLE EAC = OpenProcess(PROCESS_TERMINATE, false, eacPID);

				TerminateProcess(EAC, 0);
				eacPID = 0;
			}
			else
			{
				EACisTermed = true;
			}

			FortniteLauncherPID = util::GetProcId("FortniteLauncher.exe");

			if (FortniteLauncherPID != GetCurrentProcessId())
			{
				HANDLE Launcher = OpenProcess(PROCESS_TERMINATE, false, FortniteLauncherPID);

				TerminateProcess(Launcher, 0);
				FortniteLauncherPID = 0;
			}
			else
			{
				LauncherisTermed = true;
			}

			if (EACisTermed && LauncherisTermed)
			{
				TerminateProcess(hFortniteLauncher, 0);
				TerminateProcess(hEAC, 0);
				TerminateProcess(hClient, 0);
				TerminateThread(hLauncher, 0);
				TerminateThread(hServer, 0);
				break;
			}
		}


		exit(1);
	}

	End();


	ImGuiStyle* style = &GetStyle();

	style->WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style->WindowPadding = ImVec2(15, 8);
	style->WindowRounding = 3.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 0.9f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImColor(85, 0, 255);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImColor(85, 0, 255);
	style->Colors[ImGuiCol_TitleBgActive] = ImColor(85, 0, 255);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(
		0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(
		0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImColor(85, 0, 255);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Separator] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] =
	ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(
		0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(
		1.00f, 0.98f, 0.95f, 0.73f);
}
