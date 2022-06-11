#pragma once
#include "pch.h"
#include <winscard.h>
#include "enums.h"

#define ReadPointer(base, offset) (*(PVOID *)(((PBYTE)base + offset)))

#define VALIDATE_ADDRESS(address, error) \
    if (!address) { \
        MessageBoxA(0, error, "Cranium", MB_OK); \
        FreeLibraryAndExitThread(GetModuleHandle(NULL), 0); \
    }

#define DetoursEasy(address, hook) \
	DetourTransactionBegin(); \
    DetourUpdateThread(GetCurrentThread()); \
    DetourAttach(reinterpret_cast<void**>(&address), hook); \
    DetourTransactionCommit();

#define RELATIVE_ADDRESS(address, size) ((PBYTE)((UINT_PTR)(address) + *(PINT)((UINT_PTR)(address) + ((size) - sizeof(INT))) + (size)))

#define READ_POINTER(base, offset) (*(PVOID *)(((PBYTE)base + offset)))

#define READ_DWORD(base, offset) (*(PDWORD)(((PBYTE)base + offset)))

#define GET_POINTER(base, offset) *reinterpret_cast<PVOID*>((static_cast<PBYTE>(base) + offset));

class Util
{
private:
	static __forceinline bool MaskCompare(PVOID pBuffer, LPCSTR lpPattern, LPCSTR lpMask)
	{
		for (PBYTE value = static_cast<PBYTE>(pBuffer); *lpMask; ++lpPattern, ++lpMask, ++value)
		{
			if (*lpMask == 'x' && *reinterpret_cast<LPCBYTE>(lpPattern) != *value) return false;
		}
		return true;
	}
	
	static inline PVOID FindPatternAV(PVOID pBase, DWORD dwSize, LPCSTR lpPattern, LPCSTR lpMask)
	{
		dwSize -= static_cast<DWORD>(strlen(lpMask));

		for (auto index = 0UL; index < dwSize; ++index)
		{
			auto pAddress = reinterpret_cast<PBYTE>(pBase) + index;

			if (MaskCompare(pAddress, lpPattern, lpMask))
				return pAddress;
		}
		return NULL;
	}

	static __forceinline PBYTE FindPatternCh1(PVOID pBase, DWORD dwSize, LPCSTR lpPattern, LPCSTR lpMask)
	{
		dwSize -= static_cast<DWORD>(strlen(lpMask));
		for (auto i = 0UL; i < dwSize; ++i)
		{
			auto pAddr = static_cast<PBYTE>(pBase) + i;
			if (MaskCompare(pAddr, lpPattern, lpMask))
				return pAddr;
		}
		return NULL;
	}

public:
	static __forceinline PBYTE FindPatternCh1(LPCSTR lpPattern, LPCSTR lpMask)
	{
		// Grab module info, used for obtaining info like lpBaseOfDll and SizeOfImage.
		MODULEINFO info{};
		GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &info, sizeof(info));

		// Spin-out until the address has been found.
		PBYTE pAddr = NULL;
		do
		{
			pAddr = FindPatternCh1(info.lpBaseOfDll, info.SizeOfImage, lpPattern, lpMask);

			Sleep(200); // This could cause Fortnite to hang at startup.
		} while (!pAddr);

		// After the spin-out has been completed, return the address!
		return pAddr;
	}

	static inline PVOID FindPatternAV(LPCSTR lpPattern, LPCSTR lpMask)
	{
		MODULEINFO info = { 0 };

		GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &info, sizeof(info));

		return FindPatternAV(info.lpBaseOfDll, info.SizeOfImage, lpPattern, lpMask);
	}

	static __forceinline uintptr_t FindPattern(PVOID pBase, DWORD dwSize, LPCSTR lpPattern, LPCSTR lpMask)
	{
		dwSize -= static_cast<DWORD>(strlen(lpMask));
		for (unsigned long index = 0; index < dwSize; ++index)
		{
			PBYTE pAddress = static_cast<PBYTE>(pBase) + index;
			if (MaskCompare(pAddress, lpPattern, lpMask)) return reinterpret_cast<uintptr_t>(pAddress);
		}
		return NULL;
	}

	static __forceinline uintptr_t FindPattern(LPCSTR lpPattern, LPCSTR lpMask, BOOL SleepBetween = false)
	{
		MODULEINFO info = {nullptr};
		GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &info, sizeof(info));

		uintptr_t pAddr = 0;

		do
		{
			pAddr = FindPattern(info.lpBaseOfDll, info.SizeOfImage, lpPattern, lpMask);

			Sleep(50);
		}
		while (!pAddr);

		return pAddr;
	}
	
	static void* FindStringRef(const std::wstring& string)
	{
		uintptr_t base_address = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));

		const auto dosHeader = (PIMAGE_DOS_HEADER)base_address;
		const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)base_address + dosHeader->e_lfanew);

		IMAGE_SECTION_HEADER* textSection = nullptr;
		IMAGE_SECTION_HEADER* rdataSection = nullptr;

		auto sectionsSize = ntHeaders->FileHeader.NumberOfSections;
		auto section = IMAGE_FIRST_SECTION(ntHeaders);

		for (WORD i = 0; i < sectionsSize; section++)
		{
			auto secName = std::string((char*)section->Name);

			if (secName == ".text")
			{
				textSection = section;
			}
			else if (secName == ".rdata")
			{
				rdataSection = section;
			}

			if (textSection && rdataSection)
				break;
		}

		auto textStart = base_address + textSection->VirtualAddress;

		auto rdataStart = base_address + rdataSection->VirtualAddress;
		auto rdataEnd = rdataStart + rdataSection->Misc.VirtualSize;

		const auto scanBytes = reinterpret_cast<std::uint8_t*>(textStart);

		//scan only text section
		for (DWORD i = 0x0; i < textSection->Misc.VirtualSize; i++)
		{
			if (&scanBytes[i])
			{
				if ((scanBytes[i] == ASM::CMOVL || scanBytes[i] == ASM::CMOVS) && scanBytes[i + 1] == ASM::LEA)
				{
					auto stringAdd = reinterpret_cast<uintptr_t>(&scanBytes[i]);
					stringAdd = stringAdd + 7 + *reinterpret_cast<int32_t*>(stringAdd + 3);

					//check if the pointer is actually a valid string by checking if it's inside the rdata section
					if (stringAdd > rdataStart && stringAdd < rdataEnd)
					{
						std::wstring lea((wchar_t*)stringAdd);

						if (lea == string)
						{
							return &scanBytes[i];
						}
					}
				}
			}
		}

		return nullptr;
	}

	static __forceinline uintptr_t FindPatternEasy(const char* signature, bool bRelative = false, uint32_t offset = 0)
	{
		uintptr_t base_address = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));
		static auto patternToByte = [](const char* pattern)
		{
			auto bytes = std::vector<int>{};
			const auto start = const_cast<char*>(pattern);
			const auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current)
			{
				if (*current == '?')
				{
					++current;
					if (*current == '?') ++current;
					bytes.push_back(-1);
				}
				else { bytes.push_back(strtoul(current, &current, 16)); }
			}
			return bytes;
		};

		const auto dosHeader = (PIMAGE_DOS_HEADER)base_address;
		const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)base_address + dosHeader->e_lfanew);

		const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto patternBytes = patternToByte(signature);
		const auto scanBytes = reinterpret_cast<std::uint8_t*>(base_address);

		const auto s = patternBytes.size();
		const auto d = patternBytes.data();

		for (auto i = 0ul; i < sizeOfImage - s; ++i)
		{
			bool found = true;
			for (auto j = 0ul; j < s; ++j)
			{
				if (scanBytes[i + j] != d[j] && d[j] != -1)
				{
					found = false;
					break;
				}
			}
			if (found)
			{
				uintptr_t address = reinterpret_cast<uintptr_t>(&scanBytes[i]);
				if (bRelative)
				{
					address = ((address + offset + 4) + *(int32_t*)(address + offset));
					return address;
				}
				return address;
			}
		}
		return NULL;
	}

	static void* FindByString(const std::wstring& string, std::vector<int> opcodesToFind = {}, bool bRelative = false, uint32_t offset = 0, bool forward = false)
	{
		auto ref = FindStringRef(string);

		if (ref)
		{
			printf("Ref %ls %p\n", string.c_str(), ref);


			const auto scanBytes = static_cast<std::uint8_t*>(ref);

			//scan backwards till we hit a ret (and assume this is the function start)
			for (auto i = 0; forward ? (i < 2048) : (i > -2048); forward ? i++ : i--)
			{
				if (opcodesToFind.size() == 0)
				{
					if (scanBytes[i] == ASM::INT3 || scanBytes[i] == ASM::RETN)
					{
						return &scanBytes[i + 1];
					}
				}
				else
				{
					for (uint8_t byte : opcodesToFind)
					{
						if (scanBytes[i] == byte && byte != ASM::SKIP)
						{
							if (bRelative)
							{
								uintptr_t address = reinterpret_cast<uintptr_t>(&scanBytes[i]);
								address = ((address + offset + 4) + *(int32_t*)(address + offset));
								return (void*)address;
							}
							return &scanBytes[i];
						}
					}
				}
			}
		}

		return nullptr;
	}


	static __forceinline std::wstring sSplit(std::wstring s, std::wstring delimiter)
	{
		size_t pos;
		std::wstring token;
		while ((pos = s.find(delimiter)) != std::string::npos)
		{
			token = s.substr(0, pos);
			return token;
		}
		return token;
	}

	static __forceinline std::wstring GetRuntimePath()
	{
		wchar_t result[MAX_PATH];
		std::wstring path(result, GetModuleFileNameW(nullptr, result, MAX_PATH));
		size_t pos = path.find_last_of(L"\\/");
		return (std::wstring::npos == pos) ? L"" : path.substr(0, pos);
	}

	static __forceinline void CopyToClipboard(const std::string& s)
	{
		OpenClipboard(nullptr);
		EmptyClipboard();
		const auto hg = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
		if (!hg)
		{
			CloseClipboard();
			return;
		}
		memcpy(GlobalLock(hg), s.c_str(), s.size() + 1);
		GlobalUnlock(hg);
		SetClipboardData(CF_TEXT, hg);
		CloseClipboard();
		GlobalFree(hg);
	}

	static __forceinline bool IsBadReadPtr(void* p)
	{
		MEMORY_BASIC_INFORMATION mbi;
		if (VirtualQuery(p, &mbi, sizeof(mbi)))
		{
			DWORD mask = (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);
			bool b = !(mbi.Protect & mask);
			if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) b = true;

			return b;
		}
		return true;
	}
};
