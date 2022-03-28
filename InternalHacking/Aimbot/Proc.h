#pragma once

class Proc
{
private:
	Proc() = default;
	~Proc() = default;

public:
	static void Create();
	static Proc* Get();
	static void Destroy();

public:
	DWORD GetProcID(const wchar_t* procName);
	uintptr_t GetModuleBaseAddress(DWORD procID, const wchar_t* moduleName);
	uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<UINT> offsets);
	uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<UINT> offsets);

private:
	static Proc* instance;
};

