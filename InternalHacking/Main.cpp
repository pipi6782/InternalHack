#include "Global.h"
#include "Proc.h"
#include "Mem.h"

#define IDC_INFAMMO_BUTTON 100

#define DLL_EXPORT extern "C" __declspec(dllexport)

//Global Variable
HINSTANCE hInstance;
HWND g_hWnd;
DWORD procID;
uintptr_t moduleBase;
HANDLE hProcess;
vector<UINT> bulletOffsets = { 0xD28,0x38,0,0x30,0x260,0x4F8 };
uintptr_t engineAddress;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void RegisterWndClass();


DWORD WINAPI HackThread(HMODULE hModule)
{
    {
 //   ////Get module base
 //   //uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"Personal-Win64-Shipping.exe");

 //   //bool bAmmo = false;

 //   //vector<UINT> offsets = { 0xD28,0x38,0,0x30,0x260,0x4F8 };
 //   //uintptr_t* gEngine = (uintptr_t*)(moduleBase + 0x49F9308); //GEngine�ּ�
 //   //uintptr_t bulletAddr = Proc::Get()->FindDMAAddy((uintptr_t)gEngine, offsets);

	////Create Window
 //   // Initialize global strings
 //   
 //   LoadStringW(hInst, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
 //   LoadStringW(hInst, IDC_INTERNALHACK, szWindowClass, MAX_LOADSTRING);
 //   MyRegisterClass(hInst);

 //   HWND hwnd = CreateWindow(szWindowClass,
 //       szTitle,
 //       WS_OVERLAPPEDWINDOW,
 //       CW_USEDEFAULT,
 //       CW_USEDEFAULT,
 //       800,
 //       600,
 //       0,
 //       0,
 //       hInst,
 //       0);

 //   if(hwnd == nullptr)
 //       MessageBox(NULL, L"null", L"null", MB_OK);

 //   BOOL result = ShowWindow(hwnd, SW_NORMAL);
 //   if(result == FALSE)
 //       MessageBox(NULL, L"FALSE", L"FALSE", MB_OK);

 //   UpdateWindow(hwnd);
 //   MSG msg;

 //   while (GetMessage(&msg, NULL, 0, 0))
 //   {
 //       TranslateMessage(&msg);
 //       DispatchMessage(&msg);
 //   }

	//FreeConsole();
	//FreeLibraryAndExitThread(hModule, 0);
	//return 0;

    }

	hInstance = hModule;
	MSG msg;
	RegisterWndClass();
	

	g_hWnd = CreateWindow(
		L"Hacking",
		L"Hacking",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		800,
		600,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (g_hWnd == nullptr)
	{
		FreeLibraryAndExitThread(hModule, 0);
		return 0;
	}

	ShowWindow(g_hWnd, SW_SHOW);
	MessageBox(nullptr, L"result false", L"result false", MB_OK);

	UpdateWindow(g_hWnd);
	MessageBox(nullptr, L"update false", L"update false", MB_OK);

	procID = Proc::Get()->GetProcID(L"Personal-Win64-Shipping.exe");

	if (procID != NULL)
	{
		//��� ���̵� ������
		moduleBase = Proc::Get()->GetModuleBaseAddress(procID, L"Personal.exe");

		cout << std::hex << moduleBase << endl;
		//��ŷ�� ���α׷��� �ڵ� ������
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procID); //PROCESS_ALL_ACCESS ��� ������ ����� �� �ְ� ��

		//GEngine ������
		engineAddress = moduleBase + 0x49F9308;
	}
	else
	{
		FreeLibraryAndExitThread(hModule, 0);
		return 0;
	}

	while(true)
	{
		if (PeekMessage(&msg, g_hWnd, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	FreeLibraryAndExitThread(hModule, 0);
	return msg.wParam;
}

void RegisterWndClass()
{
	WNDCLASS wndClass;

	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = CreateSolidBrush(0xFF0000);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = NULL;
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = L"Hacking";
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wndClass);
}

int APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	Proc::Create();
	Mem::Create();
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;

	default:
		break;
	}

	Proc::Destroy();
	Mem::Destroy();

	return TRUE;
}



////���μ��� ���̵� ������
//DWORD procID = Proc::Get()->GetProcID(L"Personal-Win64-Shipping.exe");
//
//if (procID)
//{
//	//��� ���̵� ������
//	moduleBase = Proc::Get()->GetModuleBaseAddress(procID, L"Personal.exe");
//
//	cout << std::hex << moduleBase << endl;
//	//��ŷ�� ���α׷��� �ڵ� ������
//	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procID); //PROCESS_ALL_ACCESS ��� ������ ����� �� �ְ� ��
//
//	////GEngine ������
//	uintptr_t dynamicPtrBaseAddr = moduleBase + 0x49F9308;
//
//	////Character�� Bullet�� �ּ� ��������
//	vector<UINT> offsets = { 0xD28,0x38,0,0x30,0x260,0x4F8 };
//	bulletAddr = Proc::Get()->FindDMAAddy(hProcess, dynamicPtrBaseAddr, offsets);
//
//	int current;
//	ReadProcessMemory(hProcess, reinterpret_cast<BYTE*>(bulletAddr), &current, sizeof(int), nullptr);
//	cout << std::dec << current << endl;
//}
//else
//{
//	cout << "���μ����� ����" << endl;
//	return 0;
//	Mem::Destroy();
//	Proc::Destroy();
//}
//
//DWORD dwExit = 0;
//while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE)
//{
//	if (GetAsyncKeyState('1') & 1)
//	{
//		bBullet = !bBullet;
//		if (bBullet == true)
//			Mem::Get()->PatchEx(reinterpret_cast<BYTE*>(moduleBase + 0xE11312), (BYTE*)("/x83/x87/xF8/x04/x00/x00/x01"), 7, hProcess);
//		else
//			Mem::Get()->PatchEx(reinterpret_cast<BYTE*>(moduleBase + 0xE11312), (BYTE*)("/x83/x87/xF8/x04/x00/x00/x02"), 7, hProcess);
//		cout << (bBullet ? "true" : "false") << endl;
//	}
//
//	if (GetAsyncKeyState(VK_ESCAPE) & 1)
//	{
//		break;
//	}
//}

LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hInfiniteAmmo;
	bool bInfAmmo = false;
	switch (message)
	{
	case WM_CREATE:
		hInfiniteAmmo = CreateWindow(L"BUTTON", L"InfAmmo", WS_VISIBLE | WS_CHILD, 100, 100, 100, 100, hWnd, (HMENU)IDC_INFAMMO_BUTTON, hInstance, NULL);
		break;
		
	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case IDC_INFAMMO_BUTTON:
			bInfAmmo = !bInfAmmo;
			if (bInfAmmo == true)
			{
				uintptr_t bulletAddress = Proc::Get()->FindDMAAddy(engineAddress, bulletOffsets);
				*(int*)(bulletAddress) = 1002;
			}
			break;
		}


		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}