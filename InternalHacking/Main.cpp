#include "Global.h"
#include "Proc.h"
#include "Mem.h"
#include "Character.h"

#define IDC_INFAMMO 100
#define IDC_HEALTH 101
#define IDC_RECOIL 102

#define DLL_EXPORT extern "C" __declspec(dllexport)

//Global Variable
HINSTANCE hInstance;
HWND g_hWnd;
DWORD procID;
uintptr_t moduleBase;
HANDLE hProcess;
uintptr_t engineAddress;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void RegisterWndClass();


DWORD WINAPI HackThread(HMODULE hModule)
{
	
	hInstance = hModule;
	MSG msg;
	RegisterWndClass();	
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
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

	UpdateWindow(g_hWnd);

	//모듈 아이디 얻어오기
	moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");
	uintptr_t* player = (uintptr_t*)(moduleBase + 0x10f4f4);
	uintptr_t* entityList = (uintptr_t*)(moduleBase + 0x10f4f8);

	for (int i = 0; i < 32; i++)
	{
		uintptr_t* entity = (uintptr_t*)(*entityList + 0x4 * i);
		Character* character = new Character();
		character->SetCharacterInfo((char*)(*entity+0x255), *(float*)(*entity + 0x44), *(float*)(*entity + 0x40), *(int*)(*entity + 0xF8),)
	}
	while (true)
	{
		if (PeekMessage(&msg, g_hWnd, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			for (int i = 0; i < 32; i++)
			{
				uintptr_t* entity = entities[i];
				if (entity != nullptr)
				{
					static Rotation rotation;
					rotation.Pitch = *(float*)(*entity + 0x44);
					rotation.Yaw = *(float*)(*entity + 0x40);

					cout << rotation.Pitch << endl;
					cout << rotation.Yaw << endl;
				}
			}
			system("cls");
		}
	}
	
	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
	return (DWORD)msg.wParam;
}

void RegisterWndClass()
{
	WNDCLASS wndClass;

	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = CreateSolidBrush(COLORREF(0xf0f0f0));
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hInfiniteAmmo;
	static HWND hRecoil;
	static HWND hHealth;
	static char str[256];
	static bool bInfAmmo = false;
	static bool bRecoil = false;
	static bool bHealth = false;
	switch (message)
	{
	case WM_CREATE:
		hInfiniteAmmo = CreateWindow(L"BUTTON", L"InfAmmo", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 20, 20, 100, 25, hWnd, (HMENU)IDC_INFAMMO, hInstance, NULL);
		hRecoil = CreateWindow(L"BUTTON", L"No Recoil", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 20, 50, 100, 25, hWnd, (HMENU)IDC_RECOIL, hInstance, NULL);
		hHealth = CreateWindow(L"BUTTON", L"Inf Health", WS_VISIBLE | WS_CHILD | BS_CHECKBOX, 20, 80, 100, 25, hWnd, (HMENU)IDC_HEALTH, hInstance, NULL);
		break;
		
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_INFAMMO:
			bInfAmmo = !bInfAmmo;
			if (bInfAmmo == true)
			{
				SendMessage(hInfiniteAmmo, BM_SETCHECK, BST_CHECKED, 0);
				Mem::Get()->Nop((BYTE*)(moduleBase + 0x637E9), 2);
				
			}
			else
			{
				SendMessage(hInfiniteAmmo, BM_SETCHECK, BST_UNCHECKED, 0);
				Mem::Get()->Patch((BYTE*)(moduleBase + 0x637E9), (BYTE*)("\xFF\x0E"), 2);
			}
			break;

		case IDC_RECOIL: {
			bRecoil = !bRecoil;
			if (bRecoil == true)
			{
				Mem::Get()->Nop((BYTE*)(moduleBase+0x63786),10);
				SendMessage(hRecoil, BM_SETCHECK, BST_CHECKED, 0);
			}
			else
			{
				Mem::Get()->Patch((BYTE*)(moduleBase + 0x63786), (BYTE*)("\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2"), 10);
				SendMessage(hRecoil, BM_SETCHECK, BST_UNCHECKED, 0);
			}
			}
			break;

		case IDC_HEALTH: {
			bHealth = !bHealth;
			if (bHealth == true)
			{
				Mem::Get()->Nop((BYTE*)(moduleBase + 0x2CA5E), 6);
				SendMessage(hHealth, BM_SETCHECK, BST_CHECKED, 0);
			}
			else
			{
				Mem::Get()->Patch((BYTE*)(moduleBase + 0x2CA5E), (BYTE*)("\x89\x82\xF8\x00\x00\x00"), 6);
				SendMessage(hHealth, BM_SETCHECK, BST_UNCHECKED, 0);
			}
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
