/**
 * Copyright (c) 2020-2021 Kareem Olim (Kemo)
 * All Rights Reserved. Licensed under the Neo License
 * https://neonite.dev/LICENSE.html
 */

#pragma once

inline void* (*ProcessEvent)(void*, void*, void*);
inline UObject* (*SpawnActor)(UObject* UWorld, UClass* Class, FTransform const* UserTransformPtr,
	const FActorSpawnParameters& SpawnParameters);
inline void (*GetFullName)(FField* Obj, FString& ResultString, const UObject* StopOuter, EObjectFullNameFlags Flags);
inline GObjects* GObjs;
inline UEngine* GEngine;

inline UObject* (*StaticConstructObject)(
	UClass* Class,
	UObject* InOuter,
	void* Name,
	EObjectFlags SetFlags,
	EInternalObjectFlags InternalSetFlags,
	UObject* Template,
	bool bCopyTransientsFromClassDefaults,
	void* InstanceGraph,
	bool bAssumeTemplateIsArchetype
	);

inline UObject* (*StaticLoadObject)(
	UClass* ObjectClass,
	UObject* InOuter,
	const TCHAR* InName,
	const TCHAR* Filename,
	uint32_t LoadFlags,
	void* Sandbox,
	bool bAllowObjectReconciliation,
	void* InstancingContext
	);

inline UObject* KismetRenderingLibrary;
inline UObject* KismetStringLibrary;


namespace UE4
{
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

	inline auto StaticLoadObjectEasy(UClass* inClass, const wchar_t* inName, UObject* inOuter = nullptr)
	{
		return StaticLoadObject(inClass, inOuter, inName, nullptr, 0, nullptr, false, nullptr);
	}

	//Frees the memory for the name
	inline void Free(void* buffer)
	{
		//FreeInternal(buffer);
		std::free(buffer);
	}

	//Find any entity inside the UGlobalObjects array aka. GObjects.
	template <typename T>
	static T FindObject(wchar_t const* name, bool ends_with = false, bool to_lower = false, int toSkip = 0)
	{
		for (auto i = 0x0; i < GObjs->NumElements; ++i)
		{
			auto object = GObjs->GetByIndex(i);
			if (object == nullptr)
			{
				continue;
			}

			std::wstring objectFullName = object->GetFullName();

			if (to_lower)
			{
				std::transform(objectFullName.begin(), objectFullName.end(), objectFullName.begin(),
					[](const unsigned char c) { return std::tolower(c); });
			}

			if (!ends_with)
			{
				if (objectFullName.starts_with(name))
				{
					if (toSkip > 0)
					{
						toSkip--;
					}
					else
					{
						return reinterpret_cast<T>(object);
					}
				}
			}
			else
			{
				if (objectFullName.ends_with(name))
				{
					return reinterpret_cast<T>(object);
				}
			}
		}
		return nullptr;
	}

	inline void DumpGObjects()
	{
		std::wofstream log("GObjects.log");

		for (auto i = 0x0; i < GObjs->NumElements; ++i)
		{
			auto object = GObjs->GetByIndex(i);
			if (object == nullptr)
			{
				continue;
			}
			std::wstring className = object->Class->GetFullName();
			std::wstring objectName = object->GetFullName();
			std::wstring item = L"\n[" + std::to_wstring(i) + L"] Object:[" + objectName + L"] Class:[" + className +
				L"]\n";
			log << item;
		}
	}

	inline void DumpBPs()
	{
		std::wofstream log("Blueprints.log");
		for (auto i = 0x0; i < GObjs->NumElements; ++i)
		{
			auto object = GObjs->GetByIndex(i);
			if (object == nullptr)
			{
				continue;
			}

			auto ClassName = object->Class->GetName();

			if (ClassName == XOR(L"BlueprintGeneratedClass"))
			{
				auto objectNameW = object->GetName();
				log << objectNameW + L"\n";
			}
		}
		log.flush();
	}
}
