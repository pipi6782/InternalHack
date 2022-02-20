#include "Global.h"
#include "Mem.h"

Mem* Mem::instance = nullptr;

void Mem::Create()
{
    if (instance == nullptr)
        instance = new Mem();
}

Mem* Mem::Get()
{
    return instance;
}

void Mem::Destroy()
{
    SafeDelete(instance);
}

void Mem::PatchEx(BYTE* dst, BYTE* src, UINT size, HANDLE hProcess)
{
    //dst - 어떤 위치에 있는 메모리의 속성을 바꿀거냐
    //src - 어떤 내용을 쓸거냐
    //size - 속성을 바꿀 메모리의 범위는 얼마나 되냐
    //hProcess - 접근할 프로세스는 어떤 프로세스냐

    DWORD oldProtect; //속성을 변경할 메모리의 기존 속성을 백업하기 위한 용도

    //메모리의 속성을 바꾼 후
    VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);

    //원하는 값을 쓰고
    WriteProcessMemory(hProcess, dst, src, size, nullptr);
    
    //기존의 속성으로 원복시킨다
    VirtualProtectEx(hProcess, dst, size, oldProtect, &oldProtect);
}

void Mem::NopEx(BYTE* dst, UINT size, HANDLE hProcess)
{
    BYTE* nopArray = new BYTE[size];

    //0x90 >> nop = 아무 행동도 하지 않음
    memset(nopArray, 0x90, size * sizeof(BYTE));

    PatchEx(dst, nopArray, size, hProcess);

    SafeDeleteArray(nopArray);
}

void Mem::Patch(BYTE* dst, BYTE* src, UINT size)
{
    //dst - 어떤 위치에 있는 메모리의 속성을 바꿀거냐
   //src - 어떤 내용을 쓸거냐
   //size - 속성을 바꿀 메모리의 범위는 얼마나 되냐
   //hProcess - 접근할 프로세스는 어떤 프로세스냐

    DWORD oldProtect; //속성을 변경할 메모리의 기존 속성을 백업하기 위한 용도

    //메모리의 속성을 바꾼 후
    VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);

    //원하는 값을 쓰고
    memcpy(dst, src, size);

    //기존의 속성으로 원복시킨다
    VirtualProtect(dst, size, oldProtect, &oldProtect);
}

void Mem::Nop(BYTE* dst, UINT size)
{
    DWORD oldProtect; //속성을 변경할 메모리의 기존 속성을 백업하기 위한 용도

    //메모리의 속성을 바꾼 후
    VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);

    //원하는 값을 쓰고
    memset(dst, 0x90, size);

    //기존의 속성으로 원복시킨다
    VirtualProtect(dst, size, oldProtect, &oldProtect);
}
