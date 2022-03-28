#include "Global.h"
#include "Proc.h"

Proc* Proc::instance = nullptr;

void Proc::Create()
{
    if (instance != nullptr)
        instance = new Proc();
}

Proc* Proc::Get()
{
    return instance;
}

void Proc::Destroy()
{
    SafeDelete(instance);
}

DWORD Proc::GetProcID(const wchar_t* procName)
{
    //���� �����ϴ� ���μ����� ID��������
    DWORD procID = 0; //�����ϰ��� �ϴ� ���λ��� ID�� ����
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnap, &procEntry) == true)
        {
            do
            {
                //�����ϴ� ���μ����� �̸��� ���������� �ϴ� ���μ����� �̸��� ������
                if (!_wcsicmp(procEntry.szExeFile, procName))
                {
                    //procID�� ���� ���μ��� ���̵� ����
                    procID = procEntry.th32ProcessID;
                    break;
                }

            } while (Process32Next(hSnap, &procEntry));
        }
    }
    CloseHandle(hSnap);
    return procID;
}

uintptr_t Proc::GetModuleBaseAddress(DWORD procID, const wchar_t* moduleName)
{
    uintptr_t moduleBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 moduleEntry;
        moduleEntry.dwSize = sizeof(MODULEENTRY32);
        if (Module32First(hSnap, &moduleEntry))
        {
            do
            {
                if (_wcsicmp(moduleEntry.szModule, moduleName))
                {
                    moduleBaseAddr = (uintptr_t)moduleEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &moduleEntry));
        }
    }
    CloseHandle(hSnap);
    return moduleBaseAddr;
}

uintptr_t Proc::FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<UINT> offsets)
{
    uintptr_t addr = ptr;
    for (UINT i = 0; i < offsets.size(); i++)
    {
        ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(uintptr_t), 0);
        addr += offsets.at(i);
    }
    return addr;
}

uintptr_t Proc::FindDMAAddy(uintptr_t ptr, std::vector<UINT> offsets)
{
    uintptr_t addr = ptr;
    for (UINT i = 0; i < offsets.size(); i++)
    {
        // 0xD28,0x38,0,0x30,0x260,0x4F8
        addr = *(uintptr_t*)addr;
        addr += offsets.at(i);
    }
    return addr;
}
