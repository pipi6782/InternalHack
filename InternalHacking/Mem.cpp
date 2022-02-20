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
    //dst - � ��ġ�� �ִ� �޸��� �Ӽ��� �ٲܰų�
    //src - � ������ ���ų�
    //size - �Ӽ��� �ٲ� �޸��� ������ �󸶳� �ǳ�
    //hProcess - ������ ���μ����� � ���μ�����

    DWORD oldProtect; //�Ӽ��� ������ �޸��� ���� �Ӽ��� ����ϱ� ���� �뵵

    //�޸��� �Ӽ��� �ٲ� ��
    VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);

    //���ϴ� ���� ����
    WriteProcessMemory(hProcess, dst, src, size, nullptr);
    
    //������ �Ӽ����� ������Ų��
    VirtualProtectEx(hProcess, dst, size, oldProtect, &oldProtect);
}

void Mem::NopEx(BYTE* dst, UINT size, HANDLE hProcess)
{
    BYTE* nopArray = new BYTE[size];

    //0x90 >> nop = �ƹ� �ൿ�� ���� ����
    memset(nopArray, 0x90, size * sizeof(BYTE));

    PatchEx(dst, nopArray, size, hProcess);

    SafeDeleteArray(nopArray);
}

void Mem::Patch(BYTE* dst, BYTE* src, UINT size)
{
    //dst - � ��ġ�� �ִ� �޸��� �Ӽ��� �ٲܰų�
   //src - � ������ ���ų�
   //size - �Ӽ��� �ٲ� �޸��� ������ �󸶳� �ǳ�
   //hProcess - ������ ���μ����� � ���μ�����

    DWORD oldProtect; //�Ӽ��� ������ �޸��� ���� �Ӽ��� ����ϱ� ���� �뵵

    //�޸��� �Ӽ��� �ٲ� ��
    VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);

    //���ϴ� ���� ����
    memcpy(dst, src, size);

    //������ �Ӽ����� ������Ų��
    VirtualProtect(dst, size, oldProtect, &oldProtect);
}

void Mem::Nop(BYTE* dst, UINT size)
{
    DWORD oldProtect; //�Ӽ��� ������ �޸��� ���� �Ӽ��� ����ϱ� ���� �뵵

    //�޸��� �Ӽ��� �ٲ� ��
    VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);

    //���ϴ� ���� ����
    memset(dst, 0x90, size);

    //������ �Ӽ����� ������Ų��
    VirtualProtect(dst, size, oldProtect, &oldProtect);
}
