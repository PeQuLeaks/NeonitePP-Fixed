#pragma once
#include "settings.h"
#include "util.h"
#include "GUI/gui.h"


inline HANDLE hLauncher = INVALID_HANDLE_VALUE;
inline HANDLE hFortniteLauncher = INVALID_HANDLE_VALUE;
inline HANDLE hEAC = INVALID_HANDLE_VALUE;
inline HANDLE hClient = INVALID_HANDLE_VALUE;

namespace launcher
{
	inline void Plat()
	{
		std::string exchange = exchangeCode;
		std::string altPath = path;
		std::string fnPath;

		if (altPath.empty())
		{
			char programData[MAX_PATH];
			char datPath[] = "\\Epic\\UnrealEngineLauncher\\LauncherInstalled.dat";
			json launcherInstalled;

			SHGetFolderPath(nullptr, CSIDL_COMMON_APPDATA,
				nullptr, 0, programData);

			//TODO: change this
			char* launcherInstalledPath = new char[
				std::strlen(programData) + std::strlen(datPath) + 1];

			std::strcpy(launcherInstalledPath, programData);
			std::strcat(launcherInstalledPath, datPath);

			std::ifstream f(launcherInstalledPath);
			f >> launcherInstalled;

			json installationList = launcherInstalled["InstallationList"];

			for (int i = 0; i < installationList.size(); i++)
			{
				if (installationList[i]["AppName"] == "Fortnite")
				{
					fnPath = installationList[i]["InstallLocation"];
				}
			}

			if (fnPath.empty())
			{
				console.AddLog("[x] Couldn't find Fortnite directory.");
				return;
			}

			strncpy(path, fnPath.c_str(), fnPath.size() + 1);
			settings::config(true);
		}
		else
		{
			fnPath = altPath;
		}

		std::string szClientFile =
			fnPath +
			"\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe";
		std::string szEACFile =
			fnPath +
			"\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping_EAC.exe";

		std::string szLauncherFile =
			fnPath +
			"\\FortniteGame\\Binaries\\Win64\\FortniteLauncher.exe";

		std::ostringstream oss;
		if (exchange.empty())
		{
			oss <<
				" -epicapp=Fortnite -epicenv=Prod -epiclocale=en-us -epicportal -nobe -fromfl=eac -fltoken=3db3ba5dcbd2e16703f3978d -AUTH_LOGIN="
				<< name
				<< "@unused.com -AUTH_PASSWORD=unused -AUTH_TYPE=epic";
		}
		else
		{
			oss <<
				" -AUTH_LOGIN=unused AUTH_TYPE=exchangecode -epicapp=Fortnite -epicenv=Prod-nobe -fromfl=eac -fltoken=3db3ba5dcbd2e16703f3978d -epicportal -epiclocale=en-us -AUTH_PASSWORD="
				<< exchange;
		}
		std::string s = oss.str();
		char* args = new char[s.length() + 1];
		std::copy(s.c_str(), s.c_str() + s.length() + 1, args);

		hEAC = util::startup(szEACFile.c_str(), args);
		util::suspend(hEAC);

		hFortniteLauncher = util::startup(szLauncherFile.c_str(), args);
		util::suspend(hFortniteLauncher);

		hClient = util::startup(szClientFile.c_str(), args);

		if (hClient && hClient != INVALID_HANDLE_VALUE)
		{
			console.AddLog("[+] Fortnite was launched.");

			//prevent mapping the dll too fast
			while (pid == 0)
			{
				pid = util::GetProcId("FortniteClient-Win64-Shipping.exe");
			}

			std::string dllPath = util::GetEXEPath() + "\\Cranium.dll";
			std::string dllSslPath = util::GetEXEPath() + "\\PlataniumV2.dll";

			console.AddLog("[+] Trying to inject PlataniumV2.");
			if (ManualMap(hClient, dllSslPath.c_str()))
			{
				//CloseHandle(hClient);
				console.AddLog("[+] Injected PlataniumV2.");
				console.AddLog("[+] Injecting Cranium.");
				console.AddLog("[=] Ultimate FLEX.");

				if (!ManualMap(hClient, dllPath.c_str())) {
					console.AddLog("[x] Something went wrong when trying to inject Cranium!");
					console.AddLog("[x] Trying to continue anyways.");

				}
			}
			else {
				console.AddLog("[x] Something went wrong while injecting PlataniumV2.");
				console.AddLog("[x] Trying to close the game.");
				CloseHandle(hClient);
			}

			while (true)
			{
				if (WaitForSingleObject(hClient, 10) != WAIT_TIMEOUT) break;
			}
			util::resume(hEAC);
			util::resume(hFortniteLauncher);
			TerminateProcess(hEAC, 1);
			TerminateProcess(hFortniteLauncher, 1);
			TerminateProcess(hLauncher, 1);
			hEAC = INVALID_HANDLE_VALUE;
			hLauncher = INVALID_HANDLE_VALUE;
			hFortniteLauncher = INVALID_HANDLE_VALUE;
			hClient = INVALID_HANDLE_VALUE;
		}
	}
	inline void AuroraRuntime()
	{
		std::string exchange = exchangeCode;
		std::string altPath = path;
		std::string fnPath;

		if (altPath.empty())
		{
			char programData[MAX_PATH];
			char datPath[] = "\\Epic\\UnrealEngineLauncher\\LauncherInstalled.dat";
			json launcherInstalled;

			SHGetFolderPath(nullptr, CSIDL_COMMON_APPDATA,
				nullptr, 0, programData);

			//TODO: change this
			char* launcherInstalledPath = new char[
				std::strlen(programData) + std::strlen(datPath) + 1];

			std::strcpy(launcherInstalledPath, programData);
			std::strcat(launcherInstalledPath, datPath);

			std::ifstream f(launcherInstalledPath);
			f >> launcherInstalled;

			json installationList = launcherInstalled["InstallationList"];

			for (int i = 0; i < installationList.size(); i++)
			{
				if (installationList[i]["AppName"] == "Fortnite")
				{
					fnPath = installationList[i]["InstallLocation"];
				}
			}

			if (fnPath.empty())
			{
				console.AddLog("[x] Couldn't find Fortnite directory.");
				return;
			}

			strncpy(path, fnPath.c_str(), fnPath.size() + 1);
			settings::config(true);
		}
		else
		{
			fnPath = altPath;
		}

		std::string szClientFile =
			fnPath +
			"\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe";
		std::string szEACFile =
			fnPath +
			"\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping_EAC.exe";

		std::string szLauncherFile =
			fnPath +
			"\\FortniteGame\\Binaries\\Win64\\FortniteLauncher.exe";

		std::ostringstream oss;
		if (exchange.empty())
		{
			oss <<
				" -epicapp=Fortnite -epicenv=Prod -epiclocale=en-us -epicportal -nobe -fromfl=eac -fltoken=3db3ba5dcbd2e16703f3978d -AUTH_LOGIN="
				<< name
				<< "@unused.com -AUTH_PASSWORD=unused -AUTH_TYPE=epic";
		}
		else
		{
			oss <<
				" -AUTH_LOGIN=unused AUTH_TYPE=exchangecode -epicapp=Fortnite -epicenv=Prod-nobe -fromfl=eac -fltoken=3db3ba5dcbd2e16703f3978d -epicportal -epiclocale=en-us -AUTH_PASSWORD="
				<< exchange;
		}
		std::string s = oss.str();
		char* args = new char[s.length() + 1];
		std::copy(s.c_str(), s.c_str() + s.length() + 1, args);

		hEAC = util::startup(szEACFile.c_str(), args);
		util::suspend(hEAC);

		hFortniteLauncher = util::startup(szLauncherFile.c_str(), args);
		util::suspend(hFortniteLauncher);

		hClient = util::startup(szClientFile.c_str(), args);

		if (hClient && hClient != INVALID_HANDLE_VALUE)
		{
			console.AddLog("[+] Fortnite was launched.");

			//prevent mapping the dll too fast
			while (pid == 0)
			{
				pid = util::GetProcId("FortniteClient-Win64-Shipping.exe");
			}

			std::string dllPath = util::GetEXEPath() + "\\Cranium.dll";
			std::string dllSslPath = util::GetEXEPath() + "\\Aurora.Runtime.dll";

			console.AddLog("[+] Trying to inject Aurora.Runtime.");
			if (ManualMap(hClient, dllSslPath.c_str()))
			{
				//CloseHandle(hClient);
				console.AddLog("[+] Injected Aurora.Runtime.");
				console.AddLog("[+] Injecting Cranium.");
				console.AddLog("[=] Ultimate FLEX.");

				if (!ManualMap(hClient, dllPath.c_str())) {
					console.AddLog("[x] Something went wrong when trying to inject Cranium!");
					console.AddLog("[x] Trying to continue anyways.");
					//console.AddLog("[x] Ultimate FLEX.");
				}
			}
			else {
				console.AddLog("[x] Something went wrong while injecting Aurora.Runtime.");
				console.AddLog("[x] Trying to close the game.");
				CloseHandle(hClient);
			}

			while (true)
			{
				if (WaitForSingleObject(hClient, 10) != WAIT_TIMEOUT) break;
			}
			util::resume(hEAC);
			util::resume(hFortniteLauncher);
			TerminateProcess(hEAC, 1);
			TerminateProcess(hFortniteLauncher, 1);
			TerminateProcess(hLauncher, 1);
			hEAC = INVALID_HANDLE_VALUE;
			hLauncher = INVALID_HANDLE_VALUE;
			hFortniteLauncher = INVALID_HANDLE_VALUE;
			hClient = INVALID_HANDLE_VALUE;
		}
	}
	inline void CraniumOnly()
	{
		std::string exchange = exchangeCode;
		std::string altPath = path;
		std::string fnPath;

		if (altPath.empty())
		{
			char programData[MAX_PATH];
			char datPath[] = "\\Epic\\UnrealEngineLauncher\\LauncherInstalled.dat";
			json launcherInstalled;

			SHGetFolderPath(nullptr, CSIDL_COMMON_APPDATA,
				nullptr, 0, programData);

			//TODO: change this
			char* launcherInstalledPath = new char[
				std::strlen(programData) + std::strlen(datPath) + 1];

			std::strcpy(launcherInstalledPath, programData);
			std::strcat(launcherInstalledPath, datPath);

			std::ifstream f(launcherInstalledPath);
			f >> launcherInstalled;

			json installationList = launcherInstalled["InstallationList"];

			for (int i = 0; i < installationList.size(); i++)
			{
				if (installationList[i]["AppName"] == "Fortnite")
				{
					fnPath = installationList[i]["InstallLocation"];
				}
			}

			if (fnPath.empty())
			{
				console.AddLog("[x] Couldn't find Fortnite directory.");
				return;
			}

			strncpy(path, fnPath.c_str(), fnPath.size() + 1);
			settings::config(true);
		}
		else
		{
			fnPath = altPath;
		}

		std::string szClientFile =
			fnPath +
			"\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe";
		std::string szEACFile =
			fnPath +
			"\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping_EAC.exe";

		std::string szLauncherFile =
			fnPath +
			"\\FortniteGame\\Binaries\\Win64\\FortniteLauncher.exe";

		std::ostringstream oss;
		if (exchange.empty())
		{
			oss <<
				" -epicapp=Fortnite -epicenv=Prod -epiclocale=en-us -epicportal -nobe -fromfl=eac -fltoken=3db3ba5dcbd2e16703f3978d -AUTH_LOGIN="
				<< name
				<< "@unused.com -AUTH_PASSWORD=unused -AUTH_TYPE=epic";
		}
		else
		{
			oss <<
				" -AUTH_LOGIN=unused AUTH_TYPE=exchangecode -epicapp=Fortnite -epicenv=Prod-nobe -fromfl=eac -fltoken=3db3ba5dcbd2e16703f3978d -epicportal -epiclocale=en-us -AUTH_PASSWORD="
				<< exchange;
		}
		std::string s = oss.str();
		char* args = new char[s.length() + 1];
		std::copy(s.c_str(), s.c_str() + s.length() + 1, args);

		hEAC = util::startup(szEACFile.c_str(), args);
		util::suspend(hEAC);

		hFortniteLauncher = util::startup(szLauncherFile.c_str(), args);
		util::suspend(hFortniteLauncher);

		hClient = util::startup(szClientFile.c_str(), args);

		if (hClient && hClient != INVALID_HANDLE_VALUE)
		{
			console.AddLog("[+] Fortnite was launched.");

			//prevent mapping the dll too fast
			while (pid == 0)
			{
				pid = util::GetProcId("FortniteClient-Win64-Shipping.exe");
			}

			std::string dllPath = util::GetEXEPath() + "\\Cranium.dll";
			//std::string dllSslPath = util::GetEXEPath() + "\\SSLLocal.dll";

			console.AddLog("[+] Trying to inject Cranium.");

			if (!ManualMap(hClient, dllPath.c_str())) {
				console.AddLog("[x] Something went wrong when trying to inject Cranium!");
				//console.AddLog("[+] Something went wrong while injecting PlataniumV2.");
				console.AddLog("[x] Trying to close the game.");
				CloseHandle(hClient);
			}


			while (true)
			{
				if (WaitForSingleObject(hClient, 10) != WAIT_TIMEOUT) break;
			}
			util::resume(hEAC);
			util::resume(hFortniteLauncher);
			TerminateProcess(hEAC, 1);
			TerminateProcess(hFortniteLauncher, 1);
			TerminateProcess(hLauncher, 1);
			hEAC = INVALID_HANDLE_VALUE;
			hLauncher = INVALID_HANDLE_VALUE;
			hFortniteLauncher = INVALID_HANDLE_VALUE;
			hClient = INVALID_HANDLE_VALUE;
		}
	}
}
